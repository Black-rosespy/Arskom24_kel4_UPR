from flask import Flask, request, jsonify
from flask_cors import CORS
import datetime

app = Flask(__name__)
from db import get_db_connection, init_db
init_db()  # Taruh di bawah app = Flask(__name__)
CORS(app)

latest_data = {
    "lampu": "-",
    "gerakan": "-",
    "cahaya": "-",
    "suhu": "-"
}
log_data = []

@app.route('/update', methods=['POST'])
def update_data():
    global latest_data
    latest_data = request.json
    return jsonify({"message": "Data diterima"}), 200

@app.route('/monitor', methods=['GET'])
def get_monitor_data():
    return jsonify(latest_data)

@app.route('/save-log', methods=['POST'])
def save_log():
    conn = get_db_connection()
    conn.execute('''
        INSERT INTO sensor_logs (suhu, lampu, cahaya, gerakan)
        VALUES (?, ?, ?, ?)
    ''', (
        latest_data.get("suhu"),
        latest_data.get("lampu"),
        latest_data.get("cahaya"),
        latest_data.get("gerakan")
    ))
    conn.commit()
    conn.close()

    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    return jsonify({"message": "Log disimpan", "timestamp": timestamp}), 200

@app.route('/logs', methods=['GET'])
def get_logs():
    return jsonify(log_data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)

