import ConsoleOutput from "../components/ConsoleOutput";

function FileSignature() {
    return (
        <div>
            <h1>Security Modules</h1>
            <h2>File Signature Based</h2>
            <ConsoleOutput endpoint="run-python" />
        </div>
    );
}

export default FileSignature;
