#include <EEPROM.h>

const int nut = 2;
const int led = 13;

bool cauhinh = false;
unsigned long lucNhan = 0;
bool dangNhan = false;
int demNhan = 0;
long baud = 9600;
unsigned long lanGuiCuoi = 0; 

void setup() {
  pinMode(nut, INPUT);
  pinMode(led, OUTPUT);
  
  EEPROM.get(0, baud);
  if (baud <= 0) baud = 9600;
  
  Serial.begin(baud);
  Serial.println("Bat dau, Baud: " + String(baud));
}

void loop() {
  kiemTraNut();
  dieuKhienLED();
  
  if (cauhinh) {
    xuLyCauHinh();
  } else {
    if (millis() - lanGuiCuoi > 2000) {
      Serial.println("Dang hoat dong, baud: " + String(baud));
      lanGuiCuoi = millis(); 
    }
  }
  delay(10);
}

void kiemTraNut() {
  if (digitalRead(nut) == HIGH) {
    if (!dangNhan) {
      dangNhan = true;
      lucNhan = millis();
    }  
    if (millis() - lucNhan > 3000) {
      if (!cauhinh) vaoCauHinh();
      else thoatCauHinh();
      lucNhan = millis();
    }
  } else {
    if (dangNhan) {
      dangNhan = false;
      if (cauhinh && (millis() - lucNhan < 1000)) {
        demNhan++;
        Serial.println("Nhan: " + String(demNhan) + " lan");
      }
    }
  }
}
void vaoCauHinh() {
  cauhinh = true;
  demNhan = 0;
  Serial.end();
  Serial.begin(9600);
  Serial.println("Thong Bao");
  Serial.println("1 lan: 9600 bps");
  Serial.println("2 lan: 115200 bps");
}
void thoatCauHinh() {
  cauhinh = false;
  Serial.println("Thoat cau hinh...");
  delay(500);
  Serial.end();
  Serial.begin(baud);
  Serial.println("Baud hien tai la: " + String(baud));
  lanGuiCuoi = millis(); 
}
void xuLyCauHinh() {
  if (demNhan > 0 && (millis() - lucNhan > 1000)) {
    if (demNhan == 1) datBaud(9600);
    else if (demNhan == 2) datBaud(115200);
    demNhan = 0;
  }
}
void datBaud(long newBaud) {
  baud = newBaud;
  EEPROM.put(0, baud); 
  for(int i = 0; i < demNhan; i++) {
    digitalWrite(led, HIGH);
    delay(400);
    digitalWrite(led, LOW);
    delay(300);
  } 
  Serial.println("Dat Baud: " + String(baud));
}
void dieuKhienLED() {
  if (cauhinh) {
    digitalWrite(led, (millis() % 400) < 200);
  } else {
    digitalWrite(led, LOW);
  }
}