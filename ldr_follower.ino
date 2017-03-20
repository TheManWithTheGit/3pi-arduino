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

int left_ldr = 6;
int right_ldr = 7;
int left_value = 0;
int right_value = 0;
int z = 0;
unsigned int sensors[5];
unsigned int position;
unsigned int sensorOne;
unsigned int sensorTwo;
unsigned int sensorThree;
unsigned int sensorFour;
unsigned int sensorFive;

void setup() {
	analog.setMode(MODE_10_BIT);    // 10-bit analog-to-digital conversions
	lcd.gotoXY(0, 0);
	print("wait");
	wait_for_button_press(ANY_BUTTON);
	delay(1000);

	
}

void loop() {
	
	left_value = analogRead(left_ldr);
	right_value = analogRead(right_ldr);


	if (left_value < 600 || right_value < 600)
	{
		for (z = 0; z < 1; z++)
		{
			set_motors(150, -150);
			delay(50);
			set_motors(0, 0);
		}
		if (left_value < right_value)
			set_motors(60, 70);
		else if (left_value > right_value)
			set_motors(70, 60);
	}
	else
		set_motors(150, -150);



	//checks if the Line follow sensors detect a line.
	read_line_sensors(sensors, IR_EMITTERS_ON);
	sensorOne = noiseFilter(sensors[0]);
	sensorTwo = noiseFilter(sensors[1]);
	sensorThree = noiseFilter(sensors[2]);
	sensorFour = noiseFilter(sensors[3]);
	sensorFive = noiseFilter(sensors[4]);

	if (sensorOne > 900 || sensorTwo > 900 || sensorThree > 900 || sensorFour > 900 || sensorFive > 900)
	{
		//motionState = LINE_FOLLOW;
		set_motors(0, 0);
		break;
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
