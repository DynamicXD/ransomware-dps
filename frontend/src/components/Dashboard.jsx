import React, { useState } from "react";
import "../styles/Dashboard.css";
import { useNavigate } from 'react-router-dom';

export default function Dashboard() {
  const [active, setActive] = useState("Home");
  const navigate = useNavigate();

  const handleNavigation = (route) => {
    setActive(route);
    navigate(`/${route.split(" ").join("")}`);
  };

  const handleDownloadScript = (feature) => {
    console.log(`Downloading script for ${feature}`);
  };

  const features = [
    { title: "🕵 Honeypot Triggers", desc: "Detect ransomware using bait files", color: "#ff4d4d", feature: "honeypot" },
    { title: "🔌 USB Monitor", desc: "Detect unauthorized USB devices", color: "#ffaa00", feature: "usb" },
    { title: "📂 File Monitoring", desc: "Track suspicious file system changes", color: "#0099ff", feature: "filesystem" },
    { title: "🔄 Rollback System", desc: "Restore files after an attack", color: "#00ff99", feature: "rollback" },
    { title: "🌐 Network Analysis", desc: "Monitor network traffic for anomalies", color: "#9966ff", feature: "network" },
    { title: "🛡 Firewall Protection", desc: "Enhance security with real-time firewall monitoring", color: "#ff66cc", feature: "firewall" }
  ];

  return (
    <div className="dashboard-container">
      <div className="overlay"></div>
      
      <nav className="top-nav">
        <div className="navbar-container">
          <div className="logo">0xpwnsec</div>
          <ul className="nav-menu">
            {["Home", "Honeypot", "USB Monitor", "Logs", "File Signature"].map((item) => (
              <li key={item} className={`nav-item ${active === item ? "active" : ""}`} onClick={() => handleNavigation(item)}>
                {item}
              </li>
            ))}
          </ul>
          <button className="sign-in-btn">Sign In</button>
        </div>
      </nav>

      <main className="main-content">
        <div className="hero-section">
          <h1 className="hero-title">Cyber Threat Detection</h1>
          <p className="hero-subtext">Monitor & prevent ransomware attacks in real-time</p>
        </div>

        <div className="dashboard-grid">
          <div className="dashboard-row center-row">
            {features.slice(0, 3).map((item, index) => (
              <div className="dashboard-card" key={index} style={{ borderColor: item.color }}>
                <h3>{item.title}</h3>
                <p>{item.desc}</p>
                <div style={{ display: 'flex', gap: '8px', marginTop: '10px' }}>
                  <button className="cta-button" onClick={() => handleDownloadScript(item.feature)}>
                    Download Script
                  </button>
                  <button className="cta-button" onClick={() => handleDownloadScript(`${item.feature}-manual`)}>
                    Download Manual
                  </button>
                </div>
              </div>
            ))}
          </div>
          <div className="dashboard-row">
            {features.slice(3, 6).map((item, index) => (
              <div className="dashboard-card" key={index} style={{ borderColor: item.color }}>
                <h3>{item.title}</h3>
                <p>{item.desc}</p>
                <div style={{ display: 'flex', gap: '8px', marginTop: '10px' }}>
                  <button className="cta-button" onClick={() => handleDownloadScript(item.feature)}>
                    Download Script
                  </button>
                  <button className="cta-button" onClick={() => handleDownloadScript(`${item.feature}-manual`)}>
                    Download Manual
                  </button>
                </div>
              </div>
            ))}
          </div>
        </div>
      </main>
    </div>
  );
}