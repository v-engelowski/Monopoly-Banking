#define DEBUG 1 /*!< If debug mode is to be used*/


#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)      /*!< Redefines Serial.print(x) as DEBUG_PRINT so it only outputs to the serial monitor, if DEBUG is defined */
#define DEBUG_PRINTLN(x) Serial.println(x)   /*!< Redefines Serial.println(x) as DEBUG_PRINTLN so it only outputs to the serial monitor, if DEBUG is defined */
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif


#define DEFAULT_DELAY 1500  /*!< Default delay used in the lcd_print function */


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


const byte PN532_SCK = 2;   /*!< Port for SCK */
const byte PN532_MOSI = 3;  /*!< Port for MOSI */
const byte PN532_SS = 4;    /*!< Port for SS */
const byte PN532_MISO = 5;  /*!< Port for MISO */


const byte COL = 4; /*!< Number of columns on the keypad */
const byte ROW = 4; /*!< Number of rows on the keypad */

const byte COL_PINS[] = { 6, 7, 8, 9 };     /*!< Which pins are used as columns */
const byte ROW_PINS[] = { 10, 11, 12, 13 }; /*!< Which pins are used as row */

const char KEYS[ROW][COL] = {
  { 'D', '#', '0', '*' },
  { 'C', '9', '8', '7' },
  { 'B', '6', '5', '4' },
  { 'A', '3', '2', '1' }
};    /*!< What each button represents on the keypad */

const char validChars[] = { '1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D' };   /*!< Valid keypresses */

Keypad keypad = Keypad(makeKeymap(KEYS), ROW_PINS, COL_PINS, ROW, COL);   /*!< Keypad instance */


const uint8_t validIDs[] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }; /*!< List of valid IDs */

const uint8_t playerCount = 12;       /*!< Amount of cards */
const uint8_t firstValidPlayerID = 5; /*!< First valid ID for player */

const uint8_t bankID = 3;             /*!< ID of bank */
const uint8_t parkingID = 4;          /*!< ID for free parking */

const uint32_t defaultMoney = 30000;        /*!< Starting credits for players */
const uint32_t defaultMoneyBank = 600000;   /*!< Starting credits for bank */

uint32_t bank[13];  /*!< Int array where bank balance is stored */


const char names[][16] = { "", "", "", "Stonks", "Parken", "BuegelEisen", "NikeAirMax", "Aida", "Korken", "Zylinder", "Excavator", "La Tortuga", "Big D" };   /*!< Array of names for the players */


Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);  /*!< Instanz erzeugen mit SPI Protokoll */
LiquidCrystal_I2C lcd(0x27, 16, 2);   /*!< Instanz erzeugen für LCD */


/**
* @brief One time set up of variables and objects
*
* This function is called right after the Arduino boots up
*/

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


/**
* @brief Main loop function used by Arduino
*
* This function runs indefinitely
*/

void loop() {
  while (1) {
    char userInput;
    uint8_t id1 = 0;
    uint8_t id2 = 0;
    uint32_t transferAmount = 0;
    bool transactionSuccess;

    lcd_print(WAIT_INPUT_TEXT1, WAIT_INPUT_TEXT2, 0);

    userInput = getKeypadInput();

    if (userInput == '*') {
      lcd_print("Warte auf", "Sender");
      while(!id1) id1 = readNFC();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(names[id1]);
      lcd.setCursor(0, 1);
      lcd.print("erkannt");
      delay(DEFAULT_DELAY);

      lcd_print("Warte auf", "Empfaenger");
      while(!id2) id2 = readNFC();
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

      transactionSuccess = transaction(bankID, id1, 4000);

      if (!transactionSuccess) lcd_print("Nicht genug C", "in der Bank");
      else lcd_print("4000 Credits", "ueberwiesen", 2000);

      continue;
    }
  }
}

/**
* @brief Displays 2 Strings on the LCD with optional timeout
*
* Displays the first parameter on the first line, and the second parameter at the second line. Optionally waits before returning.
*
* @param text1[]: Char array / srting to display on line 1
* @param text2[]: Char array / srting to display on line 2
* @param timeout(optional): How many miliseconds to display the text
* 
*/

void lcd_print(char text1[], char text2[], uint16_t timeout = 1000) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text1);
  lcd.setCursor(0, 1);
  lcd.print(text2);

  if (timeout > 0) delay(timeout);
}

/**
* @brief Waits for a properly formated NFC-Tag to be scanned and returns the conatining ID
*
* As long as the NFC reader has not detected a valid card, it keeps rescanning for cards. If it has scanned a card, it tests if it contains a valid ID.
*
* @return ID stored on card
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

        for (uint8_t i = 0; i < sizeof(validIDs); i++) {
          Serial.println(String(i));    // !!do not remove, program doesn't work without it for some reason!!
          if (validIDs[i] == complete) return complete;
        }

        complete = 0;
      }
    }
  }

  return complete;
}


/**
* @brief Waits for a input on the keyboard
*
* If no valid key or no keypress is pressed, it waits until one is pressed.
*
* @return A valid char pressed on keypad
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


/**
* @brief Subtracts amount from sender and adds amount to receiver
*
* This function first checks, if the sender has enough money in the bank. If there is enough money, subtracts the amount to send from sender, and adds it to the receiver.
*
* @param sender: Id from player who wants to send money
* @param receiver: Id from player who receives the money
* @param amount: How much money to send
*
* @return bool If transaction was successful
*
*/

bool transaction(uint8_t sender, uint8_t receiver, uint32_t amount) {
  uint32_t senderCredits = bank[sender];
  uint32_t receiverCredits = bank[receiver];

  if (senderCredits < amount) return false;

  bank[sender] = bank[sender] - amount;
  bank[receiver] = bank[receiver] + amount;

  return true;
}


/**
* @brief Concats chars pressed on Keypad and converts them to int
*
* Stores the last keypresses in an array, displays the keypresses on the LCD and converts it to an int.
* 
* @return: int value of number char array
*/

uint32_t cashinput() {
  char input[6];
  uint32_t total = 0;
  uint8_t digitCount = 0;

  char key;

  lcd.setCursor(0, 1);
  while (1) {

    key = getKeypadInput();

    if (key == '#') break;
    if (digitCount >= 6) continue;
    if (key == 'A' | key == 'B' | key == 'C') continue;
    if (key == '*') return 0;

    // input[digitCount] = ((int)(char)key - '0');
    input[digitCount] = key;

    total = String(input).toInt();

    digitCount++;


    lcd.print(key);
  }

  DEBUG_PRINTLN(total);

  return total;
}


#ifdef DEBUG
/**
* This only runs if the DEBUG macro is set
*
* Prints the content of the bank array
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
