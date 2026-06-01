# DigitalTwin-Experiment-Basic

## English
This is an experiment by a 2nd-year, 4th-semester student. This experiment resulted from an AI whip until my tokens were limited for a month. Use it sparingly as a reference, as this AI code is messy in many ways. Enjoy using it to your heart's content.

## Indonesia
ini adalah eksperimen mahasiswa tingkat 2 semester 4. eksperimen ini hasil dari pecut AI sampai token ku kena limit sebulan. gunakan seperlunya sebagai referensi, karena code ini hasil ai banyak hal yang berantakan terkait isinya. selamat menggunakan sepuasnya

- Frontend utama: `smarthome.html` (model GLB: `smarthouse.glb`).

Isi repositori (top-level)
- `smarthome.html` — aplikasi frontend (Three.js + WebSocket hooks)
- `smarthouse.glb` — 3D model yang dipakai oleh halaman
- `js/` — library dan helper Three.js (lokal)
- `assets/` — fonts, LUTs, textures, model support
- `ESP32-Camera-Stream/` — sketch dan server code untuk ESP32-CAM (AI-Thinker)
	- `ESP32-Camera-Stream.ino`, `app_httpd.cpp`, `camera_pins.h`, `board_config.h`
- `ESP32S3-Sensor/` — contoh sketch sensor untuk ESP32-S3 (`ESP32S3-Sensor.ino`)
- `node-red.json` — Node-RED export (flows) used by the project
- `README.md` — (this file)

Quick start — jalankan frontend lokal
1. Dari folder proyek, jalankan static server (port 8000):

```bash
cd C:\laragon\www\digital-twin-project
# Python 3
python -m http.server 8000
```

2. Buka browser: `http://localhost:8000/smarthome.html`

Catatan: membuka `smarthome.html` langsung dari file:// terkadang dibatasi oleh browser (fetch/CORS); gunakan web server lokal.

Flash & jalankan ESP32-CAM (stream)
1. Buka `ESP32-Camera-Stream/ESP32-Camera-Stream.ino` di Arduino IDE atau `arduino-cli`.
2. Pilih board: **AI Thinker ESP32-CAM** (atau board yang cocok untuk modul Anda).
3. Pastikan `board_config.h` mendefinisikan `CAMERA_MODEL_AI_THINKER` (default repository sudah disederhanakan).
4. Atur SSID dan password di sketch, lalu upload.
5. Buka Serial Monitor pada baud `115200` untuk melihat log dan IP address.
6. Buka `http://<ESP32-IP>/stream` di browser atau gunakan tombol Open Camera di dashboard.

Catatan: sketch menyajikan MJPEG stream pada `/stream` (server default port 80). Beberapa build/variants juga membuka stream pada port 81 — periksa IP/port pada Serial Monitor.

ESP32 Sensor (ESP32-S3)
- contoh sketch: `ESP32S3-Sensor/ESP32S3-Sensor.ino`
- ubah SSID/MQTT broker sesuai jaringan Anda, lalu upload ke board ESP32-S3.

Node-RED
- `node-red.json` berisi export flows. Import file ini dari antarmuka Node-RED (Menu → Import) lalu Deploy.

Debug & troubleshooting singkat
- Jika frontend tidak terhubung ke WebSocket, pastikan Node-RED berjalan dan WebSocket endpoint tersedia.
- Jika kamera tidak streaming: periksa Serial Monitor, pastikan WiFi terhubung dan IP tercetak.
- Jika `Camera capture failed` muncul, coba turunkan resolusi pada sketch atau pastikan PSRAM tersedia.
