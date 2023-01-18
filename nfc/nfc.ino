#define DEBUG 1

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif


#define DEFAULT_DELAY 1500

// LCD has only 16 chars
#define WELCOME_TEXT1 "Willkommen zu"
#define WELCOME_TEXT2 "Emscopoly!"

#define WAIT_INPUT_TEXT1 "* - Transaktion"
#define WAIT_INPUT_TEXT2 "# - Kontostand"


#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

void lcd_print(char text1[], char text2[], uint16_t timeout = 1000);

// NP532
const byte PN532_SCK = 2;   //
const byte PN532_MOSI = 3;  // Definiere die Anschlüsse für
const byte PN532_SS = 4;    // die SPI Verbindung zum RFID
const byte PN532_MISO = 5;  // Board

// Keypad
const byte COL = 4;
const byte ROW = 4;

const byte COL_PINS[] = { 6, 7, 8, 9 };
const byte ROW_PINS[] = { 10, 11, 12, 13 };

const char KEYS[ROW][COL] = {
  { 'D', '#', '0', '*' },
  { 'C', '9', '8', '7' },
  { 'B', '6', '5', '4' },
  { 'A', '3', '2', '1' }
};

const char validChars[] = { '1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D' };

Keypad keypad = Keypad(makeKeymap(KEYS), ROW_PINS, COL_PINS, ROW, COL);

// Banking system
const uint8_t playerCount = 12;
const uint8_t firstValidPlayerID = 5;

const uint8_t bankID = 3;
const uint8_t parkingID = 4;

const uint16_t defaultMoney = 1500;
const uint16_t defaultMoneyBank = 15000;

uint16_t bank[13];


const char names[][16] = { "", "", "", "Stonks", "Parken", "BuegelEisen", "NikeAirMax", "Aida", "Korken", "Zylinder", "Excavator", "La Tortuga", "Big D" };


Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);  // Instanz erzeugen mit SPI Protokoll
LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup() {  // Beginn Setup Funktion
#ifdef DEBUG
  Serial.begin(115200);  // Öffne serielle Übertragung mit 115200 Baud (ser Monitor gleiche Baud Einstellung!)
#endif

  lcd.init();
  lcd.setBacklight(1);

  nfc.begin();  // Beginne Kommunikation mit RFID Leser

  unsigned long versiondata = nfc.getFirmwareVersion();  // Lese Versionsnummer der Firmware aus

  if (!versiondata) {                         // Wenn keine Antwort kommt
    DEBUG_PRINT("Kann kein Board finden !");  // Sende Text "Kann kein..." an seriellen Monitor
    while (1)
      ;
  }

  DEBUG_PRINTLN("NFC Reader found!");
  DEBUG_PRINT("Firmware ver. ");
  DEBUG_PRINT((versiondata >> 16) & 0xFF);  // Monitor, wenn Antwort vom Board kommt
  DEBUG_PRINT('.');
  DEBUG_PRINTLN((versiondata >> 8) & 0xFF);  //
  DEBUG_PRINTLN("");
  nfc.SAMConfig();  // Konfiguriere Board RFID Tags zu lesen

  // Bank init
  for (uint8_t i = 1; i < playerCount + 1; i++) {
    if (i < 4 && i != 3) bank[i] = 0;
    if (i == 3) bank[i] = defaultMoneyBank;
    if (i > 4) bank[i] = defaultMoney;
  }

  DEBUG_PRINTLN("Bank Init complete:");
  for (uint8_t i = 3; i < playerCount + 1; i++) {
    DEBUG_PRINTLN("ID " + String(i) + " hat " + String(bank[i]) + " Credits");
  }

  lcd_print(WELCOME_TEXT1, WELCOME_TEXT2, 3000);
}


void loop() {
  while (1) {
    char userInput;
    uint8_t id1;
    uint8_t id2;
    char name[] = "Spieler";
    uint16_t transferAmount;
    bool transactionSuccess;

    lcd_print(WAIT_INPUT_TEXT1, WAIT_INPUT_TEXT2, 0);

    userInput = getKeypadInput();

    if (userInput == '*') {
      lcd_print("Warte auf", "Sender");
      id1 = readNFC();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(names[id1]);
      lcd.setCursor(0, 1);
      lcd.print("erkannt");
      delay(DEFAULT_DELAY);

      lcd_print("Warte auf", "Empfaenger");
      id2 = readNFC();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(names[id2]);
      lcd.setCursor(0, 1);
      lcd.print("erkannt");
      delay(DEFAULT_DELAY);

      lcd_print("Betrag", "");
      if (cashinput) transferAmount = cashinput();
      else continue;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bestaetigen mit");
      lcd.setCursor(0, 1);
      lcd.print("'#'");

      userInput = getKeypadInput();

      if (userInput == '#') {
        lcd_print("Ueberweising in", "bearbeitung...");

        transactionSuccess = transaction(id1, id2, transferAmount);

        if (transactionSuccess) lcd_print("Erfolgreich", "");
        else lcd_print("Nicht genug", "Credits");

      } else lcd_print("Ueberweisung", "abgebrochen");

      debugPrintBank();

      continue;
    }

    if (userInput == '#') {
      lcd_print("Bitte Card", "dranhalten");

      id1 = readNFC();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(names[id1]);
      lcd.setCursor(0, 1);
      lcd.print("erkannt");
      delay(DEFAULT_DELAY);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(String(bank[id1]) + " Credits");
      lcd.setCursor(0, 1);
      lcd.print("auf dem Konto");
      delay(2000);

      continue;
    }
    if (userInput == 'A') {
      lcd_print("Bitte Card", "dranhalten");
      id1 = readNFC();

      transactionSuccess = transaction(bankID, id1, 200);

      if (!transactionSuccess) lcd_print("Nicht genug C", "in der Bank");
      else lcd_print("200C wurden", "ueberwiesen", 2000);

      continue;
    }
  }
}

/*
  lcd_print

  Displays 2 Strings on the LCD with optional timeout

  text1[]: char array / srting to display on line 1
  text2[]: char array / srting to display on line 2
  timeout (optional): how many miliseconds to display the text
  
*/

void lcd_print(char text1[], char text2[], uint16_t timeout = 1000) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text1);
  lcd.setCursor(0, 1);
  lcd.print(text2);

  if (timeout > 0) delay(timeout);
}

/*
  Function: readNFC

  Waits for a properly formated NFC-Tag to be scanned and returns the conatining ID

  returns: ID stored on card
*/

uint8_t readNFC() {  // Beginne Loop-Funktion
  uint8_t success;   // Variable anlegen
  uint8_t uid[8];
  uint8_t uidLength;

  uint8_t complete;

  while (!complete) {
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);


    if (success) {
      if (uidLength == 7) {

        uint8_t data[32];

        nfc.ntag2xx_ReadPage(6, data);  // Read block 6 - the first user writable block

        uint8_t first = (char)data[0] - '0';  // Converts the first 2 HEX bits to char and subtracts '0' to get the int value
        uint8_t second = (char)data[1] - '0';

        uint8_t complete = (first * 10) + second;  //concats two ints

        return complete;
      }
    }
  }
}


/*
  Function: getKeypadInput

  Waits for a input on the keyboard

  returns: valid char pressed on keypad
*/

char getKeypadInput() {
  char keyInput;
  uint8_t validKey = 0;

  while (!validKey) {
    keyInput = keypad.getKey();

    for (uint8_t i = 0; i < sizeof(validChars); i++) {
      if (keyInput == validChars[i]) validKey = 1;
    }
  }

  return keyInput;
}


/*
  Function: transaction

  Subtracts amount from sender and adds amount to receiver

  sender: id from player who wants to send money
  receiver: id from player who receives the money
  amount: how much money to send

  returns: if transaction was successful

*/

bool transaction(uint8_t sender, uint8_t receiver, uint16_t amount) {
  uint16_t senderCredits = bank[sender];
  uint16_t receiverCredits = bank[receiver];

  if (senderCredits < amount) return false;

  bank[sender] = bank[sender] - amount;
  bank[receiver] = bank[receiver] + amount;

  return true;
}

int cashinput() {
  char input[5];
  uint16_t total = 0;
  uint8_t digitCount = 0;

  char key;

  lcd.setCursor(0, 1);
  while (1) {

    key = getKeypadInput();

    if (key == '#' | digitCount >= 5) break;
    if (key == 'A' | key == 'B' | key == 'C') continue;
    if (key == '*') return 0;
    if (key == 'D' && digitCount > 0) {
      input[digitCount - 1] = '\0';
      digitCount--;
      continue;
    }

    // input[digitCount] = ((int)(char)key - '0');
    input[digitCount] = key;

    total = String(input).toInt();

    digitCount++;


    lcd.print(key);
  }

  // DEBUG_PRINTLN(total);

  return total;
}


#ifdef DEBUG
/*
  debugPrintBank

  Prints the content of the bank array
*/

void debugPrintBank() {
  for (uint8_t i = 3; i < playerCount; i++) {
    if (i == 3) {
      DEBUG_PRINTLN("Bank has " + String(bank[i]) + " Credits");
      continue;
    }
    if (i == 4) {
      DEBUG_PRINTLN("Parking has " + String(bank[i]) + " Credits");
      continue;
    }
    DEBUG_PRINTLN("Player " + String(i) + " has " + String(bank[i]) + " Credits");
  }
}
#endif
