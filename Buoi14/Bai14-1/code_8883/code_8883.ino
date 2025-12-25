#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/* ====== CẤU HÌNH ====== */
const char* ssid = "liunew";           // <--- Sửa Wifi
const char* password = "liuliuliuliu";     // <--- Sửa Pass

// IP Home Assistant
const char* mqtt_server = "test.mosquitto.org"; // <--- sv ở nước ngoài
const int mqtt_port = 8883;                 // <--- ĐỔI SANG CỔNG BẢO MẬT
const char* mqtt_user = "mqtt-user";
const char* mqtt_pass = "123456";
const char* mqtt_topic = "iot/lab14/sensor";

// Dùng WiFiClientSecure thay cho WiFiClient thường
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // QUAN TRỌNG: Bỏ qua kiểm tra chứng chỉ (cho phép tự ký)
  // Lệnh này giúp kết nối được với Mosquitto mặc định của Home Assistant
  espClient.setInsecure(); 
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTTS connection...");
    String clientId = "ESP8266Client-Secure-";
    clientId += String(random(0xffff), HEX);
    
    // --- SỬA 
    if (client.connect(clientId.c_str())) { 
      Serial.println("connected (Encrypted)");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Gửi dữ liệu
  StaticJsonDocument<200> doc;
  doc["temp"] = random(250, 350) / 10.0;
  doc["hum"] = random(400, 800) / 10.0;
  char payload[128];
  serializeJson(doc, payload);

  Serial.print("Publishing Secure message: ");
  Serial.println(payload);
  client.publish(mqtt_topic, payload);
  
  delay(5000);
}