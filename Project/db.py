# db.py
import sqlite3

# Fungsi untuk buka koneksi ke database SQLite lokal
def get_db_connection():
    conn = sqlite3.connect('sensor_data.db')  # File database
    conn.row_factory = sqlite3.Row           # Supaya hasil query bisa diakses pakai nama kolom
    return conn

# Fungsi untuk inisialisasi database jika belum ada
def init_db():
    conn = get_db_connection()
    conn.execute('''
        CREATE TABLE IF NOT EXISTS sensor_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,  -- Otomatis ambil waktu saat data dimasukkan
            suhu REAL,     -- Data suhu dari DHT11
            lampu TEXT,    -- Status lampu (Hidup/Mati)
            cahaya TEXT,   -- Terang atau Gelap
            gerakan TEXT   -- Terdeteksi / Tidak Terdeteksi
        )
    ''')
    conn.commit()
    conn.close()
