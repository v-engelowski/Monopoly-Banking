#include <Keypad.h>

const byte COL = 4;
const byte ROW = 4;

const byte COL_PINS[COL] = { 5, 4, 3, 2 };
const byte ROW_PINS[ROW] = { 6, 7, 8, 9 };

const char KEYS[ROW][COL] = {
  { '1', '4', '7', '*' },
  { '2', '5', '8', '0' },
  { '3', '6', '9', '#' },
  { 'A', 'B', 'C', 'D' }
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
