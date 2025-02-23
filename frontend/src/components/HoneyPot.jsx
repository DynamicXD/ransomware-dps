import React, { useEffect, useState } from 'react';

function OutputPage() {
  const [output, setOutput] = useState('');
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');

  useEffect(() => {
    // Fetch the output from the backend when the component mounts
    const fetchOutput = async () => {
      try {
        const response = await fetch('http://localhost:5000/run-c'); // Change to /run-c for C
        const data = await response.json();
        if (response.ok) {
          setOutput(data.output);
        } else {
          setError(data.error || 'Failed to fetch output');
        }
      } catch (err) {
        setError('An error occurred while fetching the output');
      } finally {
        setLoading(false);
      }
    };

    fetchOutput();
  }, []);

  if (loading) {
    return <div>Loading...</div>;
  }

  if (error) {
    return <div>Error: {error}</div>;
  }

  return (
    <div>
      <h1>Output Page</h1>
      <pre>{output}</pre>
    </div>
  );
}

export default OutputPage;