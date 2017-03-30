
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
const int intv = 333; //updates every 1/3 of a sec
unsigned int currentMillis = 0;

//variables for the LDR_func
const int left_ldr = 6;
const int right_ldr = 7;
int left_value = 0;
int right_value = 0;
int z = 0;
int y = 0;
unsigned int sensors[5];
unsigned int pos;
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
int TILTpos = 0;
int x = 0;
unsigned int sensorWithNoise = 0;
int millisTimer = 0;

//variables for the TILT_func
int tiltFlat = 0;
int pinFive = A5; //for reading the pin five for the tilt sensor
int TILTlastCentrePos;
//variables for the SEARCH_MODE
unsigned int sensorsSearch[5];

//variables for the noiseFilter
boolean swap = true; //setting up the switch statement for later
int temp, i, j, counter = 0; //now making my variables
int bufferOne[5] = {}; //this is where all the values go in
int bufferTwo[5] = {}; //this is for when I need to sort the buffer

					   //related to switching states for the course
int motionState;
#define LDR_FOLLOW 1 //could've used const, but this'll do
#define LINE_FOLLOW 2
#define TILT_BALANCE 3

//preface, yes I know that infinite loops are bad, but any other implementation is just more code for the same result, so while(1) will do


void setup() { //runs once

	bot.init(2000, 1); //This is required for the line follower sensors and timings
	pinMode(pinFive, INPUT);
	tiltFlat = analogRead(pinFive); //reads the tilt sensor and sets this as the desired value for later

	OrangutanBuzzer::playFrequency(2000, 500, 10); //beep
	clear();
	lcd.gotoXY(0, 0);
	print("SELECT");
	lcd.gotoXY(0, 1);
	print("MODE:");
}

void loop() { //runs forever

	unsigned char buttonPress = OrangutanPushbuttons::getSingleDebouncedPress(BUTTON_A | BUTTON_B | BUTTON_C);
	//I want to get a button press to select the mode I want to start with

	if (buttonPress & BUTTON_A) //Button A = LDR following mode and everything after
	{
		motionState = LDR_FOLLOW;
		clear();
		lcd.gotoXY(0, 0);
		print("LDR");
		OrangutanBuzzer::playFrequency(1000, 250, 14); //the buzz let me know its doing something
		buttonPress = 9; //This stops the if block from running infintly once it has run once 
		delay(200); //a slight delay so you can leat your finger escape
	}
	if (buttonPress & BUTTON_B) //Button B = line following mode and everything after
	{
		motionState = LINE_FOLLOW;
		clear();
		lcd.gotoXY(0, 0);
		print("LINE");
		OrangutanBuzzer::playFrequency(1000, 250, 14);
		buttonPress = 9;
		delay(200);
	}
	if (buttonPress & BUTTON_C) //Button C = seesaw balancing mode
	{
		motionState = TILT_BALANCE;
		clear();
		lcd.gotoXY(0, 0);
		print("TILT");
		OrangutanBuzzer::playFrequency(1000, 250, 14);
		buttonPress = 9;
		delay(200);
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
	//the calibration runs once, so it can detect the line
	//OrangutanBuzzer::playFrequency(2000, 10, 14);
	for (LDRcounter = 0; LDRcounter <= 40; LDRcounter++)
	{
		//turns a bit to one side, then turns all the way to the other side, then re-centeres
		//so it goes: turn, check, turn, check
		if (LDRcounter < 10 || LDRcounter >= 30)
			set_motors(40, -40);
		else
			set_motors(-40, 40);

		//this turns on an IR emitter, and reads the value that the arduino calculates from the wierd sensor
		bot.calibrateLineSensors(IR_EMITTERS_ON);
		//reading accurate valeus is not important for this part of the code, so I only get it to read a few
		delay(1);
	}
	//stops the robot, just incase
	set_motors(0, 0);

	//the calibration code above does get called again for re-calibrating the sensors, and I should've made the above code into a function.
	//but since the code is only called twice, its much easier to just copy-paste and change the values.

	//This loop keeps running until a condition is met and beaks out
	while (1) {


		//reads the values from the IR sensors, and cleans the signal
		bot.readLineSensors(sensors, IR_EMITTERS_ON);
		sensorOne = sensors[0];
		sensorTwo = sensors[1];
		sensorThree = sensors[2];
		sensorFour = sensors[3];
		sensorFive = sensors[4];

		//reads the LDR values
		left_value = analogRead(left_ldr);
		right_value = analogRead(right_ldr);

		//this is a timer to read the values from two sensors, so I can read instant values.
		//This was made to not use the delay() function because I don't want to halt the whole system
		currentMillis = millis();
		if (currentMillis - previousMillis >= intv)
		{
			previousMillis = currentMillis; //resets the timer
			clear();
			lcd.gotoXY(0, 7);
			lcd.print("1");//so I can tell which function its on
			lcd.gotoXY(0, 0);
			lcd.print(left_value);//left LDR
			lcd.gotoXY(0, 1);
			lcd.print(sensorFive); //rightmost IR sensor
		}


		if (left_value < 500 || right_value < 500) //if the robot sees lights, check other conditions, otherwise spin
		{


			if (sensorOne >= 2000 || sensorTwo >= 2000 || sensorThree >= 2000 || sensorFour >= 2000 || sensorFive >= 2000)//if it detects the line, changes to LINE_func
			{
				set_motors(20, 20);
				delay(200);
				set_motors(100, -100); //this rotates the robot, since it will be facing the wrong way when it switches mode
				delay(200);
				set_motors(0, 0);
				motionState = LINE_FOLLOW; //changes motionState for when it breaks out
				return; //breaks out of the LDR_func to move to the LINE_func in the switch
			}
			if (left_value < right_value) //I could've done the whole differential calc stuff, but this is lighter and does the job just as well
				set_motors(70, 130);
			else if (left_value > right_value)
				set_motors(130, 70);
		}
		else
			set_motors(80, -80); //spin spin spin

	}

}
void LINE_func(void) {

	//proper sensor calibration for the IR sensors
	for (LINEcounter = 0; LINEcounter <= 100; LINEcounter++)
	{
		if (LINEcounter < 25 || counter >= 75)
			set_motors(40, -40);
		else
			set_motors(-40, 40);

		bot.calibrateLineSensors(IR_EMITTERS_ON);

		delay(10);
	}
	set_motors(0, 0);



	while (1) {
		// Get the position (called pos here) of the line.
		//For this part, I want to read both the 0-4000 combined value, and the individual sensor values 
		sensorWithNoise = bot.readLine(sensors, IR_EMITTERS_ON);


		//pos = noiseFilter(sensorWithNoise); //For some reason, the noise filter breaks the readLine values, so I've left it out
		pos = sensorWithNoise;

		sensorOne = sensors[0];
		sensorTwo = sensors[1];
		sensorThree = sensors[2];
		sensorFour = sensors[3];
		sensorFive = sensors[4];

		TILTpin = analogRead(A5); //reading the tilt sensor
		currentMillis = millis();
		if (currentMillis - previousMillis >= intv)
		{
			previousMillis = currentMillis;
			clear();
			lcd.gotoXY(0, 7);
			lcd.print("2");
			lcd.gotoXY(0, 0);
			lcd.print(sensors[0]);//leftmost IR sensor
			lcd.gotoXY(0, 1);
			lcd.print(pos);//value readLine outputs
		}


		// The "centrePos" term should be 0 when we are on the line. Hence, we subtract 2000 from the sensor value
		int centrePos = pos - 2000;

		//The code below is very hacky because of a laste minute change that was made to the course that the robot will travel through.
		//Originally there was no line on the seesaw, so I would check if there was no line detected, and then check the tilt sensor,
		//but since there is a line now, the only way to check for the ramp is to use the tilt sensor. But this is not possible, due to the 
		//fact that the robot moving makes the tilt sensor produce a value much higher than the ramp would, so it would get tripped from just moving.
		//This means I cannot transition to the seesaw cleanly now, as I have no valid detection method.
		
		if(millis() >= 18000) //since literally nothing other than a timer is reliable, this is what I'm using.
				      //This timer runs from when the robot is turned on, so its not the most accurate, but the best we have so far
		{
			motionState = TILT_BALANCE;
			return;
		}



		// Compute the change in pos and the current absolute pos from when it started
		int delta = centrePos - lastCentrePos;
		integral += centrePos;

		// Remember the last pos for comparasion later
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
			set_motors((maximum + powerDiff) * 1.2, maximum * 1.2);
		else
			set_motors(maximum * 1.2, (maximum - powerDiff) * 1.2);

	}
}
void TILT_func(void) { //this doesn't really work :/
	while (1) {
		//reads the raw tilt sensor value
		TILTpos = noiseFilter(analogRead(pinFive));
			
		TILTcentrePos = TILTpos - tiltFlat; //makes 0 the level/desired value
		OrangutanBuzzer::playFrequency(1000, 250, 7);
		//THE BIGGEST PROBLEMS IS THAT THE TILT SENSOR DETECTS THE ROBOTS ACCELERATION, 
		//AND THAT AT LOW RPM THE MOTOR IS TOO WEAK, BUT WHEN ITS MOVES FASTER THE TILT SENSOR DETECTS THE MOVEMENT
		//THIS SHOULD BE CHANGED BEFORE THE ACTUAL TEST, BUT IF YOU'RE READING THIS, WELL, I TRIED...

		currentMillis = millis();
		if (currentMillis - previousMillis >= intv)
		{
			previousMillis = currentMillis; //resets the timer effectivly
			clear();
			lcd.gotoXY(0, 7);
			lcd.print("3");
			lcd.gotoXY(0, 0);
			lcd.print(analogRead(pinFive));//reads the raw tilt sensor value
			lcd.gotoXY(0, 1);
			lcd.print(TILTcentrePos);//reads the "centered" tilt sensor value
		}

		// The "TILTcentrePos" term should be 0 when we are on the line. Hence, we subtract the reference value from the readout
		// Compute the change in pos and the current absolute pos from when it started
		int deltaTILT = TILTcentrePos - TILTlastCentrePos;

		// Remember the last pos for comparasion later
		TILTlastCentrePos = TILTcentrePos;

		////this equation will increase/decrease motor speed depending on the conditions above and constants
		////this equation specifically makes it where it will move forward, 
		////and if the angle begins to change, it'll move in the opposite direction to counter the movement
		
		//int TILTpowerDiff = (TILTcentrePos * 2) - (deltaTILT*3/2);

		//// Compute the actual motor settings. Never set either motor to a negative value
		//const int TILTmaximum = 40; 

		//if (deltaTILT > 10) // I dont want it to change too quickly, otherwise it'll just get stuck in a feedback loop
		//	deltaTILT = 10;   //Although it seems to be super sensitive, so feedback is always happening.
		//else if (deltaTILT < -10)
		//	deltaTILT = -10;

		//if (TILTpowerDiff > TILTmaximum) //Don't want the robot to fly off the ramp
		//	TILTpowerDiff = TILTmaximum;

		//if (TILTpowerDiff < 10 && TILTpowerDiff > -10)//want it to react a bit faster a low speeds.
		//	TILTpowerDiff = TILTpowerDiff * 2;

		//int TILTpowerDiffClean = noiseFilter(TILTpowerDiff);

		//set_motors(TILTpowerDiffClean, TILTpowerDiffClean);

		
		//trying something easier due to feedback loops
		//I hate if else blocks with a passion, but its a more surefire way to test/debug

		if (TILTcentrePos >= 14) //this makes the robot inch forward so the ramp has time to react
		{
			set_motors(35, 35);
			delay(90);
			set_motors(0, 0);
			delay(600);
		}
		else if (TILTcentrePos <14 && TILTcentrePos > -14) //if the ramp is lifted up at all, stop
		{
			set_motors(0, 0);
			delay(500);
		}
		else if (TILTcentrePos <= -14)
		{
			set_motors(-35, -35);
			delay(90);
			set_motors(0, 0);
			delay(600);
		}
	


	}
}
void SEARCH_mode(void) { //if the robot gets lost, hopefully this will make it find the line again
			 //This is getting cut for now, as it causes more problems than it solves.
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
							//although it seems to be causing the values read from the IR sensors to break, and so the robot goes haywire. 
							//Oh the irony
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

