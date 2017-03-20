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
int x = 0;
unsigned int sensorWithNoise = 0;
unsigned int position;
unsigned int sensorOne;
unsigned int sensorTwo;
unsigned int sensorThree;
unsigned int sensorFour;
unsigned int sensorFive;

void setup() {
	lcd.gotoXY(0, 0);
	print("wait");
	wait_for_button_press(ANY_BUTTON);
	delay(1000);
	unsigned int counter; // used as a simple timer

						  // This must be called at the beginning of 3pi code, to set up the
						  // sensors.  We use a value of 2000 for the timeout, which
						  // corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	//for (x = 0; x < 1; x++)
	//{
	//	bot.init(2000);
	//	// Auto-calibration: turn right and left while calibrating the
	//	// sensors.
	//	for (counter = 0; counter < 160; counter++)
	//	{
	//		if (counter < 40 || counter >= 120)
	//			set_motors(70, -70);
	//		else
	//			set_motors(-70, 70);

	//		// This function records a set of sensor readings and keeps
	//		// track of the minimum and maximum values encountered.  The
	//		// IR_EMITTERS_ON argument means that the IR LEDs will be
	//		// turned on during the reading, which is usually what you
	//		// want.
	//		bot.calibrateLineSensors(IR_EMITTERS_ON);

	//		// Since our counter runs to 80, the total delay will be
	//		// 80*20 = 1600 ms.
	//		delay(1);
	//	}
	//	set_motors(0, 0);
	//}
	OrangutanBuzzer::playFrequency(1000, 500, 14);

}

void loop() {
	LINE_func();
}

void LINE_func(void) {

	//need to re-run the calibration on the line in order to see it.
	for (x = 0; x != 1; x++)
	{
		//turns the robot 180 because when it starts the line func, it's facing the wrong way.
		set_motors(120, -120);
		delay(250);
		set_motors(0, 0);

		bot.init(2000);
		// Auto-calibration: turn right and left while calibrating the
		// sensors.
		for (counter = 0; counter < 100; counter++)
		{
			if (counter < 25 || counter >= 75)
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
			if (TILTpin > 342) //checking if the robot is on the seesaw
			{
				//motionState = TILT_BALANCE;
				//break;
				set_motors(0, 0);
				OrangutanBuzzer::playFrequency(3000, 500, 14);
				delay(100000);
			}
			else
			{
				set_motors(0, 0);
				OrangutanBuzzer::playFrequency(1000, 500, 14);
				OrangutanBuzzer::playFrequency(2000, 500, 14);
			}
			//SEARCH_mode; //if its not on the seesaw, it must be lost, so search for the line again. In reality if it loses the line, its screwed.
			
		}
		
		position = noiseFilter(sensorWithNoise);

		// The "centrePos" term should be 0 when we are on the line. Hece, that is the desired value
		int centrePos = position - 2000;

		// Compute the change in position and the current absolute position
		int delta = centrePos - lastCentrePos;
		integral += centrePos; //this value XXXXXXXXXXXX

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
			set_motors((maximum + powerDiff) * 3, maximum * 3);
		else
			set_motors(maximum * 3, (maximum - powerDiff) * 3);
	}
}
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

	if (counter < 5) //trigger when the buffer is full
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
				if (bufferTwo[i] > bufferTwo[i + 1])
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
