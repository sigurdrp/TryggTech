#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <WiFi.h>
#include <esp_now.h>
#include "esp_wifi.h"
#include "esp_err.h"
 
MPU9250_asukiaaa mySensor;
 
uint8_t receiverAddress[] = {0xEC, 0xE3, 0x34, 0x9A, 0xCD, 0xE0};
const int ESPNOW_CH = 6;
 
struct Payload {
  int id;
  char message[24];
} myData{1, ""};
 
void OnDataSent(const wifi_tx_info_t* info, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}
 
void sendMessage(const char* msg) {
  strncpy(myData.message, msg, sizeof(myData.message) - 1);
  myData.message[sizeof(myData.message) - 1] = '\0';
 
  esp_err_t result = esp_now_send(receiverAddress, reinterpret_cast<uint8_t*>(&myData), sizeof(myData));
  Serial.printf("Send result: %d (%s)\n", result, esp_err_to_name(result));
}
 
void setup() {
  Serial.begin(115200);
  Wire.begin();
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
 
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  esp_wifi_set_channel(ESPNOW_CH, WIFI_SECOND_CHAN_NONE);
 
  esp_err_t e = esp_now_init();
  Serial.printf("esp_now_init: %d (%s)\n", e, esp_err_to_name(e));
  if (e != ESP_OK) return;
 
  esp_now_register_send_cb(OnDataSent);
 
  esp_now_peer_info_t peerInfo{};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = ESPNOW_CH;
  peerInfo.ifidx = WIFI_IF_STA;
  peerInfo.encrypt = false;
 
  e = esp_now_add_peer(&peerInfo);
  Serial.printf("add_peer: %d (%s)\n", e, esp_err_to_name(e));
  if (e != ESP_OK) return;
 
  Serial.print("Sender MAC: ");
  Serial.println(WiFi.macAddress());
}
 
void loop() {
  mySensor.accelUpdate();
  float ax = mySensor.accelX();
  float ay = mySensor.accelY();
  float az = mySensor.accelZ();
 
  float abs_a = sqrt(ax * ax + ay * ay + az * az);
  if (abs_a > 3) {
    sendMessage("Event triggered");
  }
 
  delay(100);
}
 