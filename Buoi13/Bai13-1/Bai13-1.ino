#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

/* ====== CONFIG (Theo trang 5 PDF) ====== */
const char* WIFI_SSID = "Mang_lagnhucucai";
const char* WIFI_PASS = "matkhausai";

/* COAP Server (Receiver) */
// Thay IP này bằng IP máy tính của bạn
IPAddress coap_server_ip(192, 168, 1, 102); 
const int coap_port = 5683;

/* COAP Resource */
const char* coap_resource = "sensor/temp";

/* Send interval (ms) */
const unsigned long SEND_INTERVAL = 5000;

/* ======================================= */

WiFiUDP udp;
Coap coap(udp);

unsigned long lastSend = 0;

/* Hàm kết nối Wifi (Theo trang 6 PDF) */
void setup_wifi() {
  Serial.print("Connecting WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setup_wifi();

  coap.start();
  Serial.println("CoAP client started");
}

void loop() {
  coap.loop();

  // Gửi tin mỗi 5 giây (Theo trang 7 PDF)
  if (millis() - lastSend >= SEND_INTERVAL) {
    lastSend = millis();

    /* Payload mẫu */
    String payload = "temp=30.5";
    
    Serial.print("Sending CoAP NON POST to ");
    Serial.print(coap_resource);
    Serial.print(" payload: ");
    Serial.println(payload);

    /* Gửi tin nhắn (Đã sửa để chạy được với thư viện mới)
       Code gốc trong PDF chỉ có 7 tham số -> Sẽ lỗi
       Code này dùng 9 tham số -> Sẽ chạy OK
    */
    coap.send(
      coap_server_ip,
      coap_port,
      coap_resource,
      (COAP_TYPE)1,   // 1 là NON (Non-confirmable) 0 là CON
      (COAP_METHOD)2, // 2 là POST
      NULL, 0,        // Token (Bắt buộc phải có ở bản mới)
      (uint8_t*)payload.c_str(),
      payload.length()
    );
  }
}