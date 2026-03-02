#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <MPU6050.h>

const char* ssid = "ESP32_SERVER";
const char* password = "12345678";

const char* receiverIP = "192.168.4.1";  // Default AP IP

MPU6050 mpu(0x68);

#define LED 2

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  Wire.begin(21, 22);
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Receiver AP");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Receiver!");
}

void loop() {

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    String url = String("http://") + receiverIP + "/data";
    http.begin(url);
    http.addHeader("Content-Type", "text/plain");

    String payload = String(ax)+","+String(ay)+","+String(az)+","+
                     String(gx)+","+String(gy)+","+String(gz);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.println("Sent: " + payload);

      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
    }

    http.end();
  }

  delay(1000);
}