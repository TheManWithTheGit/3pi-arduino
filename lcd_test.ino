#include <OrangutanLCD.h>
#include <OrangutanAnalog.h>
OrangutanLCD lcd;
OrangutanAnalog analog;

int left_ldr = 6;
int right_ldr = 7;
int left_value = 0;
int right_value = 0;

void setup() {
 analog.setMode(MODE_10_BIT);    // 10-bit analog-to-digital conversions
}

void loop() {
  left_value = analogRead(left_ldr);
  right_value = analogRead(right_ldr);
  lcd.gotoXY(0, 0);
  //print("l: ");
  //lcd_goto_xy(3, 0);
  lcd.print(left_value);
  lcd.gotoXY(0, 1);
  lcd.print("r: ");
  lcd.gotoXY(3, 1);
  lcd.print(right_value);
  delay(1000);
}
  
