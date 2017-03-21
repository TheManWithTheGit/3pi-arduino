#include <Pololu3pi.h>
#include <PololuQTRSensors.h>
#include <OrangutanMotors.h>
#include <OrangutanAnalog.h>
#include <OrangutanLEDs.h>
#include <OrangutanLCD.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanBuzzer.h>

void motors_init(void); //required for the code to work
//This shortens the libraries for easier typing
OrangutanLCD lcd;
OrangutanAnalog analog;
Pololu3pi bot;
//timer for debugging inputs/outputs
unsigned int previousMillis = 0; //could've made the variable long, but unnecessary
const int interval = 333; //updates every 1/3 of a sec
unsigned int currentMillis = 0;

//variables for the LDR_func
const int left_ldr = 6;
const int right_ldr = 7;
int left_value = 0;
int right_value = 0;
int z = 0;
int y = 0;
unsigned int sensors[5];
unsigned int position;
unsigned int sensorOne;
unsigned int sensorTwo;
unsigned int sensorThree;
unsigned int sensorFour;
unsigned int sensorFive;
int counter;
//variables for the LINE_func

//variables for the TILT_func

//variables for the SEARCH_MODE
unsigned int sensorsSearch[5];
//variables for the noiseFilter

//related to switching states for the course
int motionState;
#define LDR_FOLLOW 1;
#define LINE_FOLLOW 2;
#define TILT_BALANCE 3;


void setup() {
	
	bot.init(2000); //This is required for the line follower sensors and timings
	
	OrangutanBuzzer::playFrequency(3000, 250, 14); //beep
	clear();
	lcd.gotoXY(0, 0);
	print("SELECT");
	lcd.gotoXY(0, 1);
	print("MODE:");
	unsigned char buttonPress = OrangutanPushbuttons::getSingleDebouncedPress(ANY_BUTTON);
	//I want to get a button press to select the mode I want to start with
	
	if (buttonPress & BUTTON_A) //Button A = LDR following mode and everything after
	{
		motionState=LDR_FOLLOW;
		clear();
		lcd.gotoXY(0, 0);
		print("LDR");
		OrangutanBuzzer::playFrequency(3000, 250, 14); //the buzzes let me know its doing something
	}
	if (buttonPress & BUTTON_B) //Button B = line following mode and everything after
	{
		motionState=LINE_FOLLOW;
		clear();
		lcd.gotoXY(0, 0);
		print("LINE");
		OrangutanBuzzer::playFrequency(3000, 250, 14);
	}
	if (buttonPress & BUTTON_C) //Button C = seesaw balancing mode
	{
		motionState=TILT_BALANCE;
		clear();
		lcd.gotoXY(0, 0);
		print("TILT");
		OrangutanBuzzer::playFrequency(3000, 250, 14);
	}
	delay(500);
}

void loop() {
	//This is the main tree where all the functions get called to do thier job
	switch (motionState)//motionState changes in the functions when certain conditions are met
	{
	case LDR_FOLLOW:
		LDR_func();
		break;

	case LINE_FOLLOW:
		LINE_func();
		break;

	case TILT_BALANCE:
		TILT_func();
		break;
	}

}

void LDR_func(void) {
	//the calibration runs once, so it can detect the line
	for(y=0;y<1;y++)
	{
		for (counter = 0; counter<40; counter++)
	{
		if (counter < 10 || counter >= 30)//turns a bit to one side, turns all the way to the other side, then re-centeres
			set_motors(40, -40);
		else
			set_motors(-40, 40);

		//this turns on an IR emitter, and reads the value that the arduino calculates from the wierd sensor
		bot::calibrateLineSensors(IR_EMITTERS_ON);
		//reading very different values doesn't matter, so time is more important
		delay(1);
	}
		//stops the robot, just incase
	set_motors(0, 0);
	}
	
	
	//reads the values from the IR sensors, and cleans the signal
	bot::readLineSensors(sensors, IR_EMITTERS_ON);
	sensorOne = noiseFilter(sensors[0]);
	sensorTwo = noiseFilter(sensors[1]);
	sensorThree = noiseFilter(sensors[2]);
	sensorFour = noiseFilter(sensors[3]);
	sensorFive = noiseFilter(sensors[4]);

	//this is a timer to read the values from two sensors, so I can read instant values.
	//This was made to not use the delay() function because I can't halt the whole system
	currentMillis = millis();
	 if (currentMillis - previousMillis >= interval) 
	 {
  	 previousMillis = currentMillis; //resets the timer effectivly
	 clear();
	 lcd.gotoXY(0,0);
	 lcd.print(sensorOne);
	 lcd.gotoXY(0,1);
	 lcd.print(sensorFive);
	 }

	//reads the LDR values
	left_value = analogRead(left_ldr);
	right_value = analogRead(right_ldr);

	if (left_value < 500 || right_value < 500) //if the robot sees lights, do stuff, otherwise spin
	{

		
		if (sensorOne > 2000 || sensorTwo > 2000 || sensorThree > 2000 || sensorFour > 2000 || sensorFive > 2000)//if it detects the line, chnages to LINE_func
		{
			set_motors(120, -120); //this rotates the robot, since it will be facing the wrong way when it switches mode
			delay(250);
			set_motors(0, 0);
			motionState = LINE_FOLLOW;
			return; //breaks out of the LDR_func to move to the LINE_func
		}
		if (left_value < right_value) //I could've done the whole differential calc stuff, but this is lighter and does the job just as well
			set_motors(70, 140);
		else if (left_value > right_value)
			set_motors(140, 70);
	}
	else
		set_motors(110, -110); //spin spin spin

}
void LINE_func(void) {
	//todo, merged from other branches.
}
void TILT_func(void) {
	//todo, merged from other branches.
}
void SEARCH_mode(void) {
	while(1)
	{
 	 clear();
	 lcd.gotoXY(0, 0);
 	 print("SEARCH");
 	 set_motors(50,70);//this makes the robot spin in a large circle
 	 bot::readLineSensors(sensors, IR_EMITTERS_ON);
 	 if (sensors[0] > 1100 || sensors[1] > 1100 || sensors[2] > 1100 || sensors[3] > 1100 || sensors[4] > 1100)//This checks if any sensor is over a line, and if its is, resume
	 {
	     motionState=LINE_func;
 	     return;
	 }	
	}
	
}
//outputs an int, takes in an int, takes median of input values
int noiseFilter(int number) //noise filter so that the robot doesn't go crazy because of noise
{
	boolean swap = true; //setting up the switch statement for later
	int temp, i, j, counter = 0; //now making my variables
	int bufferOne[5] = {}; //this is where all the values go in
	int bufferTwo[5] = {}; //this is for when I need to sort the buffer

	for (i = 0; i <= 3; i++) //prevents the buffer from overflowing
	{
		bufferOne[i] = bufferOne[i + 1]; //shifts all the numbers down a cell
	}

	bufferOne[4] = number; //save the new number to the end of the buffer

	if (counter < 5) //when the buffer start its still has empty cells. This returns the first few numbers until its full
	{
		counter++;
			return number;
	}
	else //when the buffer is full, sort
	{
		for (i = 0; i < 5; i++)
		{
			bufferTwo[i] = bufferOne[i]; //puttng values into the other buffer
		}
		while (swap) //start of the sorting part
		{
			swap = false; //sets the loop to run once
			j++;
			for (i = 0; i < 4 - j; i++)
			{
				if (bufferTwo[i] > bufferTwo[i + 1]) //relativly simple bubble sort
				{
					temp = bufferTwo[i];
					bufferTwo[i] = bufferTwo[i + 1];
					bufferTwo[i + 1] = temp;
					swap = true;
				}
			}
		}
		return bufferTwo[2]; //once the sorting is done, returns the median value
	}
}
