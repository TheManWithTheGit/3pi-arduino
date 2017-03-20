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
int counter;

void setup() {
	analog.setMode(MODE_10_BIT);    // 10-bit analog-to-digital conversions
	lcd.gotoXY(0, 0);
	print("wait");
	wait_for_button_press(ANY_BUTTON);
	delay(500);

	bot.init(2000);


	for (counter = 0; counter<40; counter++)
	{
		if (counter < 10 || counter >= 30)
			set_motors(40, -40);
		else
			set_motors(-40, 40);

		// This function records a set of sensor readings and keeps
		// track of the minimum and maximum values encountered.  The
		// IR_EMITTERS_ON argument means that the IR LEDs will be
		// turned on during the reading, which is usually what you
		// want.
		bot.calibrateLineSensors(IR_EMITTERS_ON);

		// Since our counter runs to 80, the total delay will be
		// 80*20 = 1600 ms.
		delay(2);
	}
	set_motors(0, 0);

	OrangutanBuzzer::playFrequency(1000, 500, 14);



}

void loop() {
	//checks if the Line follow sensors detect a line.
	read_line_sensors(sensors, IR_EMITTERS_ON);
	sensorOne = noiseFilter(sensors[0]);
	sensorTwo = noiseFilter(sensors[1]);
	sensorThree = noiseFilter(sensors[2]);
	sensorFour = noiseFilter(sensors[3]);
	sensorFive = noiseFilter(sensors[4]);

	/*clear();
	lcd.gotoXY(0,0);
	lcd.print(sensorOne);
	lcd.gotoXY(0,1);
	lcd.print(sensorFive);
	delay(200);
	*/


	left_value = analogRead(left_ldr);
	right_value = analogRead(right_ldr);

	if (left_value < 500 || right_value < 500)
	{

		
		if (sensorOne > 2000 || sensorTwo > 2000 || sensorThree > 2000 || sensorFour > 2000 || sensorFive > 2000)
		{
			//motionState = LINE_FOLLOW;
			set_motors(0, 0);
			delay(10000);
			//break;
		}
		if (left_value < right_value)
			set_motors(70, 140);
		else if (left_value > right_value)
			set_motors(140, 70);
	}
	else
		set_motors(110, -110);

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
