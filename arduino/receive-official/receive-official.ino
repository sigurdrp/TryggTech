#include <WiFi.h>
#include <esp_now.h>
#include "esp_wifi.h"
#include "esp_err.h"

const int ESPNOW_CH = 6;

struct Payload {
  int  id;
  char message[24];
};

static void printMac(const uint8_t* mac, char out[18]) {
  snprintf(out, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void OnDataRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  char fromMac[18] = "00:00:00:00:00:00";
  int rssi = 0;

  if (info) {
    if (info->src_addr) {
      printMac(info->src_addr, fromMac);
    }
    if (info->rx_ctrl) {
      rssi = info->rx_ctrl->rssi;
    }
  }

  if (len == (int)sizeof(Payload)) {
    Payload p{};
    memcpy(&p, data, sizeof(Payload));
    p.message[sizeof(p.message) - 1] = '\0';  // safety null-termination

    Serial.printf("{\"id\":%d,\"message\":\"%s\",\"rssi\":%d,\"from\":\"%s\"}\n",
                  p.id, p.message, rssi, fromMac);
  } else {
    Serial.printf("{\"warn\":\"size_mismatch\",\"len\":%d,\"from\":\"%s\",\"rssi\":%d,\"raw\":\"",
                  len, fromMac, rssi);
    for (int i = 0; i < len; ++i) {
      Serial.printf("%02X", data[i]);
    }
    Serial.println("\"}");
  }
}

void setup() {
  Serial.begin(115200);
  delay(50);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);

  // lock channel for stable ESP-NOW
  esp_wifi_set_channel(ESPNOW_CH, WIFI_SECOND_CHAN_NONE);

  esp_err_t e = esp_now_init();
  Serial.printf("esp_now_init: %d (%s)\n", e, esp_err_to_name(e));
  if (e != ESP_OK) return;

  e = esp_now_register_recv_cb(OnDataRecv);
  Serial.printf("register_recv_cb: %d (%s)\n", e, esp_err_to_name(e));

  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.printf("Locked channel: %d\n", ESPNOW_CH);
}

void loop() {
  delay(100);
}
