#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

/* ====== CẤU HÌNH ====== */
const char* ssid     = "MayUEH";
const char* password = "khongyeu";

// IP máy tính 
IPAddress coap_server_ip(192, 168, 172, 158); 
WiFiUDP udp;
Coap coap(udp);

// Hàm này chạy khi nhận được các gói tin từ Server
void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  Serial.print("Nhan duoc Block, kich thuoc: ");
  Serial.println(packet.payloadlen);
  
  // In thử vài ký tự đầu xem có phải chữ A không
  char p[10];
  memcpy(p, packet.payload, 5);
  p[5] = 0;
  Serial.print("Noi dung (trich doan): ");
  Serial.println(p);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  coap.response(callback_response);
  coap.start();

  // Gửi yêu cầu tải file firmware
  Serial.println("Dang yeu cau tai Firmware...");
  coap.get(coap_server_ip, 5683, "firmware");
}

void loop() {
  coap.loop();
}