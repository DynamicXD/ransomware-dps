import React, { useState } from "react";
import "./Dashboard.css"; // Updated styles

export default function Dashboard() {
  const [isSidebarOpen, setSidebarOpen] = useState(true);
  const [active, setActive] = useState("Home");

  return (
    <div className="dashboard-container">
      {/* Sidebar */}
      <aside className={`sidebar ${isSidebarOpen ? "open" : "closed"}`}>
        <button className="toggle-btn" onClick={() => setSidebarOpen(!isSidebarOpen)}>
          {isSidebarOpen ? "❌" : "☰"}
        </button>
        <h2 className="sidebar-title">DASHBOARD</h2>
        <nav className="sidebar-menu">
          {["Home", "Honeypot", "USB Monitor", "File Scanner", "Rollback System"].map((item) => (
            <a 
              key={item} 
              href="#" 
              className={`sidebar-link ${active === item ? "active" : ""}`} 
              onClick={() => setActive(item)}
            >
              {item}
            </a>
          ))}
        </nav>
      </aside>

      {/* Main Content */}
      <main className={`main-content ${isSidebarOpen ? "shifted" : ""}`}>
        <div className="hero-section">
          <h1 className="hero-title">Cyber Threat Detection</h1>
          <p className="hero-subtext">Monitor & prevent ransomware attacks in real-time</p>
        </div>

        <div className="dashboard-grid">
          {[
            { title: "🕵 Honeypot Triggers", desc: "Detect ransomware using bait files", color: "#ff4d4d" },
            { title: "🔌 USB Monitor", desc: "Detect unauthorized USB devices", color: "#ffaa00" },
            { title: "📂 File Scanner", desc: "Track suspicious file system changes", color: "#0099ff" },
            { title: "🔄 Rollback System", desc: "Restore files after an attack", color: "#00ff99" },
          ].map((card, index) => (
            <div key={index} className="dashboard-card" style={{ borderColor: card.color }}>
              <h3>{card.title}</h3>
              <p>{card.desc}</p>
              <button className="cta-button">View Details</button>
            </div>
          ))}
        </div>
      </main>
    </div>
  );
}
