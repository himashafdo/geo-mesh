#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <MPU6050.h>

const char* ssid = "Dialog 4G 424";
const char* password = "A1b978e1";

const char* receiverIP = "192.168.8.140";  // <-- PUT RECEIVER IP HERE

MPU6050 mpu;

#define LED 2   // Built-in LED (usually GPIO2)

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
}

void loop() {

  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    String url = String("http://") + receiverIP + "/data";
    http.begin(url);
    http.addHeader("Content-Type", "text/plain");

    String payload = String(ax) + "," + String(ay) + "," + String(az) + "," +
                     String(gx) + "," + String(gy) + "," + String(gz);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.println("Data Sent: " + payload);

      // 🔵 BLINK LED WHEN PACKET SENT
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
    }

    http.end();
  }

  delay(1500);  // Send every 1 second
}


