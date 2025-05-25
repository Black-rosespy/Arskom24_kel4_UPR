// Fungsi buat update waktu sekarang di UI (opsional)
function updateTime() {
  const now = new Date();
  document.getElementById("currentTime").textContent = now.toLocaleString();
}
setInterval(updateTime, 1000);
updateTime();

// Fungsi utama untuk update elemen dashboard berdasarkan data
function updateDashboard(data) {
  // Update status lampu
  const lampu = document.getElementById("lampuStatus");
  if (data.lampu === "terang") lampu.textContent = "🟢 Terang";
  else if (data.lampu === "redup") lampu.textContent = "🟡 Redup";
  else lampu.textContent = "🔴 Mati";

  // Update nilai lux & status cahaya
  document.getElementById("cahayaLux").textContent = `${data.lux} lux`;
  document.getElementById("cahayaStatus").textContent =
    data.lux > 50 ? "Terang" : "Gelap";

  // Update status gerakan
  document.getElementById("gerakanStatus").textContent = data.gerakan
    ? "🟠 Gerakan Terdeteksi"
    : "🔵 Tidak Ada Gerakan";

  // Tampilkan waktu gerakan jika ada
  document.getElementById("waktuGerak").textContent = data.gerakan
    ? `Waktu: ${new Date().toLocaleTimeString()}`
    : "--";

  // Update suhu
  document.getElementById("suhuNilai").textContent = `${data.suhu} °C`;

  // Simpan deskripsi log terakhir
  if (data.gerakan) {
    lastLogText = "Gerakan terdeteksi – Lampu menyala terang";
  } else if (data.lux > 50) {
    lastLogText = "Cahaya cukup – Lampu dimatikan";
  } else {
    lastLogText = "Tidak ada gerakan – Lampu meredup";
  }
}

// Event saat tombol Simpan Log diklik
document.getElementById("simpanLogBtn").addEventListener("click", () => {
  const logList = document.getElementById("logList");
  const li = document.createElement("li");
  li.textContent = `[${new Date().toLocaleTimeString()}] ${lastLogText}`;
  logList.prepend(li); // Tambahkan log ke atas
});

// Simulasi data sensor per 5 detik (jika ESP32 belum tersambung)
setInterval(() => {
  const simulatedData = {
    lampu:
      Math.random() < 0.3 ? "mati" : Math.random() < 0.5 ? "redup" : "terang",
    lux: Math.floor(Math.random() * 100),
    gerakan: Math.random() < 0.4,
    suhu: (25 + Math.random() * 10).toFixed(1),
    log: "",
  };

  // Tentukan pesan log berdasarkan kondisi
  if (simulatedData.gerakan) {
    simulatedData.log = "Gerakan terdeteksi – Lampu menyala terang";
  } else if (simulatedData.lux > 50) {
    simulatedData.log = "Cahaya cukup – Lampu dimatikan";
  } else {
    simulatedData.log = "Tidak ada gerakan – Lampu meredup";
  }

  // Update dashboard dengan data simulasi
  updateDashboard(simulatedData);
}, 5000);

// Variabel global untuk simpan deskripsi log terakhir
let lastLogText = "";
