from flask import Flask, request, jsonify
from flask_cors import CORS
import datetime

# Inisialisasi Flask app
app = Flask(__name__)

# Import fungsi dari db.py dan inisialisasi database
from db import get_db_connection, init_db
init_db()

# Aktifkan CORS agar frontend bisa akses backend ini
CORS(app)

# Variabel global untuk menyimpan data terbaru dari ESP32
latest_data = {
    "lampu": "-",
    "gerakan": "-",
    "cahaya": "-",
    "suhu": "-"
}

# (Opsional) Menyimpan data log untuk akses cepat (bukan database)
log_data = []

# Endpoint untuk menerima data dari ESP32
@app.route('/update', methods=['POST'])
def update_data():
    global latest_data
    latest_data = request.json  # Ambil data JSON dari ESP32
    return jsonify({"message": "Data diterima"}), 200

# Endpoint untuk diakses oleh dashboard frontend
@app.route('/monitor', methods=['GET'])
def get_monitor_data():
    return jsonify(latest_data)

# Endpoint untuk menyimpan data terbaru ke database SQLite
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

    # Buat timestamp untuk log
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    return jsonify({"message": "Log disimpan", "timestamp": timestamp}), 200

# (Opsional) Endpoint dummy untuk ambil log yang disimpan di RAM
@app.route('/logs', methods=['GET'])
def get_logs():
    return jsonify(log_data)

# Jalankan server di port 5000
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
