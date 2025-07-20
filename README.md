IOT-WebApi‐Blazor‐FreeRTOS
A full-stack IoT solution built with:

🧠 ESP32 running FreeRTOS – collects sensor data and sends it to the backend

🌐 ASP.NET Core Web API – receives telemetry and handles control logic

💻 Blazor WebAssembly Dashboard – displays real-time data and sends commands

This project showcases a scalable architecture for smart agriculture, home automation, or industrial IoT.

🗂️ Project Structure

/
├── ESP32-FreeRtos/         ← ESP32 firmware with FreeRTOS + WiFi + HTTP client
├── Airponic/                 ← ASP.NET Core Web API for data handling
├── Airponic.Client/           ← Blazor WebAssembly UI for real-time monitoring
└── README.md

🚀 Getting Started
✅ Prerequisites
Hardware:

  ESP32 development board (e.g., ESP32 DevKit V1, ESP32-WROOM)

  Sensors (e.g., DHT22, water pump relay, etc.)

Firmware Tools:

  VS Code with PlatformIO or Arduino IDE with ESP32 board support

Backend:

  .NET 8 SDK

  SQLite / PostgreSQL / SQL Server

Frontend:

  .NET SDK

  Web browser (Edge, Chrome, Firefox)
  
🔧 ESP32 
Open in PlatformIO or Arduino IDE.

Configure:

Wi-Fi SSID/password

API base URL (http://<yourserver>/api/telemetry)

Flash to ESP32.

ESP32 sends telemetry (e.g., temperature, humidity) via HTTP POST and optionally receives control commands via polling or push.

You can use FreeRTOS tasks and timers for scheduling sensor reads, pump control, and communication

🔒 Security Notes
For production, use HTTPS and API key/token validation for ESP32 requests.

Consider JWT authentication for user access to the dashboard.

🧪 Testing
ESP32: Use serial monitor to debug sensor readings and API communication

API: Use Swagger, Postman, or curl to verify endpoints

Blazor: View logs and check developer console

🔮 Possible Enhancements
Add JWT-based authentication

Real-time data updates via SignalR

OTA firmware updates to ESP32

📬 Contact
Made by Nima Azhdari
For questions or feedback, open an issue.



