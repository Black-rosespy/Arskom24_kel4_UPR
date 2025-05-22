// Fungsi untuk menampilkan waktu sekarang
function updateTime() {
  const now = new Date();
  document.getElementById("currentTime").textContent = now.toLocaleString();
}
setInterval(updateTime, 1000);
updateTime();

// Simulasi data dari ESP32
function updateDashboard(data) {
  // Lampu
  const lampu = document.getElementById("lampuStatus");
  if (data.lampu === "terang") lampu.textContent = "🟢 Terang";
  else if (data.lampu === "redup") lampu.textContent = "🟡 Redup";
  else lampu.textContent = "🔴 Mati";

  // Cahaya
  document.getElementById("cahayaLux").textContent = `${data.lux} lux`;
  document.getElementById("cahayaStatus").textContent =
    data.lux > 50 ? "Terang" : "Gelap";

  // Gerakan
  document.getElementById("gerakanStatus").textContent = data.gerakan
    ? "🟠 Gerakan Terdeteksi"
    : "🔵 Tidak Ada Gerakan";
  document.getElementById("waktuGerak").textContent = data.gerakan
    ? `Waktu: ${new Date().toLocaleTimeString()}`
    : "--";

  // Suhu
  document.getElementById("suhuNilai").textContent = `${data.suhu} °C`;

  // Simpan log terakhir tapi belum ditampilkan
  if (data.gerakan) {
    lastLogText = "Gerakan terdeteksi – Lampu menyala terang";
  } else if (data.lux > 50) {
    lastLogText = "Cahaya cukup – Lampu dimatikan";
  } else {
    lastLogText = "Tidak ada gerakan – Lampu meredup";
  }
}

document.getElementById("simpanLogBtn").addEventListener("click", () => {
  const logList = document.getElementById("logList");
  const li = document.createElement("li");
  li.textContent = `[${new Date().toLocaleTimeString()}] ${lastLogText}`;
  logList.prepend(li);
});

// Simulasi data per 5 detik
setInterval(() => {
  const simulatedData = {
    lampu:
      Math.random() < 0.3 ? "mati" : Math.random() < 0.5 ? "redup" : "terang",
    lux: Math.floor(Math.random() * 100),
    gerakan: Math.random() < 0.4,
    suhu: (25 + Math.random() * 10).toFixed(1),
    log: "",
  };

  if (simulatedData.gerakan) {
    simulatedData.log = "Gerakan terdeteksi – Lampu menyala terang";
  } else if (simulatedData.lux > 50) {
    simulatedData.log = "Cahaya cukup – Lampu dimatikan";
  } else {
    simulatedData.log = "Tidak ada gerakan – Lampu meredup";
  }

  updateDashboard(simulatedData);
}, 5000);

let lastLogText = "";
