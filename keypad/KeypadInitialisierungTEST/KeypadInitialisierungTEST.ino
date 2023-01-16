#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte COL = 4;
const byte ROW = 4;

const byte COL_PINS[COL] = { 5, 4, 3, 2 };
const byte ROW_PINS[ROW] = { 6, 7, 8, 9 };

const char KEYS[ROW][COL] = {
  { '*', '0', '#', 'D' },
  { '7', '8', '9', 'C' },
  { '4', '5', '6', 'B' },
  { '1', '2', '3', 'A' }
};

Keypad keypad = Keypad(makeKeymap(KEYS), ROW_PINS, COL_PINS, ROW, COL);



void setup() {
  Serial.begin(9600);
  Serial.flush();

  lcd.init();
  lcd.setBacklight(1);
}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();

  if(key){

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(key);
  }
}
