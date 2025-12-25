#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

/* ====== CẤU HÌNH ====== */
const char* ssid     = "MayUEH";
const char* password = "khongyeu";

// IP máy tính của bạn (Hãy kiểm tra lại bằng ipconfig nếu cần)
IPAddress coap_server_ip(192, 168, 172, 158); 

WiFiUDP udp;
Coap coap(udp);

// Hàm này sẽ tự động chạy khi ESP nhận được câu trả lời từ Server
void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  
  Serial.print("Nhan duoc phan hoi: ");
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

  // Đăng ký hàm callback để xử lý kết quả trả về
  coap.response(callback_response);
  
  coap.start();
}

void loop() {
  coap.loop();
  
  // Cứ 2 giây lại hỏi 1 lần (POLLING)
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 2000) {
    lastTime = millis();
    
    Serial.println("Dang gui lenh GET (Hoi nhiet do)...");
    
    // Gửi lệnh GET đến server
    coap.get(coap_server_ip, 5683, "sensor/temp");
  }
}