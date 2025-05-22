#include <WiFi.h>
#include <HTTPClient.h>
#include "DHTesp.h"

// --- Konfigurasi WiFi ---
const char* ssid = "inikanki";      // Ganti dengan nama WiFi Anda
const char* password = "28052006"; // Ganti dengan password WiFi Anda
const String serverName = "http://192.168.54.123:5000/update"; // Ganti dengan IP PC kamu dan port server Python

// --- Definisikan Pin ---
const int ledControlPin = 25; // Pin ESP32 untuk LED kontrol (via nMOS)
const int ldrAnalogPin = 35;  // Pin AO LDR
const int dhtPin = 32;        // Pin DHT11
const int trigPin = 33;       // Pin Trig sensor ultrasonik
const int echoPin = 26;       // Pin Echo sensor ultrasonik
const int buzzerPin = 27;     // Pin Buzzer

// --- Objek Sensor ---
DHTesp dht;

// --- Konfigurasi Batas dan Durasi ---
const int ldrTerangMaksimal = 600;  // LDR <= ini dianggap terang maksimal (LED mati)
const int ldrGelapMaksimal = 4095;  // LDR = ini dianggap gelap (LED 60%)
const int jarakDeteksi = 18;        // Batas jarak deteksi objek (cm)

// Durasi dan frekuensi buzzer
const int buzzerLdrFreq = 1000;
const int buzzerLdrDelay = 200;

const int buzzerObjekFreq = 1500;
const int buzzerObjekDelay = 150;
const int buzzerObjekCount = 2;

// --- Variabel State ---
bool buzzerLdrAktif = false;          // State untuk buzzer LDR
bool objekTerdeteksiSebelumnya = false; // State untuk deteksi objek (agar buzzer tidak berulang)

// --- Fungsi Setup ---
void setup() {
  // Inisialisasi Serial untuk debugging
  Serial.begin(115200);
  Serial.println("Sistem Kontrol Rumah Pintar Aktif!");

  // Konfigurasi Pin
  pinMode(ledControlPin, OUTPUT);
  pinMode(ldrAnalogPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // Pastikan buzzer mati saat inisialisasi

  // Inisialisasi Sensor DHT11
  dht.setup(dhtPin, DHTesp::DHT11);

  // Inisialisasi WiFi
  setupWiFi();
}

// --- Fungsi Loop Utama ---
void loop() {
  // --- Baca Sensor ---
  int nilaiLdrAnalog = analogRead(ldrAnalogPin);
  float suhuC = dht.getTemperature(); // Menggunakan suhuC agar sesuai dengan payload JSON
  long durasiUltrasonik = ukurJarak();
  float jarakCm = durasiToCm(durasiUltrasonik);

  // --- Logika Deteksi dan Kontrol ---
  bool objekSaatIniTerdeteksi = (jarakCm <= jarakDeteksi);
  String statusLampu = ""; // Variabel untuk status lampu di JSON
  String kondisiCahaya = ""; // Variabel untuk kondisi cahaya di JSON
  String statusGerakan = objekSaatIniTerdeteksi ? "Terdeteksi" : "Tidak Terdeteksi"; // Variabel untuk gerakan di JSON

  // --- Fitur 4: Notifikasi Buzzer LDR ---
  if (nilaiLdrAnalog <= ldrTerangMaksimal && !buzzerLdrAktif) {
    tone(buzzerPin, buzzerLdrFreq);
    delay(buzzerLdrDelay);
    noTone(buzzerPin);
    buzzerLdrAktif = true;
    Serial.println("Notifikasi: Cahaya Terang Terdeteksi!");
  } else if (nilaiLdrAnalog > ldrTerangMaksimal) {
    buzzerLdrAktif = false;
  }

  // --- Fitur 2: Deteksi Objek & Kontrol LED 100% ---
  if (objekSaatIniTerdeteksi) {
    if (!objekTerdeteksiSebelumnya) {
      Serial.println("Objek terdeteksi!");
      for (int i = 0; i < buzzerObjekCount; i++) {
        tone(buzzerPin, buzzerObjekFreq);
        delay(buzzerObjekDelay);
        noTone(buzzerPin);
        if (i < buzzerObjekCount - 1) {
          delay(buzzerObjekDelay / 2); // Jeda lebih singkat antar bunyi
        }
      }
      objekTerdeteksiSebelumnya = true;
    }

    if (nilaiLdrAnalog > ldrTerangMaksimal) {
      analogWrite(ledControlPin, 255); // LED 100%
      statusLampu = "Hidup 100%";
    } else {
      analogWrite(ledControlPin, 0); // LED mati jika sangat terang
      statusLampu = "Mati";
    }
  } else {
    objekTerdeteksiSebelumnya = false; // Reset state objek terdeteksi
    // --- Fitur 1: Kontrol LED Berdasarkan LDR (tanpa objek) ---
    if (nilaiLdrAnalog <= ldrTerangMaksimal) { // LDR <= 600 (terang)
      analogWrite(ledControlPin, 0); // LED mati
      statusLampu = "Mati";
    } else if (nilaiLdrAnalog >= ldrGelapMaksimal) { // LDR = 4095 (gelap)
      analogWrite(ledControlPin, map(60, 0, 100, 0, 255)); // LED 60%
      statusLampu = "Hidup 60%";
    } else {
      analogWrite(ledControlPin, 0); // Kondisi tengah, LED mati
      statusLampu = "Mati"; // Sesuaikan jika ada logika kecerahan progresif di sini
    }
  }

// --- Tentukan Kondisi Cahaya untuk JSON ---
// Asumsi: nilai LDR lebih rendah berarti lebih terang, nilai LDR lebih tinggi berarti lebih gelap
if (nilaiLdrAnalog <= ldrTerangMaksimal) { // Anda bisa menyesuaikan ambang batas ini
  kondisiCahaya = "Terang";
} else {
  kondisiCahaya = "Gelap";
}

  // --- Kirim Data ke Web Server ---
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{";
    jsonPayload += "\"suhu\":" + String(suhuC, 1) + ","; // Suhu dengan 1 desimal
    jsonPayload += "\"lampu\":\"" + statusLampu + "\",";
    jsonPayload += "\"cahaya\":\"" + kondisiCahaya + "\",";
    jsonPayload += "\"gerakan\":\"" + statusGerakan + "\"";
    jsonPayload += "}";

    Serial.print("Mengirim data: ");
    Serial.println(jsonPayload);

    int httpResponseCode = http.POST(jsonPayload);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    http.end();
  } else {
    Serial.println("WiFi tidak terhubung, tidak dapat mengirim data.");
  }

  // --- Delay untuk Stabilitas ---
  delay(1000); // Penyesuaian delay agar tidak terlalu sering mengirim data dan membaca sensor
}

// --- Fungsi Pembantu (Helper Functions) ---

// Inisialisasi koneksi WiFi
void setupWiFi() {
  Serial.print("Menghubungkan ke WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerkoneksi ke WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Mengukur jarak dengan sensor ultrasonik
long ukurJarak() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH);
}

// Mengkonversi durasi gema menjadi jarak dalam centimeter
float durasiToCm(long durasi) {
  return durasi * 0.0343 / 2; // Kecepatan suara 0.0343 cm/µs
}