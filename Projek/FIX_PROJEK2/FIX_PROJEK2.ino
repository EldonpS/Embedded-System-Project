#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <AltSoftSerial.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Deklarasikan pin dan objek yang diperlukan
#define SS_PIN 10
#define RST_PIN 7
#define BUZZER 6
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN);
AltSoftSerial neogps;
SoftwareSerial SIM800L(2, 3);
TinyGPSPlus gps;

unsigned long previousSecond = 0;
static unsigned long t1;
static unsigned long t2;
static unsigned long t3;
static unsigned long t4;
static unsigned long t5;
static unsigned long t_unlock = 3000;
static unsigned long t_lock = 3000;
unsigned long previousMillis = 0;  // Waktu sebelumnya untuk penambahan titik
const unsigned long interval = 500;
double latitude, longitude;
int counter = 0;
int detik = 0;
int kiri = 5, kanan = 4;
String response;
int lastStringLength = response.length();
String link;
byte cek = 0;

void setup() {

  setuptrack();
  Serial.begin(9600);  // Initiate a serial communication
  SPI.begin();         // Initiate SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  lcd.init();  // Initialize the lcd
  pinMode(kanan, OUTPUT);
  pinMode(kiri, OUTPUT);
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Tempelkan kartu");
  delay(2000);
}

void loop() {
  // Look for new cards
  GPS();
  baca();
  linkSms();
  

}

void baca(){
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  // Show UID on serial monitor
  //Serial.print("UID tag :");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanning");
  //Serial.print("NUID tag is :");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //Serial.print(mfrc522.uid.uidByte[i], HEX);
    lcd.print(".");
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
    delay(300);
  }
  //Serial.println();
  //Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == "63 54 09 1C" && cek == 0) {
    //Serial.println("Authorized access/Unlocking");
   // Serial.println();
    noTone(BUZZER);
    t1 = millis();
    lcd.clear();

    while (millis() - t1 <= t_lock) {
      lock();
      millis();

      // if (millis() / 1000 != previousSecond) {
      //   previousSecond = millis() / 1000;
      //   //Serial.print("Elapsed Time (s): ");
      //   //Serial.println(previousSecond);
      // }
      lcd.setCursor(0,0);
      lcd.print("Unlocking");

    }
    off();

    lcd.setCursor(0, 0);
    lcd.print("Sedang Meminjam");
    
    cek = 1;

  } else if (content.substring(1) == "63 54 09 1C" && cek == 1) {
    lcd.clear();
   // Serial.println("Authorized access/Locking");
    //Serial.println();
    t4 = millis();
    while (millis() - t4 <= t_lock) {
      lock();
      millis();

      // if (millis() / 1000 != previousSecond) {
      //   previousSecond = millis() / 1000;
      //   //Serial.print("Elapsed Time (s): ");
      //   //Serial.println(previousSecond);
      // }
      lcd.setCursor(0,0);
      lcd.print("Locking");

    }
    noTone(BUZZER);
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
    off();
    delay(1000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Terima kasih");
    t2 = 0;
    t3 = 0;
    cek = 0;
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tempelkan Kartu");

  } else {
    //Serial.println(" Access denied");
    tone(BUZZER, 300);
    delay(DENIED_DELAY);
    noTone(BUZZER);
  }
}

// int check() {
//   bool cardRemoved = false;
//   bool current, previous;
//   int counter = 0;
//   previous = !mfrc522.PICC_IsNewCardPresent();

//   while (!cardRemoved) {
//     current = !mfrc522.PICC_IsNewCardPresent();

//     if (current && previous) counter++;

//     previous = current;
//     cardRemoved = (counter > 2);
//     delay(50);
//   }

//   //Serial.println("Card was removed");
//   mfrc522.PICC_HaltA();
//   mfrc522.PCD_StopCrypto1();
//   return 1;
// }

// void hilang() {
//   off();
//   tone(BUZZER, 300);
//   baca();
// }

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

void setuptrack(){
  neogps.begin(9600);
  SIM800L.begin(9600);  
  SIM800L.println("AT+CMGF=1");
  delay(1000);
  SIM800L.println("AT+CNMI=2,2,0,0,0");
}

void GPS(){
  if(neogps.available()) {
    gps.encode(neogps.read());
  }
  if(gps.location.isUpdated()) {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    link = "www.google.com/maps/place/" + String(latitude, 6) + "," + String(longitude, 6) ;
    Serial.println(link);
  
  }
}

void linkSms(){
  if (SIM800L.available()>0){
      response = SIM800L.readStringUntil('\n');
    }

  if(lastStringLength != response.length()){
      GPS();
      //Perintah ON
      if(response.indexOf("FIND")!=-1){ //ganti FIND dengan keyword teman-teman
          
          SIM800L.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
          delay(1000);  // Delay of 1000 milli seconds or 1 second
          SIM800L.println("AT+CMGS=\"081335372942\"\r"); //ganti no hp kalian ya
          delay(1000);
          SIM800L.println(link);// The SMS text you want to send
          delay(100);
          SIM800L.println((char)26);// ASCII code of CTRL+Z
          delay(1000);
      } 
  }
}