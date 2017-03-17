
#include <Pololu3pi.h>
#include <PololuQTRSensors.h>
#include <OrangutanMotors.h>
#include <OrangutanAnalog.h>
#include <OrangutanLEDs.h>
#include <OrangutanLCD.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanBuzzer.h>

void motors_init(void);

OrangutanLCD lcd; //reassigning the libraries to something easier to type
OrangutanAnalog analog;
Pololu3pi bot;
unsigned int sensors[5]; // an array to hold sensor values
unsigned int lastCentrePos = 0;
long integral = 0;
int counter = 0;
int IRone=23;
int IRtwo=24;
int IRthree=25;
int IRfour=26;
int IRfive=27;


void setup () {
 /* delay(1000);
  unsigned int counter; // used as a simple timer

  // This must be called at the beginning of 3pi code, to set up the
  // sensors.  We use a value of 2000 for the timeout, which
  // corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
  bot.init(2000);
  // Auto-calibration: turn right and left while calibrating the
  // sensors.
  for (counter=0; counter<120; counter++)
  {
    if (counter < 40 || counter >= 80)
      set_motors(60, -60);
    else
      set_motors(-60, 60);

    // This function records a set of sensor readings and keeps
    // track of the minimum and maximum values encountered.  The
    // IR_EMITTERS_ON argument means that the IR LEDs will be
    // turned on during the reading, which is usually what you
    // want.
    bot.calibrateLineSensors(IR_EMITTERS_ON);

    // Since our counter runs to 80, the total delay will be
    // 80*20 = 1600 ms.
    delay(10);
  }
  set_motors(0, 0);
  */
}

void loop () {
  LINE_func();
}

void LINE_func(void) {
 //defining and initialising ldrs for line follow to zero.
  while (1) {
    clear();
  left_value = analogRead(IRone);
  right_value = analogRead(IRfive);
  lcd.gotoXY(0, 0);
  print("l: ");
  lcd_goto_xy(3, 0);
  lcd.print(left_value);
  lcd.gotoXY(0, 1);
  lcd.print("r: ");
  lcd.gotoXY(3, 1);
  lcd.print(right_value);
  delay(1000);
  }
}


//pin 23 to 27 are the IR pins
