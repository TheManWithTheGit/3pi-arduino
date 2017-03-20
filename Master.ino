#include <Pololu3pi.h>
#include <PololuQTRSensors.h>
#include <OrangutanMotors.h>
#include <OrangutanAnalog.h>
#include <OrangutanLEDs.h>
#include <OrangutanLCD.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanBuzzer.h>

void motors_init(void);
OrangutanLCD lcd;
OrangutanAnalog analog;
Pololu3pi bot;

// variables for the LDR_func
int left_ldr = 6;
int right_ldr = 7;
int left_value = 0;
int right_value = 0;
// variables for the LINE_func

//variables for the TILT_func

//variables for the SEARCH_MODE
unsigned int sensorsSearch[5];
//related to switching states for the course
int motionState;
#define LDR_FOLLOW 1;
#define LINE_FOLLOW 2;
#define TILT_BALANCE 3;


void setup() {
	OrangutanBuzzer::playFrequency(3000, 250, 14);
	clear();
	lcd.gotoXY(0, 0);
	print("SELECT");
	lcd.gotoXY(0, 1);
	print("MODE:");
	unsigned char buttonPress = get_single_debounced_button_press(ANY_BUTTON);
	
	if (buttonPress & BUTTON_A) 
	{
		motionState=LDR_FOLLOW;
		clear();
		lcd.gotoXY(0, 0);
		print("LDR");
		OrangutanBuzzer::playFrequency(3000, 250, 14);
	}
	if (buttonPress & BUTTON_B) 
	{
		motionState=LINE_FOLLOW;
		clear();
		lcd.gotoXY(0, 0);
		print("LINE");
		OrangutanBuzzer::playFrequency(3000, 250, 14);
	}
	if (buttonPress & BUTTON_C) 
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
	switch (motionState)
	{
	case LDR_FOLLOW:
		LDR_func();
		

	case LINE_FOLLOW:
		LINE_func();
		

	case TILT_BALANCE:
		TILT_func();
		
	}

}

void LDR_func(void) {
	//todo, merged from other branches.
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
 	 set_motors(50,70);
 	 read_line_sensors(sensors, IR_EMITTERS_ON);
 	 if (sensors[0] > 900 || sensors[1] > 900 || sensors[2] > 900 || sensors[3] > 900 || sensors[4] > 900)
	 {
	     motionState=LINE_func;
 	     break;
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

	if (counter < 5) //since the buffer still has empty cells, return the first few numbers until its full
	{
		counter++;
			return number;
	}
	else //when the buffer is full, sort
	{
		for (i = 0; i < 5; i++)
		{
			bufferTwo[i] = bufferOne[i]; //puttng values into other buffer
		}
		while (swap) //start of the sorting part
		{
			swap = false; //sets the loop to run once
			j++;
			for (i = 0; i < 4 - j; i++) //takes lowest value and moves it
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
