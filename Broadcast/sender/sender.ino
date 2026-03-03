#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <MPU6050.h>
#include <esp_wifi.h>

MPU6050 mpu;

// -------- DATA STRUCT --------
typedef struct struct_message {
  uint8_t nodeID;
  float tilt;
  int soilAnalog;     // 🌱 Added
  int soilDigital;    // 🌱 Added
  uint32_t timestamp;
} struct_message;

struct_message myData;

uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// ESP32 callback
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  // 🌱 Soil Pins
  pinMode(32, INPUT);  // AO
  pinMode(4, INPUT);   // DO

  // I2C Start
  Wire.begin(21, 22);
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  Serial.println("MPU6050 Connected");

  WiFi.mode(WIFI_STA);
  WiFi.begin("Dialog 4G 424", "A1b978e1");

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  Serial.print("Sender Channel: ");
  Serial.println(WiFi.channel());

  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);

  Serial.println("Sender Ready");
}

void loop() {

  // -------- MPU --------
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float accelX = ax / 16384.0;
  float accelZ = az / 16384.0;
  float tiltAngle = atan2(accelX, accelZ) * 180 / PI;

  // -------- Soil --------
  int soilAnalog = analogRead(32);   // 0-4095
  int soilDigital = digitalRead(4);  // 0 or 1

  // -------- Pack Data --------
  myData.nodeID = 1;
  myData.tilt = tiltAngle;
  myData.soilAnalog = soilAnalog;
  myData.soilDigital = soilDigital;
  myData.timestamp = millis();

  esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  Serial.println("------ Sending ------");
  Serial.print("Tilt: "); Serial.println(tiltAngle);
  Serial.print("Soil Analog: "); Serial.println(soilAnalog);
  Serial.print("Soil Digital: "); Serial.println(soilDigital);

  delay(3000);
}