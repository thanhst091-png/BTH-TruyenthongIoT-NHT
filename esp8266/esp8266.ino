#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/* ====== CONFIG ====== */
const char* ssid        = "Mang_lagnhucucai";
const char* password    = "matkhausai";
const char* mqtt_server = "192.168.1.111";
const int   mqtt_port   = 1883;
const char* mqtt_user   = "mqtt-user";
const char* mqtt_pass   = "123456";
const char* student_id  = "10123297";
const char* mqtt_topic  = "iot/lab1/10123297/sensor"; 

/* ==================== */

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-" + String(student_id);
    
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

  // Tạo dữ liệu giả lập
  JsonDocument doc; // Cú pháp ArduinoJson V7 (V6 dùng StaticJsonDocument<200> doc)
  doc["temp"] = random(250, 350) / 10.0;
  doc["hum"] = random(400, 800) / 10.0;

  char payload[128];
  serializeJson(doc, payload);

  Serial.print("Publishing message: ");
  Serial.println(payload);
  
  if (client.publish(mqtt_topic, payload)) {
    Serial.println("Publish ok");
  } else {
    Serial.println("Publish failed");
  }

  delay(5000); // Gửi dữ liệu mỗi 5 giây
}