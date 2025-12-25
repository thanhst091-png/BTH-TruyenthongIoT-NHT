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

// Hàm này sẽ tự động chạy khi Server báo tin về
void callback_notify(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  
  Serial.print("Server tu bao ve (Notify): ");
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

  // Đăng ký hàm nhận thông báo
  coap.response(callback_notify);
  
coap.start();

  Serial.println("Dang gui lenh dang ky Observe...");

  sendObserve();
}

void loop() {
  // Trong vòng lặp KHÔNG LÀM GÌ CẢ
  // Chỉ lắng nghe
  coap.loop();
}
void sendObserve() {
  Serial.println("Dang gui goi tin Observe thu cong...");
  
  udp.beginPacket(coap_server_ip, 5683);
  
  // 1. Header: Ver=1, Type=CON(0), TKL=0 (Token Length=0) -> 0x40
  udp.write(0x40); 
  
  // 2. Code: GET (1) -> 0x01
  udp.write(0x01);
  
  // 3. Message ID (2 bytes ngẫu nhiên)
  uint8_t mid1 = random(0, 255);
  uint8_t mid2 = random(0, 255);
  udp.write(mid1);
  udp.write(mid2);

  // 4. Option: OBSERVE (Số 6) - Value 0 (Đăng ký)
  // Delta = 6, Length = 0 -> 0x60
  udp.write(0x60); 

  // 5. Option: URI-PATH (Số 11) - "sensor"
  // Delta = 11 - 6 = 5. Length = 6 -> 0x56
  udp.write(0x56); 
  udp.write("sensor", 6);

  // 6. Option: URI-PATH (Số 11 tiếp) - "temp"
  // Delta = 11 - 11 = 0. Length = 4 -> 0x04
  udp.write(0x04);
  udp.write("temp", 4);

  udp.endPacket();
}
