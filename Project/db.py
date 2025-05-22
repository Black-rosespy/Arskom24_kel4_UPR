# db.py
import sqlite3

def get_db_connection():
    conn = sqlite3.connect('sensor_data.db')
    conn.row_factory = sqlite3.Row
    return conn

def init_db():
    conn = get_db_connection()
    conn.execute('''
        CREATE TABLE IF NOT EXISTS sensor_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            suhu REAL,
            lampu TEXT,
            cahaya TEXT,
            gerakan TEXT
        )
    ''')
    conn.commit()
    conn.close()
