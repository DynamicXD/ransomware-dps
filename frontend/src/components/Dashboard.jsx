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

  const handleDownload = (e, url, feature, type) => {
    e.preventDefault();
    console.log(`Downloading ${type} for ${feature}`);
    window.open(url, '_blank');
  };

  const zipurls = [
    "https://github.com/user-attachments/files/18928476/honeypot.zip",
    "https://github.com/user-attachments/files/18928479/usb.zip",
    "https://github.com/user-attachments/files/18928477/monitor.zip",
    "https://github.com/user-attachments/files/18928474/backup.zip",
    "https://github.com/user-attachments/files/18928475/file_sign.zip",
    "https://github.com/user-attachments/files/18928478/ram_and_process.zip",
  ];

  const texturls = [
    "https://github.com/user-attachments/files/18928570/honeypot.md",
    "https://github.com/user-attachments/files/18928572/usb_manual.md",
    "https://github.com/user-attachments/files/18928573/monitor.md",
    "https://github.com/user-attachments/files/18928575/rollback.md",
    "https://github.com/user-attachments/files/18928571/file_sign_manual.md",
    "https://github.com/user-attachments/files/18928574/ram_and_process_readme.md"
  ];

  const features = [
    { title: " Honeypot Triggers", desc: "Detect ransomware using bait files", color: "#ff4d4d", feature: "honeypot" },
    { title: " USB Monitor", desc: "Detect unauthorized USB devices", color: "#ffaa00", feature: "usb" },
    { title: " File Monitoring", desc: "Track suspicious file system changes", color: "#0099ff", feature: "filesystem" },
    { title: " Rollback System", desc: "Restore files after an attack", color: "#00ff99", feature: "rollback" },
    { title: " File Signature", desc: "Monitor network traffic for anomalies", color: "#9966ff", feature: "network" },
    { title: " Analyze RAM and Process Anomalies", desc: "Enhance security with real-time firewall monitoring", color: "#ff66cc", feature: "firewall" }
  ];

  return (
    <div className="dashboard-container">
      <div className="overlay"></div>
      
      <nav className="top-nav">
        <div className="navbar-container">
          <div className="logo">0xpwnsec</div>
          <ul className="nav-menu">
            {["Home", "Honeypot", "USB Monitor", "Logs", "File Signature"].map((item) => (
              <li 
                key={item} 
                className={`nav-item ${active === item ? "active" : ""}`} 
                onClick={() => handleNavigation(item)}
              >
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
          <div className="dashboard-row">
            {features.slice(0, 3).map((item, index) => (
              <div className="dashboard-card" key={index} style={{ borderColor: item.color }}>
                <h3>{item.title}</h3>
                <p>{item.desc}</p>
                <div className="button-group">
                  <a
                    href={zipurls[index]}
                    className="cta-button"
                    onClick={(e) => handleDownload(e, zipurls[index], item.feature, 'script')}
                    download
                  >
                    Download Script
                  </a>
                  <a
                    href={texturls[index]}
                    className="cta-button"
                    onClick={(e) => handleDownload(e, texturls[index], item.feature, 'manual')}
                    download
                  >
                    Download Manual
                  </a>
                </div>
              </div>
            ))}
          </div>
          <div className="dashboard-row">
            {features.slice(3, 6).map((item, index) => (
              <div className="dashboard-card" key={index + 3} style={{ borderColor: item.color }}>
                <h3>{item.title}</h3>
                <p>{item.desc}</p>
                <div className="button-group">
                  <a
                    href={zipurls[index + 3]}
                    className="cta-button"
                    onClick={(e) => handleDownload(e, zipurls[index + 3], item.feature, 'script')}
                    download
                  >
                    Download Script
                  </a>
                  <a
                    href={texturls[index + 3]}
                    className="cta-button"
                    onClick={(e) => handleDownload(e, texturls[index + 3], item.feature, 'manual')}
                    download
                  >
                    Download Manual
                  </a>
                </div>
              </div>
            ))}
          </div>
        </div>
      </main>
    </div>
  );
}