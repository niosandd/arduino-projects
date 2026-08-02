#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// RTC pins
#define RTC_DAT 3
#define RTC_CLK 13
#define RTC_RST 4

ThreeWire myWire(RTC_DAT, RTC_CLK, RTC_RST);
RtcDS1302<ThreeWire> rtc(myWire);

// Keypad
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {12, 11, 10, 9};
byte colPins[COLS] = {8, 7, 6, 5};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

struct LogRecord {
  uint32_t unixtime;
  int sound;
  char key;
};

const int RECORD_SIZE = sizeof(LogRecord);
const int MAX_RECORDS = 100;
const int ADDR_COUNT = 0;

int lastSound = -1;
char lastKey = '-';
bool viewMode = false;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  rtc.Begin();
  rtc.SetIsWriteProtected(false);
  // Uncomment once to set the clock, then comment again
  // rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
  rtc.SetIsRunning(true);

  if (EEPROM.read(ADDR_COUNT) == 255) {
    EEPROM.write(ADDR_COUNT, 0);
  }

  lcd.clear();
  lcd.print("Sound Logger v3");
  delay(1500);
  Serial.println("LOGGER_READY");
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    lastKey = key;

    if (key == 'A') {
      viewLastRecord();
      viewMode = true;
    } else if (key == 'B') {
      clearLogs();
    } else if (key == '#') {
      viewMode = false;
    }
  }

  int sound = analogRead(A0);

  // Save immediately on significant change
  if (abs(sound - lastSound) >= 8) {
    saveRecord(sound);
    lastSound = sound;
  }

  if (!viewMode) {
    showMainScreen(sound);
  }

  delay(50);
}

void showMainScreen(int sound) {
  RtcDateTime now = rtc.GetDateTime();
  uint8_t count = EEPROM.read(ADDR_COUNT);

  lcd.setCursor(0, 0);
  print2digits(now.Hour());
  lcd.print(':');
  print2digits(now.Minute());
  lcd.print(':');
  print2digits(now.Second());
  lcd.print(" R:");
  print2digits(count);

  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(sound);
  lcd.print(" K:");
  lcd.print(lastKey);
  lcd.print("   ");
}

void saveRecord(int sound) {
  RtcDateTime now = rtc.GetDateTime();
  uint8_t count = EEPROM.read(ADDR_COUNT);

  if (count >= MAX_RECORDS) {
    return;
  }

  LogRecord rec;
  rec.unixtime = now.Epoch32Time();
  rec.sound = sound;
  rec.key = lastKey;

  int addr = 1 + count * RECORD_SIZE;
  EEPROM.put(addr, rec);
  EEPROM.write(ADDR_COUNT, count + 1);

  // Format: LOG,YYYY-MM-DD,HH:MM:SS,sound,key
  Serial.print("LOG,");
  Serial.print(now.Year());
  Serial.print('-');
  print2digitsSerial(now.Month());
  Serial.print('-');
  print2digitsSerial(now.Day());
  Serial.print(',');
  print2digitsSerial(now.Hour());
  Serial.print(':');
  print2digitsSerial(now.Minute());
  Serial.print(':');
  print2digitsSerial(now.Second());
  Serial.print(',');
  Serial.print(sound);
  Serial.print(',');
  Serial.println(lastKey);
}

void viewLastRecord() {
  uint8_t count = EEPROM.read(ADDR_COUNT);

  if (count == 0) {
    lcd.clear();
    lcd.print("No records");
    delay(1200);
    viewMode = false;
    return;
  }

  LogRecord rec;
  EEPROM.get(1 + (count - 1) * RECORD_SIZE, rec);
  RtcDateTime dt(rec.unixtime);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("#");
  lcd.print(count);
  lcd.print(" ");
  print2digits(dt.Hour());
  lcd.print(':');
  print2digits(dt.Minute());
  lcd.print(':');
  print2digits(dt.Second());

  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(rec.sound);
  lcd.print(" K:");
  lcd.print(rec.key);
}

void clearLogs() {
  EEPROM.write(ADDR_COUNT, 0);
  lastSound = -1;
  lcd.clear();
  lcd.print("Logs cleared");
  delay(1200);
  viewMode = false;
  Serial.println("LOGS_CLEARED");
}

void print2digits(int value) {
  if (value < 10) {
    lcd.print('0');
  }
  lcd.print(value);
}

void print2digitsSerial(int value) {
  if (value < 10) {
    Serial.print('0');
  }
  Serial.print(value);
}