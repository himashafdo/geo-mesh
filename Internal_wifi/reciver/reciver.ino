#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32_SERVER";
const char* password = "12345678";   // minimum 8 chars

WebServer server(80);

void handleData() {
  String data = server.arg("plain");
  Serial.println("Received:");
  Serial.println(data);
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  // Start ESP32 as Access Point
  WiFi.softAP(ssid, password);

  Serial.println("Access Point Started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());   // Usually 192.168.4.1

  server.on("/data", HTTP_POST, handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}