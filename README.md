---

# 💡 Smart Streetlight System - ESP32 + Web Dashboard

Sistem ini adalah proyek IoT berbasis ESP32 yang memantau dan mengontrol lampu jalan pintar menggunakan sensor suhu (DHT11), sensor cahaya (LDR), dan sensor jarak (ultrasonik), dengan fitur buzzer notifikasi. Data dikirim ke backend Python Flask dan ditampilkan secara real-time di dashboard web.

---

## 📁 Struktur Proyek

bash
.
├── ESP32_Firmware.ino        # Firmware untuk ESP32 (kode C++)
├── Backend.py                # Backend Flask untuk menerima dan menyimpan data
├── db.py                     # Modul koneksi database SQLite (tidak disertakan di sini)
├── Index.html                # Halaman dashboard web utama
├── style.css                 # Style dashboard web (versi alternatif)
├── script.js                 # JS alternatif untuk dashboard simulasi


---

## 🔌 Komponen Hardware

* ESP32
* Sensor DHT11 (suhu)
* Sensor LDR (cahaya)
* Sensor ultrasonik HC-SR04 (gerakan)
* Buzzer
* LED
* Modul WiFi internal ESP32

---

## ⚙ Firmware ESP32

File: ESP32_Firmware.ino

### Fitur:

* Menghubungkan ke WiFi.
* Membaca suhu dari DHT11, intensitas cahaya dari LDR, dan jarak dari sensor ultrasonik.
* Mengontrol LED secara otomatis:

  * Hidup 100% jika objek terdeteksi dan kondisi tidak terlalu terang.
  * Hidup 60% jika gelap dan tidak ada gerakan.
  * Mati jika sangat terang.
* Mengaktifkan buzzer saat:

  * Terang ekstrem (notifikasi LDR).
  * Objek baru terdeteksi (notifikasi gerakan).
* Mengirim data ke server Flask setiap detik dalam format JSON.

---

## 🌐 Backend (Python Flask)

File: Backend.py

### Endpoint:

* POST /update: Terima data dari ESP32.
* GET /monitor: Ambil data terbaru (real-time) untuk dashboard.
* POST /save-log: Simpan data ke SQLite (sensor_logs) sebagai log.
* GET /logs: (sementara hanya dummy array).

### Catatan:

Backend ini pakai db.py untuk koneksi SQLite, dan CORS sudah diaktifkan agar bisa diakses dari frontend.

---

## 🖥 Dashboard Web

File: Index.html (utama), style.css, script.js (simulasi)

### Fitur:

* Tampilkan data suhu, status lampu, deteksi gerakan, dan kondisi cahaya secara live.
* Warna status berubah (hijau, kuning, merah) sesuai kondisi.
* Tombol Simpan Log untuk simpan snapshot data ke backend.
* Simulasi visual di script.js (untuk pengujian tanpa ESP32).

---

## 🔧 Cara Menjalankan

### 1. Siapkan ESP32

* Upload ESP32_Firmware.ino via Arduino IDE.
* Pastikan ssid, password, dan serverName disesuaikan.

### 2. Jalankan Backend

bash
pip install flask flask-cors
python Backend.py


### 3. Buka Dashboard Web

* Buka Index.html di browser.
* Pastikan alamat backend sesuai (http://localhost:5000).

---

## 🧪 Catatan Tambahan

* Untuk analogWrite() di ESP32, kamu bisa gunakan ledcWrite() karena ESP32 tidak support analogWrite() secara langsung.
* Pastikan IP server (pada serverName) cocok dengan IP lokal PC kamu.
* Tambahkan file db.py untuk setup database SQLite, jika belum dibuat.

---

## 📷 Tampilan Dashboard

Dashboard menampilkan status real-time dalam bentuk kartu dan tombol interaksi seperti simpan log. Cocok untuk monitoring sistem lampu pintar berbasis sensor.

---