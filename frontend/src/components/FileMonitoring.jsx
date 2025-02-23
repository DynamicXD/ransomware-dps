// FileMonitoring.jsx
import React, { useState, useEffect } from 'react';
import '../styles/MonitoringDashboard.css';

const API_BASE_URL = 'http://localhost:8000/api';
const WS_URL = 'ws://localhost:8000/ws';

const FileMonitoring = () => {
    const [events, setEvents] = useState([]);
    const [stats, setStats] = useState({
        suspicious: 0,
        modified: 0,
        deleted: 0,
        created: 0,
        renamed: 0
    });
    const [lockedFolders, setLockedFolders] = useState([]);
    const [isMonitoring, setIsMonitoring] = useState(false);
    const [directory, setDirectory] = useState('');
    const [ws, setWs] = useState(null);

    useEffect(() => {
        if (ws) {
            ws.onmessage = (event) => {
                const data = JSON.parse(event.data);
                handleWebSocketMessage(data);
            };
        }
    }, [ws]);

    const handleWebSocketMessage = (data) => {
        switch (data.type) {
            case 'file_event':
                setEvents(prev => [data.data, ...prev].slice(0, 50));
                break;
            case 'spike_alert':
                alert("🚨 Spike Alert: " + data.data.message);
                break;
            case 'folder_locked':
                setLockedFolders(prev => [...prev, data.data.folder]);
                break;
            case 'folder_unlocked':
                setLockedFolders(prev => prev.filter(f => f !== data.data.folder));
                break;
            default:
                console.log("Unknown event:", data);
        }
    };

    const startMonitoring = async () => {
        try {
            const response = await fetch(`${API_BASE_URL}/monitor/start`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ directory })
            });
            const data = await response.json();
            if (response.ok) {
                setIsMonitoring(true);
                setWs(new WebSocket(WS_URL)); // Establish WebSocket connection
                fetchStats();
            } else {
                alert(`Error: ${data.detail}`);
            }
        } catch (error) {
            console.error("Failed to start monitoring:", error);
        }
    };

    const stopMonitoring = async () => {
        try {
            const response = await fetch(`${API_BASE_URL}/monitor/stop`, { method: 'POST' });
            const data = await response.json();
            if (response.ok) {
                setIsMonitoring(false);
                if (ws) {
                    ws.close();
                    setWs(null);
                }
            }
            alert(data.status);
        } catch (error) {
            console.error("Failed to stop monitoring:", error);
        }
    };

    const fetchStats = async () => {
        try {
            const response = await fetch(`${API_BASE_URL}/stats`);
            const data = await response.json();
            setStats({
                suspicious: data.suspicious_files,
                modified: data.total_events, // Approximate count
                deleted: 0, // Not tracked separately in backend
                created: 0,
                renamed: 0
            });
            setLockedFolders(data.locked_folders);
        } catch (error) {
            console.error("Failed to fetch stats:", error);
        }
    };

    return (
        <div className="monitoring-dashboard">
            <header className="dashboard-header">
                <h1>File System Monitor</h1>
                <input 
                    type="text" 
                    placeholder="Enter directory path" 
                    value={directory} 
                    onChange={(e) => setDirectory(e.target.value)}
                />
                <button 
                    className={`monitor-button ${isMonitoring ? 'active' : ''}`}
                    onClick={isMonitoring ? stopMonitoring : startMonitoring}
                >
                    {isMonitoring ? 'Stop Monitoring' : 'Start Monitoring'}
                </button>
            </header>

            <div className="stats-container">
                <div className="stat-card">
                    <h3>Suspicious Files</h3>
                    <p className="stat-value">{stats.suspicious}</p>
                    <span className="stat-icon">⚠️</span>
                </div>
                <div className="stat-card">
                    <h3>Modified Files</h3>
                    <p className="stat-value">{stats.modified}</p>
                    <span className="stat-icon">📝</span>
                </div>
                <div className="stat-card">
                    <h3>Monitor Status</h3>
                    <p className="stat-value">{isMonitoring ? 'Active' : 'Inactive'}</p>
                    <span className="stat-icon">{isMonitoring ? '🟢' : '🔴'}</span>
                </div>
            </div>

            <div className="main-content">
                <section className="events-section">
                    <h2>Recent Events</h2>
                    <div className="events-list">
                        {events.map((event, index) => (
                            <div key={index} className="event-item">
                                <span className="event-icon">📝</span>
                                <div className="event-details">
                                    <span className="event-type">{event.type}</span>
                                    <span className="event-path">{event.path}</span>
                                </div>
                                <span className="event-time">
                                    {new Date(event.timestamp).toLocaleTimeString()}
                                </span>
                            </div>
                        ))}
                    </div>
                </section>

                {lockedFolders.length > 0 && (
                    <section className="locked-folders-section">
                        <h2>Locked Folders</h2>
                        <div className="locked-folders-list">
                            {lockedFolders.map((folder, index) => (
                                <div key={index} className="locked-folder-item">
                                    <span className="folder-icon">🔒</span>
                                    <span className="folder-path">{folder}</span>
                                    <button 
                                        className="unlock-button"
                                        onClick={async () => {
                                            try {
                                                const response = await fetch(`${API_BASE_URL}/folders/unlock/${folder}`, { method: 'POST' });
                                                const data = await response.json();
                                                alert(data.status);
                                                setLockedFolders(prev => prev.filter(f => f !== folder));
                                            } catch (error) {
                                                console.error("Failed to unlock folder:", error);
                                            }
                                        }}
                                    >
                                        Unlock 🔓
                                    </button>
                                </div>
                            ))}
                        </div>
                    </section>
                )}
            </div>
        </div>
    );
};

export default FileMonitoring;
