import React, { useState } from "react";
import "../styles/Dashboard.css"; // Updated styles
import { useNavigate } from 'react-router-dom';

export default function Dashboard() {
  const [active, setActive] = useState("Home");
  const navigate = useNavigate()

  return (
    <div className="dashboard-container">
      {/* Background Overlay */}
      <div className="overlay"></div>

      {/* Top Navigation Bar */}
      <nav className="top-nav">
        <div className="navbar-container">
          <div className="logo">YourLogo</div>
          <ul className="nav-menu">
            {["Home", "Honeypot", "USB Monitor", "File Scanner", "File Signature"].map((item) => (
              <li
                key={item}
                className={`nav-item ${active === item ? "active" : ""}`}
                onClick={() => {
                  setActive(item)
                  navigate(`/${item.split(" ").join("")}`)
                }}
              >
                {item}
              </li>
            ))}
          </ul>
          <button className="sign-in-btn">Sign In</button>
        </div>
      </nav>

      {/* Main Content */}
      <main className="main-content">
        <div className="hero-section">
          <h1 className="hero-title">Cyber Threat Detection</h1>
          <p className="hero-subtext">Monitor & prevent ransomware attacks in real-time</p>
        </div>

        {/* Centered Grid */}
        <div className="dashboard-grid">
          <div className="dashboard-row">
            <div className="dashboard-card" style={{ borderColor: "#ff4d4d" }}>
              <h3>🕵 Honeypot Triggers</h3>
              <p>Detect ransomware using bait files</p>
              <button className="cta-button">View Details</button>
            </div>
            <div className="dashboard-card" style={{ borderColor: "#ffaa00" }}>
              <h3>🔌 USB Monitor</h3>
              <p>Detect unauthorized USB devices</p>
              <button className="cta-button">View Details</button>
            </div>
          </div>
          <div className="dashboard-row">
            <div className="dashboard-card" style={{ borderColor: "#0099ff" }}>
              <h3>📂 File Scanner</h3>
              <p>Track suspicious file system changes</p>
              <button className="cta-button">View Details</button>
            </div>
            <div className="dashboard-card" style={{ borderColor: "#00ff99" }}>
              <h3>🔄 Rollback System</h3>
              <p>Restore files after an attack</p>
              <button className="cta-button">View Details</button>
            </div>
          </div>
        </div>
      </main>
    </div>
  );
}
