#include <WiFi.h>
#include <SimpleDHT.h>

// WiFi credentials
const char* ssid = "POCOX6G";
const char* password = "Anan3";

// DHT11 sensor on GPIO4
#define DHTPIN 4
SimpleDHT11 dht11(DHTPIN);

// Web server
WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client Connected");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLine.length() == 0) {
          byte temperature = 0;
          byte humidity = 0;
          int err = dht11.read(&temperature, &humidity, NULL);

          String html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
          html += "<!DOCTYPE html><html><head>";
          html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
          html += "<title>ESP32 Weather Station</title>";
          html += "<link href='https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&display=swap' rel='stylesheet'>";
          html += "<style>";
          html += "body { margin:0; font-family:'Roboto', sans-serif; background:#e0f7fa; color:#333; }";
          html += ".container { max-width:400px; margin:50px auto; padding:20px; background:white; border-radius:15px; box-shadow:0 4px 20px rgba(0,0,0,0.1); text-align:center; }";
          html += "h1 { color:#00796b; }";
          html += ".reading { font-size:2.5rem; margin:10px 0; }";
          html += ".label { font-size:1rem; color:#666; }";
          html += ".footer { margin-top:20px; font-size:0.9rem; color:#aaa; }";
          html += ".icon { font-size:2rem; margin-bottom:10px; color:#00796b; }";
          html += "</style></head><body><div class='container'>";
          html += "<h1>🌤️ Weather Station</h1>";

          if (err != SimpleDHTErrSuccess) {
            html += "<p>Error reading sensor.</p>";
          } else {
            html += "<div class='icon'>🌡️</div><div class='reading'>" + String(temperature) + "°C</div>";
            html += "<div class='label'>Temperature</div>";
            html += "<div class='icon'>💧</div><div class='reading'>" + String(humidity) + " %</div>";
            html += "<div class='label'>Humidity</div>";
          }

          html += "<div class='footer'>ROBOARMY✨</div></div></body></html>";

          client.print(html);
          break;
        } else if (c != '\r') {
          currentLine += c;
        } else {
          currentLine = "";
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Client Disconnected");
  }
}
