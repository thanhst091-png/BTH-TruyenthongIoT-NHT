#include <DHT11.h>

// Kết nối DHT11 Data Pin với Digital Pin 2
DHT11 dht11(2);

void setup() {
    // Khởi tạo Serial ở tốc độ 9600 bps
    Serial.begin(9600);
}

void loop() {
    // Biến lưu trữ kết quả đọc
    int temperature = dht11.readTemperature();
    int humidity = dht11.readHumidity();

    if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT) {
        // Gửi dữ liệu dưới dạng chuỗi JSON qua Serial
        // Dạng: {"temp":25,"hum":60}
        Serial.print("{\"temp\":");
        Serial.print(temperature);
        Serial.print(",\"hum\":");
        Serial.print(humidity);
        Serial.println("}"); // Sử dụng Serial.println để thêm ký tự xuống dòng (\n) làm dấu kết thúc gói tin

    } else {
        // Nếu có lỗi, bạn có thể in ra lỗi để debug trong Proteus (tùy chọn)
        Serial.println("{\"error\":\"read_fail\"}");
    }
    
    delay(2000); // Đọc dữ liệu sau mỗi 2 giây
}