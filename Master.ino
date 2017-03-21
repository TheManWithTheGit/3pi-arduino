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
int LDRcounter;
//variables for the LINE_func
unsigned int lastCentrePos = 0;
int long integral = 0;
int LINEcounter = 0;
int TILTpin = 0;
int TILTcentrePos = 0;
int TILTposition = 0;
int x = 0;
unsigned int sensorWithNoise = 0;

//variables for the TILT_func
int tiltFlat = 0;
int speed;
//variables for the SEARCH_MODE
unsigned int sensorsSearch[5];
//variables for the noiseFilter
boolean swap = true; //setting up the switch statement for later
int temp, i, j, counter = 0; //now making my variables
int bufferOne[5] = {}; //this is where all the values go in
int bufferTwo[5] = {}; //this is for when I need to sort the buffer
//related to switching states for the course
int motionState;
#define LDR_FOLLOW 1
#define LINE_FOLLOW 2
#define TILT_BALANCE 3






int long zx = 0;







void setup() {

	bot.init(2000); //This is required for the line follower sensors and timings

	tiltFlat = analogRead(5); //reads the tilt sensor and sets this as the desired value 

	OrangutanBuzzer::playFrequency(2000, 500, 10); //beep
	clear();
	lcd.gotoXY(0, 0);
	print("SELECT");
	lcd.gotoXY(0, 1);
	print("MODE:");
}

void loop() {

	unsigned char buttonPress = OrangutanPushbuttons::getSingleDebouncedPress(BUTTON_A | BUTTON_B | BUTTON_C);
	//I want to get a button press to select the mode I want to start with

	if (buttonPress & BUTTON_A) //Button A = LDR following mode and everything after
	{
		motionState = LDR_FOLLOW;
		clear();
		lcd.gotoXY(0, 0);
		print("LDR");
		OrangutanBuzzer::playFrequency(3000, 250, 7); //the buzz let me know its doing something
		buttonPress = 9; //This stops the if stack from running infintly once it has run once 
	}
	if (buttonPress & BUTTON_B) //Button B = line following mode and everything after
	{
		motionState = LINE_FOLLOW;
		clear();
		lcd.gotoXY(0, 0);
		print("LINE");
		OrangutanBuzzer::playFrequency(3000, 250, 7);
		buttonPress = 9;
	}
	if (buttonPress & BUTTON_C) //Button C = seesaw balancing mode
	{
		motionState = TILT_BALANCE;
		clear();
		lcd.gotoXY(0, 0);
		print("TILT");
		OrangutanBuzzer::playFrequency(3000, 250, 7);
		buttonPress = 9;
	}


	//This is the main tree where all the functions get called to do thier job
	switch (motionState)//motionState changes in the functions when certain conditions are met
	{
		case LDR_FOLLOW:
		{
			OrangutanBuzzer::playFrequency(3000, 500, 14);
			LDR_func();
		}
		break;
		case LINE_FOLLOW:
		{
			OrangutanBuzzer::playFrequency(3000, 500, 14);
			LINE_func();
		}
		break;
		case TILT_BALANCE:
		{
			OrangutanBuzzer::playFrequency(3000, 500, 14);
			TILT_func();
		}
		break;

	}

}

void LDR_func(void) {
	while (1) {
		//the calibration runs once, so it can detect the line
		for (y = 0; y < 1; y++)
		{
			//OrangutanBuzzer::playFrequency(2000, 10, 14);
			for (LDRcounter = 0; LDRcounter <= 40; LDRcounter++)
			{
				if (LDRcounter < 10 || LDRcounter >= 30)//turns a bit to one side, turns all the way to the other side, then re-centeres
					set_motors(40, -40);
				else
					set_motors(-40, 40);

				//this turns on an IR emitter, and reads the value that the arduino calculates from the wierd sensor
				bot.calibrateLineSensors(IR_EMITTERS_ON);
				//reading very different values doesn't matter, but time is more important
				delay(1);
			}
			//stops the robot, just incase
			set_motors(0, 0);
		}


		//reads the values from the IR sensors, and cleans the signal
		bot.readLineSensors(sensors, IR_EMITTERS_ON);
		sensorOne = noiseFilter(sensors[0]);
		sensorTwo = noiseFilter(sensors[1]);
		sensorThree = noiseFilter(sensors[2]);
		sensorFour = noiseFilter(sensors[3]);
		sensorFive = noiseFilter(sensors[4]);

		//reads the LDR values
		left_value = analogRead(left_ldr);
		right_value = analogRead(right_ldr);

		//this is a timer to read the values from two sensors, so I can read instant values.
		//This was made to not use the delay() function because I can't halt the whole system
		currentMillis = millis();
		if (currentMillis - previousMillis >= interval)
		{
			previousMillis = currentMillis; //resets the timer
			clear();
			lcd.gotoXY(0, 0);
			lcd.print(left_value);
			lcd.gotoXY(0, 1);
			lcd.print(sensorFive);
		}

		
		if (left_value < 500 || right_value < 500) //if the robot sees lights, do stuff, otherwise spin
		{


			if (sensorOne >= 2000 || sensorTwo >= 2000 || sensorThree >= 2000 || sensorFour >= 2000 || sensorFive >= 2000)//if it detects the line, changes to LINE_func
			{
				set_motors(120, -120); //this rotates the robot, since it will be facing the wrong way when it switches mode
				delay(250);
				set_motors(0, 0);
				motionState = LINE_FOLLOW;
				return; //breaks out of the LDR_func to move to the LINE_func in the switch
			}
			if (left_value < right_value) //I could've done the whole differential calc stuff, but this is lighter and does the job just as well
				set_motors(70, 140);
			else if (left_value > right_value)
				set_motors(140, 70);
		}
		else
			set_motors(110, -110); //spin spin spin
		
	}

}
void LINE_func(void) {
	while (1) {
		for (x = 0; x != 1; x++)
		{

			bot.init(2000);
			// Auto-calibration: turn right and left while calibrating the
			// sensors.
			for (LINEcounter = 0; LINEcounter < 100; LINEcounter++)
			{
				if (LINEcounter < 25 || counter >= 75)
					set_motors(80, -80);
				else
					set_motors(-80, 80);

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
		}

		//defining and initialising ldrs for line follow to zero.
		while (1) {
			// Get the position of the line.  Note that we *must* provide
			// the "sensors" argument to read_line() here, even though we
			// are not interested in the individual sensor readings.
			sensorWithNoise = bot.readLine(sensors, IR_EMITTERS_ON);


			//filtering all of the 
			sensorOne = noiseFilter(sensors[0]);
			sensorTwo = noiseFilter(sensors[1]);
			sensorThree = noiseFilter(sensors[2]);
			sensorFour = noiseFilter(sensors[3]);
			sensorFive = noiseFilter(sensors[4]);

			TILTpin = analogRead(5); //reading the tilt sensor
			currentMillis = millis();
			if (currentMillis - previousMillis >= interval)
			{
				previousMillis = currentMillis; //resets the timer effectivly
				clear();
				lcd.gotoXY(0, 0);
				lcd.print(sensorOne);
				lcd.gotoXY(0, 1);
				lcd.print(sensorFive);
			}

			//if the robot doesn't detect the line on all sensors, enters search mode
			if ((sensorOne < 100) && (sensorTwo < 100) && (sensorThree < 100) && (sensorFour < 100) && (sensorFive < 100))
			{
				set_motors(120, -120);//turns the robot so that it can reverse onto the ramp, since it can only go up with the ball wheel at the front.
				delay(250);
				set_motors(-120, -120);
				delay(500);
				set_motors(-120, 120);
				delay(250);
				set_motors(0, 0);
				delay(100); //this allows the robot to come to a rest, so the tilt sensor doesn't get affected by the de-acceleration
				if (TILTpin > tiltFlat+2 || TILTpin < tiltFlat-2) //checking if the robot is on the seesaw
				{
					motionState = TILT_BALANCE;
					return;
				}
				else
					SEARCH_mode; //if its not on the seesaw, it must be lost, so search for the line again. In reality if it loses the line, its screwed.

			}

			position = noiseFilter(sensorWithNoise);//cleans the 0-4000 values for smoothness

													// The "centrePos" term should be 0 when we are on the line. Hece, that is the desired value
			int centrePos = position - 2000;

			// Compute the change in position and the current absolute position from when it started
			int delta = centrePos - lastCentrePos;
			integral += centrePos;

			// Remember the last position for comparasion later
			lastCentrePos = centrePos;

			//this equation will increase/decrease motor speed depending on the conditions above and constants
			int powerDiff = (centrePos / 20) + (integral / 10000) + (delta * 3 / 2);

			// Compute the actual motor settings. Never set either motor to a negative value
			const int maximum = 70; //this value changes the maximum difference the motors will have between them, affects sharpness of the turn
			if (powerDiff > maximum)
				powerDiff = maximum;
			else if (powerDiff < -maximum)
				powerDiff = -maximum;

			if (powerDiff < 0) //depending if the value is positive or negative, it needs to be sent to the right wheels.
				set_motors((maximum + powerDiff) * 3, maximum * 3); //I've set this to *3 because it easier than changing the powerDiff
			else
				set_motors(maximum * 3, (maximum - powerDiff) * 3);
		}
	}
}
void TILT_func(void) { //this doesn't work :/
	while (1) {
		sensorWithNoise = analogRead(5); //reads the raw tilt sensor value
		TILTposition = noiseFilter(sensorWithNoise); //cleans it

		TILTcentrePos = TILTposition - tiltFlat; //makes 0 the level/desired value
		//THE CODE BELOW IS A STOPGAP FOR A BETTER FUNCTION, BUT THIS BALANCING THINGS IS HARD
		//THE BIGGEST PROBLEMS IS THAT THE TILT SENSOR DETECTS THE ROBOTS ACCELERATION, AND THAT AT LOW RPM THE MOTOR IS TOO WEAK, SO NO DELICATE MOVEMENT

		currentMillis = millis();
		if (currentMillis - previousMillis >= interval)
		{
			previousMillis = currentMillis; //resets the timer effectivly
			clear();
			lcd.gotoXY(0, 0);
			lcd.print(speed);
			lcd.gotoXY(7, 0);
			lcd.print(TILTcentrePos);
			lcd.gotoXY(0, 1);
			lcd.print(TILTposition);
		}


		if (TILTcentrePos > 10)//The bigger the angle/delta the faster it changes motor speed
		{
			speed++;
			speed++;
			speed++;
		}
		else if (TILTcentrePos < 10 && TILTcentrePos > 5)
		{
			speed--;
			speed--;
		}
		else if (TILTcentrePos > -10 && TILTcentrePos < -5) //the order in this stack is not great, sorry
		{
			speed++;
			speed++;
		}
		else if (TILTcentrePos < 5)
		{
			speed--;
		}
		else if (TILTcentrePos > -5)
		{
			speed++;
		}
		else if (TILTcentrePos < -10)
		{
			speed--;
			speed--;
			speed--;
		}

		//makes sure that the robot doesn't go too fast
		if (speed > 30)
			speed = 30;
		else if (speed < -30)
			speed = -30;

		set_motors(speed, speed);

		delay(1); //added a small delay so the tilt sensor doesn't detect the robot's acceleration
	}
}
void SEARCH_mode(void) {
	while (1)
	{
		clear();
		lcd.gotoXY(0, 0);
		print("SEARCH");
		set_motors(50, 70);//this makes the robot spin in a large circle
		bot.readLineSensors(sensors, IR_EMITTERS_ON);
		if (sensors[0] > 1100 || sensors[1] > 1100 || sensors[2] > 1100 || sensors[3] > 1100 || sensors[4] > 1100)//This checks if any sensor is over a line, and if its is, resume
		{
			motionState = LINE_FOLLOW;
			return;
		}
	}

}
//outputs an int, takes in an int, takes median of input values
int noiseFilter(int number) //noise filter so that the robot doesn't go crazy because of noise
{
	
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
