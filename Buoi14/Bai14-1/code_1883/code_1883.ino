#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // Cần cài thư viện ArduinoJson

/* ====== CẤU HÌNH ====== */
const char* ssid     = "MayUEH";
const char* password = "khongyeu";

// Thông tin MQTT Broker (Home Assistant / Mosquitto)
const char* mqtt_server = "192.168.172.94"; // <--- Sửa IP Broker
const int mqtt_port = 1883;                // Cổng không mã hóa
const char* mqtt_user = "mqtt-user";       // User thật của bạn
const char* mqtt_pass = "123456";          // Pass thật của bạn
const char* mqtt_topic = "iot/lab14/sensor";

WiFiClient espClient;
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
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
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

  // Gửi dữ liệu giả lập mỗi 5 giây
  StaticJsonDocument<200> doc;
  doc["temp"] = random(250, 350) / 10.0;
  doc["hum"] = random(400, 800) / 10.0;
  char payload[128];
  serializeJson(doc, payload);

  Serial.print("Publishing message: ");
  Serial.println(payload);
  client.publish(mqtt_topic, payload);
  
  delay(5000);
}