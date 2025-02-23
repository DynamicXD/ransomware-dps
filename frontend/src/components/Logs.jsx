import React, { useState, useEffect } from "react";
import axios from "axios";

const Logs = () => {
    const [logs, setLogs] = useState([]);
    const [latestAlert, setLatestAlert] = useState(null);

    useEffect(() => {
        const fetchLogs = () => {
            axios.get("http://localhost:5173/logs")
                .then(res => {
                    setLogs(res.data.logs);
                    const lastLog = res.data.logs[res.data.logs.length - 1];
                    if (lastLog.includes("[ALERT]")) {
                        setLatestAlert(lastLog);
                    }
                })
                .catch(() => setLogs(["Error fetching logs."]));
        };

        fetchLogs();
        const interval = setInterval(fetchLogs, 3000);
        return () => clearInterval(interval);
    }, []);

    return (
        <div>
            {latestAlert && (
                <div style={{ color: "red", fontWeight: "bold", padding: "10px", border: "1px solid red" }}>
                     ALERT: {latestAlert}
                </div>
            )}
            <h3>Activity Log</h3>
            <div style={{ height: "300px", overflowY: "scroll", border: "1px solid black", padding: "10px" }}>
                {logs.map((log, index) => <p key={index}>{log}</p>)}
            </div>
        </div>
    );
};

export default Logs;
