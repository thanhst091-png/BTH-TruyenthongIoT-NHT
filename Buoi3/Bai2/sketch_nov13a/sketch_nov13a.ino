#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <EEPROM.h>

// Chan ket noi - NUT DOWN DOI SANG CHAN 4
#define BUZZER_PIN 6    // PD6 - Buzzer
#define LED_PIN 7       // PD7 - LED
#define BTN_MODE 3      // PD3 - Nut MODE
#define BTN_UP 2        // PD2 - Nut UP
#define BTN_DOWN 4      // PD4 - Nut DOWN (doi sang chan 4)
#define BTN_SET 0       // PD0 - Nut SET

// Dia chi EEPROM
#define ALARM_HOUR_ADDR 0
#define ALARM_MINUTE_ADDR 1
#define ALARM_STATE_ADDR 2

// Khai bao doi tuong
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Bien toan cuc
int currentHour, currentMinute, currentSecond;
int alarmHour = 7, alarmMinute = 0;
bool alarmEnabled = false;
bool alarmActive = false;

unsigned long lastDisplayUpdate = 0;
unsigned long displayUpdateInterval = 500;
unsigned long alarmStartTime = 0;
unsigned long alarmDuration = 30000;

enum Mode {CLOCK, SET_ALARM_HOUR, SET_ALARM_MINUTE};
Mode currentMode = CLOCK;

// Bien cho xu ly nut nhan
unsigned long lastButtonCheck = 0;
unsigned long buttonCheckInterval = 100;
bool btnModePressed = false;
bool btnUpPressed = false;
bool btnDownPressed = false;
bool btnSetPressed = false;

void setup() {
  Serial.begin(9600);
  
  // Khoi tao chan I/O
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP); 
  pinMode(BTN_SET, INPUT_PULLUP);
  
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  Wire.begin();
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Khoi dong...");
  
  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("Loi RTC!");
    while (1);
  }
  
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  alarmHour = EEPROM.read(ALARM_HOUR_ADDR);
  alarmMinute = EEPROM.read(ALARM_MINUTE_ADDR);
  alarmEnabled = EEPROM.read(ALARM_STATE_ADDR);
  
  if (alarmHour > 23) alarmHour = 7;
  if (alarmMinute > 59) alarmMinute = 0;
  
  delay(1000);
  lcd.clear();
  
  Serial.println("San sang! Nut DOWN o chan 4");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
    lastDisplayUpdate = currentMillis;
    readRTCTime();
    checkAlarm();
    updateDisplay();
  }
  
  if (currentMillis - lastButtonCheck >= buttonCheckInterval) {
    lastButtonCheck = currentMillis;
    handleButtons();
  }
  
  if (alarmActive) {
    handleActiveAlarm();
  }
}

void readRTCTime() {
  DateTime now = rtc.now();
  currentHour = now.hour();
  currentMinute = now.minute();
  currentSecond = now.second();
}

void checkAlarm() {
  if (alarmEnabled && !alarmActive) {
    if (currentHour == alarmHour && currentMinute == alarmMinute && currentSecond == 0) {
      alarmActive = true;
      alarmStartTime = millis();
    }
  }
}

void handleActiveAlarm() {
  unsigned long currentMillis = millis();
  
  if (currentMillis % 1000 < 500) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
  
  if (currentMillis - alarmStartTime >= alarmDuration) {
    stopAlarm();
  }
}

void stopAlarm() {
  alarmActive = false;
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}

void handleButtons() {
  int modeState = digitalRead(BTN_MODE);
  int upState = digitalRead(BTN_UP);
  int downState = digitalRead(BTN_DOWN); 
  int setState = digitalRead(BTN_SET);
  
  // Nut MODE
  if (modeState == LOW && !btnModePressed) {
    btnModePressed = true;
    if (alarmActive) {
      stopAlarm();
    } else {
      switchMode();
    }
    delay(200);
  } else if (modeState == HIGH) {
    btnModePressed = false;
  }
  
  // Nut UP
  if (upState == LOW && !btnUpPressed) {
    btnUpPressed = true;
    handleUpButton();
    delay(150);
  } else if (upState == HIGH) {
    btnUpPressed = false;
  }
  
  // Nut DOWN
  if (downState == LOW && !btnDownPressed) {
    btnDownPressed = true;
    handleDownButton();
    Serial.println("Nut DOWN duoc nhan!");
    delay(150);
  } else if (downState == HIGH) {
    btnDownPressed = false;
  }
  
  // Nut SET
  if (setState == LOW && !btnSetPressed) {
    btnSetPressed = true;
    handleSetButton();
    delay(200);
  } else if (setState == HIGH) {
    btnSetPressed = false;
  }
}

void switchMode() {
  switch (currentMode) {
    case CLOCK:
      currentMode = SET_ALARM_HOUR;
      break;
    case SET_ALARM_HOUR:
      currentMode = SET_ALARM_MINUTE;
      break;
    case SET_ALARM_MINUTE:
      currentMode = CLOCK;
      EEPROM.write(ALARM_HOUR_ADDR, alarmHour);
      EEPROM.write(ALARM_MINUTE_ADDR, alarmMinute);
      break;
  }
  lcd.clear();
}

void handleUpButton() {
  switch (currentMode) {
    case SET_ALARM_HOUR:
      alarmHour = (alarmHour + 1) % 24;
      break;
    case SET_ALARM_MINUTE:
      alarmMinute = (alarmMinute + 1) % 60;
      break;
  }
  lcd.clear();
}

void handleDownButton() {
  switch (currentMode) {
    case SET_ALARM_HOUR:
      alarmHour = (alarmHour - 1 + 24) % 24;
      break;
    case SET_ALARM_MINUTE:
      alarmMinute = (alarmMinute - 1 + 60) % 60;
      break;
  }
  lcd.clear();
}

void handleSetButton() {
  alarmEnabled = !alarmEnabled;
  EEPROM.write(ALARM_STATE_ADDR, alarmEnabled);
  lcd.clear();
}

void updateDisplay() {
  lcd.setCursor(0, 0);
  
  switch (currentMode) {
    case CLOCK:
      lcd.print("Gio: ");
      printTwoDigits(currentHour);
      lcd.print(":");
      printTwoDigits(currentMinute);
      lcd.print(":");
      printTwoDigits(currentSecond);
      
      lcd.setCursor(0, 1);
      lcd.print("BT: ");
      printTwoDigits(alarmHour);
      lcd.print(":");
      printTwoDigits(alarmMinute);
      lcd.print(alarmEnabled ? " BAT" : " TAT");
      break;
      
    case SET_ALARM_HOUR:
      lcd.print("Cai gio bao thuc");
      lcd.setCursor(0, 1);
      lcd.print("Gio: ");
      printTwoDigits(alarmHour);
      lcd.print("    ");
      break;
      
    case SET_ALARM_MINUTE:
      lcd.print("Cai phut bao thuc");
      lcd.setCursor(0, 1);
      lcd.print("Phut: ");
      printTwoDigits(alarmMinute);
      lcd.print("    ");
      break;
  }
}

void printTwoDigits(int number) {
  if (number < 10) {
    lcd.print("0");
  }
  lcd.print(number);
}