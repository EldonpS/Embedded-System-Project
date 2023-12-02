#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Sim800l.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER 6  
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000

//LOCK SYSTEM
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN);  
unsigned long previousSecond = 0;

static unsigned long t1;
static unsigned long t2;
static unsigned long t3;
static unsigned long t_unlock = 10000;
static unsigned long t_lock = 10000;

int detik = 0;
int kanan = A0;
int kiri = A1;

//SUB SYSTEM GPS GSM
Sim800l Sim800l;
String text;
char* number;
char pesan[100];
bool error;
bool pinjam = false;

int RXPin_GPS = 4;
int TXPin_GPS = 3;
int RXPin_SIM = 1;
int TXPin_SIM = 2;

double latitude, longitude;
TinyGPSPlus gps;

SoftwareSerial gpsSerial(RXPin_GPS, TXPin_GPS);
SoftwareSerial SIM800L(RXPin_SIM, TXPin_SIM);

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  SIM800L.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  pinMode(kanan, OUTPUT);
  pinMode(kiri, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Tempelkan kartu");
  Serial.println("Put your card to the reader...");
  Serial.println();
  delay(2000);
}

void loop() {
  bacaKartu();
  lokasi();
  bacaPesan();
}

void bacaKartu() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID tag: ");
  String content = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();
  Serial.print("Message: ");
  content.toUpperCase();

  if (content.substring(1) == "63 54 09 1C") {
    authorizedAccess();
  } else if (content.substring(1) == "A5 22 77 53") {
    authorizedAccess1();
  } else {
    accessDenied();
  }
}

int check() {
  bool cardRemoved = false;
  int counter = 0;
  bool current, previous;
  previous = !mfrc522.PICC_IsNewCardPresent();

  while (!cardRemoved) {
    current = !mfrc522.PICC_IsNewCardPresent();

    if (current && previous) counter++;

    previous = current;
    cardRemoved = (counter > 2);
    delay(50);
  }

  Serial.println("Card was removed");
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  return 1;
}

void authorizedAccess() {
 Serial.println("Authorized access");
    Serial.println();
    t1 = millis();
    delay(500);
    noTone(BUZZER);

    while (millis() - t1 >= t_unlock) {
      unlock();
      millis();

      if (millis() / 1000 != previousSecond) {
        previousSecond = millis() / 1000;
        Serial.print("Elapsed Time (s): ");
        Serial.println(previousSecond);
      }
    }
    off();

    t1 = millis();

    if (check() == 1) {
      hilang();
      t2 = millis() - t1;
      t3 = t2 + t3;
      detik = t3 / 1000;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pinjam selama ");
      lcd.setCursor(0, 1);
      lcd.print(detik);
      lcd.setCursor(3, 1);
      lcd.print(" detik");
      t1 = millis();
    }
}

void authorizedAccess1(){
  Serial.println("Authorized access");
    Serial.println();
    delay(500);
    noTone(BUZZER);

    t1 = millis();

    while (millis() - t1 <= t_lock) {
      lock();
      unsigned long cek1 = millis();
      millis();

      if (millis() / 1000 != previousSecond) {
        previousSecond = millis() / 1000;
        Serial.print("Elapsed Time (s): ");
        Serial.println(previousSecond);
      }
    }
    off();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Terima kasih");
    t2 = 0;
    t3 = 0;
}

void accessDenied() {
  Serial.println("Access denied");
  lcd.setCursor(0, 0);
  lcd.print("Tempelkan kartu");
  tone(BUZZER, 300);
  delay(DENIED_DELAY);
  noTone(BUZZER);
}

void lokasi() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isUpdated()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        Serial.print("Latitude: ");
        Serial.println(latitude, 6);
        Serial.print("Longitude: ");
        Serial.println(longitude, 6);
        Serial.print("Altitude: ");
        Serial.println(gps.altitude.meters());
        sms();
      }
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("No GPS detected");
    while (true);
  }
}

void sms() {
  if (gps.location.isUpdated()) {
    Sim800l.begin();
    text = "Google Maps: www.google.com/maps/place/" + String(latitude) + "," + String(longitude);
    Serial.println(text);
    text.toCharArray(pesan, 100);
    number = "089658291407"; // no tujuan 089658291407
    error = Sim800l.sendSms(number, pesan);
  }
}

void bacaPesan() {
  if (SIM800L.available()) {
    delay(10);
    char incomingChar = SIM800L.read();
    text += incomingChar;
    if (text.endsWith("\n")) {
      text.trim(); // Menghapus karakter spasi atau newline pada awal dan akhir pesan
      if (text.equalsIgnoreCase("Request")) {
        Serial.println("Menerima pesan Request");
        sms();
      }
      text = ""; // Mengosongkan variabel text untuk membaca pesan selanjutnya
    }
  }
}


void hilang() {
  off();
  tone(BUZZER, 300);
  bacaKartu();
}

void lock() {
  digitalWrite(kanan, HIGH);
  digitalWrite(kiri, LOW);
}

void unlock() {
  digitalWrite(kanan, LOW);
  digitalWrite(kiri, HIGH);
}

void off() {
  digitalWrite(kanan, LOW);
  digitalWrite(kiri, LOW);
}
