
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
unsigned int counter = 0;
unsigned int IRone=23;
unsigned int IRtwo=24;
unsigned int IRthree=25;
unsigned int IRfour=26;
unsigned int IRfive=27;

unsigned int sIRone=23;
unsigned int sIRtwo=24;
unsigned int sIRthree=25;
unsigned int sIRfour=26;
unsigned int sIRfive=27;
unsigned int out=0;

void setup () {
  lcd.gotoXY(0, 0);
  print("wait");
  wait_for_button_press(ANY_BUTTON);
  delay(1000);
  unsigned int counter; // used as a simple timer

  // This must be called at the beginning of 3pi code, to set up the
  // sensors.  We use a value of 2000 for the timeout, which
  // corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
  bot.init(2000);
  // Auto-calibration: turn right and left while calibrating the
  // sensors.
  for (counter=0; counter<160; counter++)
  {
    if (counter < 40 || counter >= 120)
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
    delay(1);
  }
  set_motors(0, 0);
  
  OrangutanBuzzer::playFrequency(3000, 500, 14);
 
 
}

void loop () {
  LINE_func();
}

void LINE_func(void) {
 //defining and initialising ldrs for line follow to zero.
  while (1) {
    unsigned int out = bot.readLine(sensors, IR_EMITTERS_ON);
    clear();
   int left_value;
   int right_value;
  unsigned int outOne;
  unsigned int outTwo;
  outOne=out[0];
  outTwo=out[4];
  left_value = analogRead(outOne);
  right_value = analogRead(outTwo);
  lcd.gotoXY(0, 0);
  print("l: ");
  lcd_goto_xy(3, 0);
  lcd.print(left_value);
  lcd.gotoXY(0, 1);
  lcd.print("r: ");
  lcd.gotoXY(3, 1);
  lcd.print(right_value);
  delay(200);
  }
}


//pin 23 to 27 are the IR pins
