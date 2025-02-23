import React from 'react'
import Dashboard from './components/Dashboard';
import { Routes, Route } from 'react-router-dom';
import FileSignature from './components/FileSignature'
import HoneyPot from './components/HoneyPot';

const App = () => {
    return (
        <Routes>
            <Route path='/' element={<Dashboard />} />
            <Route path='/FileSignature' element={<FileSignature />} />
            <Route path='/Honeypot' element={<HoneyPot />} />
        </Routes>
    )
}

export default App