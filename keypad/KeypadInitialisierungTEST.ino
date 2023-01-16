#include <Keypad.h>

const byte COL = 4;
const byte ROW = 4;

const byte COL_PINS[COL] = { 5, 4, 3, 2 };
const byte ROW_PINS[ROW] = { 6, 7, 8, 9 };

const char KEYS[ROW][COL] = {
  { '*', '0', '#', 'D' },
  { '7', '8', '9', 'C' },
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' }
};

Keypad keypad = Keypad(makeKeymap(KEYS), ROW_PINS, COL_PINS, ROW, COL);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();

  if(key){

    Serial.print(key);
  }
}
