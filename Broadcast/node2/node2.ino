#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

typedef struct struct_message {
  uint8_t nodeID;
  float tilt;
  uint32_t timestamp;
} struct_message;

struct_message myData;

uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);  // SDA, SCL
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

  Serial.print("Channel: ");
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

  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float accelX = ax / 16384.0;
  float accelZ = az / 16384.0;
  float tiltAngle = atan2(accelX, accelZ) * 180 / PI;

  myData.nodeID = 2;  // 🔥 CHANGE THIS to 1 or 2
  myData.tilt = tiltAngle;
  myData.timestamp = millis();

  esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  Serial.print("Tilt: ");
  Serial.println(tiltAngle);

  delay(3000);
}