#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Firebase_ESP_Client.h>

// ---------------- WIFI ----------------
#define WIFI_SSID "Dialog 4G 424"
#define WIFI_PASSWORD "A1b978e1"

// ---------------- FIREBASE ----------------
#define API_KEY "AIzaSyCV6tUSnuELlBUPvDU2lcS9kk01hLprSN0"
#define DATABASE_URL "https://geo-mesh-9abb8-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ---------------- STRUCT (MUST MATCH SENDER) ----------------
typedef struct struct_message {
  uint8_t nodeID;
  float tilt;
  int soilAnalog;
  int soilDigital;
  uint32_t timestamp;
} struct_message;

struct_message incomingData;
volatile bool newData = false;

// -------- ESP-NOW RECEIVE CALLBACK --------
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {

  memcpy(&incomingData, data, sizeof(incomingData));
  newData = true;

  Serial.println("Packet Received");
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  // -------- CONNECT WIFI --------
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("Gateway Channel: ");
  Serial.println(WiFi.channel());

  // -------- FIREBASE CONFIG --------
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase SignUp OK");
  } else {
    Serial.printf("SignUp Error: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase Ready");

  // -------- ESP-NOW INIT --------
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Gateway Ready - Listening...");
}

// ---------------- LOOP ----------------
void loop() {

  if (newData) {
    newData = false;

    Serial.println("Uploading to Firebase...");

    String basePath = "/nodes/node" + String(incomingData.nodeID);

    bool success =
      Firebase.RTDB.setFloat(&fbdo, basePath + "/tilt", incomingData.tilt) &&
      Firebase.RTDB.setInt(&fbdo, basePath + "/soilAnalog", incomingData.soilAnalog) &&
      Firebase.RTDB.setInt(&fbdo, basePath + "/soilDigital", incomingData.soilDigital) &&
      Firebase.RTDB.setInt(&fbdo, basePath + "/timestamp", incomingData.timestamp);

    if (success) {
      Serial.println("Upload Success");
      Serial.print("Tilt: "); Serial.println(incomingData.tilt);
      Serial.print("Soil Analog: "); Serial.println(incomingData.soilAnalog);
      Serial.print("Soil Digital: "); Serial.println(incomingData.soilDigital);
    } else {
      Serial.println("Upload Failed");
      Serial.println(fbdo.errorReason());
    }
  }

  // Auto reconnect WiFi if dropped
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
  }
}