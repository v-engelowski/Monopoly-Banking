//*******************************************************************************************************
//**  Beispielsketch für das Auslesen von ID-Tags. Bei passender ID wird PIN 13 für 2 Sekunden         **
//**  eingeschaltet (LED). Bei falscher ID wird an Pin 12 ein Ton erzeugt. (PASSIVEN Buzzer / Piezzo)  **
//**  nutzen. Statt einer LED kann auch ein Türschloss geschaltet werden (über TIP120 Schaltverstärker **
//*******************************************************************************************************
#include <Wire.h>            // Bibliothek fürs I2C Protokoll
#include <SPI.h>             // Bibliothek fürs SPI Protokoll
#include <Adafruit_PN532.h>  // Bibliothek für das NFC/RFID Modul !!! Bitte mittels Arduino IDE laden !!!
#include <LiquidCrystal_I2C.h>


const byte PN532_SCK = 2;   //
const byte PN532_MOSI = 3;  // Definiere die Anschlüsse für
const byte PN532_SS = 4;    // die SPI Verbindung zum RFID
const byte PN532_MISO = 5;  // Board


Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);  // Instanz erzeugen mit SPI Protokoll
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {               // Beginn Setup Funktion
  Serial.begin(115200);      // Öffne serielle Übertragung mit 115200 Baud (ser Monitor gleiche Baud Einstellung!)

  lcd.init();
  lcd.setBacklight(1);

  nfc.begin();                                           // Beginne Kommunikation mit RFID Leser
  unsigned long versiondata = nfc.getFirmwareVersion();  // Lese Versionsnummer der Firmware aus
  if (!versiondata) {                                    // Wenn keine Antwort kommt
    Serial.print("Kann kein Board finden !");            // Sende Text "Kann kein..." an seriellen Monitor
    while (1);
  }
  Serial.print("NFC Reader found!");
  Serial.println((versiondata >> 24) & 0xFF, HEX);  // Sende Text und Versionsinfos an seriellen
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);  // Monitor, wenn Antwort vom Board kommt
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);        //
  nfc.SAMConfig();                                       // Konfiguriere Board RFID Tags zu lesen
}

void loop() {                               // Beginne Loop-Funktion
  uint8_t success;                          // Variable anlegen
  uint8_t uid[8];
  uint8_t uidLength;
  // Warte auf einen ISO14443A Chip. Wird ein solcher entdeckt, wird die Variable
  // mit der UID gefüllt. Abhängig von der Länge (4 bytes (Mifare Classic) oder
  // 7 bytes (Mifare Ultralight) wird der Kartentyp erkannt.
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {  // Wenn erkannt wird, arbeite ab...
    if (uidLength == 7) {  // Hat die Karte/der Chip 4 byte Länge...
      // Mifare Classic Karte
      uint8_t data[32];

      nfc.ntag2xx_ReadPage(6, data);  // Read block 6 - the first user writable block

      int first = (char)data[0] - '0';  // Converts the first 2 HEX bits to char and subtracts '0' to get the int value
      int second = (char)data[1] - '0';

      int complete = (first * 10) + second;  //concats two ints

      Serial.print(complete);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println(complete);
    }
  }  // Ende der IF Abfrage/Schleife

  delay(500);
}