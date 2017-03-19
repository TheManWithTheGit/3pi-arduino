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

//variables for this func
int tiltFlat = 339;
int lastCentrePos = 0;
int centrePos = 0;
int integral = 0;
int position = 0;
int sensorWithNoise = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 1000;
int speed;

void setup() {
	lcd.gotoXY(0, 0);
	print("wait");
	wait_for_button_press(ANY_BUTTON);
	delay(1000);


	OrangutanBuzzer::playFrequency(1000, 500, 14);

	tiltFlat = analogRead(5);
}

void loop() {
	TILT_func();
}

void TILT_func(void) {
	delay(2000);

	set_motors(100, 100);
	delay(200);
	set_motors(0, 0);

	while (1) {
		sensorWithNoise = analogRead(5);
		position = noiseFilter(sensorWithNoise);

		centrePos = position - tiltFlat;
		if (centrePos > 0)
		{
			speed++;
			speed++;
			speed++;
		}
		else if (centrePos < 10)
		{
			speed--;
			
		}
		else if (centrePos < 0)
		{
			speed--;
			speed--;
			speed--;
		}

		if (speed > 30)
			speed = 30;
		else if (speed < -30)
			speed = -30;

		set_motors(speed, speed);

		delay(1);
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
