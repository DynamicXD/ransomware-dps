from flask import Flask, jsonify
import subprocess
from threading import Thread

app = Flask(__name__)

@app.after_request
def after_request(response):
    # Set CORS headers
    response.headers.add('Access-Control-Allow-Origin', '*')
    response.headers.add('Access-Control-Allow-Headers', 'Content-Type')
    response.headers.add('Access-Control-Allow-Methods', 'GET')
    return response

def run_python_script():
    result = subprocess.run(['py', 'main.py'], capture_output=True, text=True)
    return result.stdout if result.returncode == 0 else result.stderr

def run_c_program():
    compile_result = subprocess.run(['gcc', 'main2.c', '-o', 'main3.exe', '-std=c99', '-lpsapi', '-lshlwapi'], capture_output=True, text=True)
    if compile_result.returncode != 0:
        return compile_result.stderr
    run_result = subprocess.run(['./main3.exe'], capture_output=True, text=True)
    return run_result.stdout if run_result.returncode == 0 else run_result.stderr

@app.route('/run-python', methods=['GET'])
def run_python():
    try:
        # Run the Python script in a separate thread
        thread = Thread(target=run_python_script)
        thread.start()
        thread.join()  # Wait for the thread to finish
        output = run_python_script()
        return jsonify({'output': output})
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/run-c', methods=['GET'])
def run_c():
    try:
        # Run the C program in a separate thread
        thread = Thread(target=run_c_program)
        thread.start()
        thread.join()  # Wait for the thread to finish
        output = run_c_program()
        return jsonify({'output': output})
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True)