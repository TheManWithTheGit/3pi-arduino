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
int TILTpin = 0;
unsigned int sensorWithNoise=0;
unsigned int position;
unsigned int sensorOne;
unsigned int sensorTwo;
unsigned int sensorThree;
unsigned int sensorFour;
unsigned int sensorFive;

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
  
  OrangutanBuzzer::playFrequency(1000, 500, 14);
 
}

void loop () {
  LINE_func();
}

void LINE_func(void) {
  //turns the robot 180
  set_motors(255,-255);
  delay(120);
  set_motors(0,0);
  //defining and initialising ldrs for line follow to zero.
  while (1) {
  // Get the position of the line.  Note that we *must* provide
  // the "sensors" argument to read_line() here, even though we
  // are not interested in the individual sensor readings.
  sensorWithNoise = bot.readLine(sensors, IR_EMITTERS_ON);
    
    //denoising the IR sensors
    position=noiseFilter(sensorWithNoise);
    
    sensorOne=noiseFilter(sensors[0]);
    sensorTwo=noiseFilter(sensors[1]);
    sensorThree=noiseFilter(sensors[2]);
    sensorFour=noiseFilter(sensors[3]);
    sensorFive=noiseFilter(sensors[4]);
    
  //if the robot doesn't detect the line on all sensors, enters search mode
  if (sensorOne < 1000 && sensorTwo < 1000 && sensorThree < 1000 && sensorFour < 1000 && sensorFive < 1000)
    SEARCH_mode; 
  
  TILTpin=analogRead(5); //reading the tilt sensor
  if (TILTpin < 355) //checking if the robot is angled for the seesaw
  {
    motionState=TILT_BALANCE;
    break;
  }
  // The "centrePos" term should be 0 when we are on the line.
  int centrePos = (int)position - 2000;

  // Compute the change in position and the current absolute position
  int delta = centrePos - lastCentrePos;
  integral += centrePos;

  // Remember the last position.
  lastCentrePos = centrePos;

  //this equation will increase/decrease motor speed depending on the conditions above and constants
  int powerDiff = centrePos/20 + integral/10000 + delta*3/2;

  // Compute the actual motor settings.  We never set either motor
  // to a negative value.
  const int maximum = 70; //limiter if the difference is too high, will only travel at this speed
  if (powerDiff > maximum)
    powerDiff = maximum;
  if (powerDiff < -maximum)
    powerDiff = -maximum;

  if (powerDiff < 0) //otherwise use the unchanged value to the motors.
    set_motors((maximum + powerDiff)*3, maximum*3);
  else
    set_motors(maximum*3, (maximum - powerDiff)*3);
  } 
}

