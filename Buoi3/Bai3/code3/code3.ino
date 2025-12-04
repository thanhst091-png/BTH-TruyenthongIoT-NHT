#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 10

MD_Parola parola = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

String inputText = "Lonely";  
bool newMessage = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Virtual Terminal Ready!");
  Serial.println("Nhap van ban:");
  
  parola.begin();
  parola.displayClear();
  parola.displayText(inputText.c_str(), PA_CENTER, 80, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
}

void loop() {
  // Đọc dữ liệu từ Virtual Terminal
  if (Serial.available() > 0) {
    inputText = Serial.readStringUntil('\n');  
    inputText.trim();  

    if (inputText.length() > 0) {
      newMessage = true;
      Serial.print("Da nhan: ");
      Serial.println(inputText);
    }
  }

  if (newMessage) {
    parola.displayClear();
    parola.displayReset();
    parola.displayText(inputText.c_str(), PA_CENTER, 50, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    newMessage = false;
  }

  if (parola.displayAnimate()) {
    parola.displayReset();
  }
}