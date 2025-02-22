import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import App from './Dashboard.jsx'
import Dashboard from './Dashboard.jsx'

createRoot(document.getElementById('root')).render(
  <StrictMode>
    <Dashboard />
  </StrictMode>,
)
