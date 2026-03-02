#include <WiFi.h>
#include <WebServer.h>



const char* ssid = "Dialog 4G 424";
const char* password = "A1b978e1";

WebServer server(80);

void handleData() {
  String data = server.arg("plain");
  Serial.println("Received Data:");
  Serial.println(data);
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("Receiver IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/data", HTTP_POST, handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}


