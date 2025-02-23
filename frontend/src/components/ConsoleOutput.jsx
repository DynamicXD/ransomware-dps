import { useState, useEffect } from "react";
import axios from "axios";

const ConsoleOutput = ({ endpoint }) => {
    const [output, setOutput] = useState("");

    useEffect(() => {
        axios.get(`http://localhost:5000/${endpoint}`)
            .then(response => setOutput(response.data.output || response.data.error))
            .catch(error => setOutput("Error fetching output."));
    }, [endpoint]);

    return (
        <div style={{
            background: "#000",
            color: "#0f0",
            padding: "10px",
            borderRadius: "5px",
            fontFamily: "monospace",
            whiteSpace: "pre-wrap",
            height: "300px",
            overflowY: "auto"
        }}>
            {output || "Executing..."}
        </div>
    );
};

export default ConsoleOutput;
