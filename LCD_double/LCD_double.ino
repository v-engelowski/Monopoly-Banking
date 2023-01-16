#include <Wire.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();

  lcd.setBacklight(1);

  lcd.setCursor(0, 0);
  lcd.print("Reihe 1");
}

void loop() {
  // put your main code here, to run repeatedly:

}
