#include <OrangutanLCD.h>
#include <OrangutanAnalog.h>
#include <OrangutanMotors.h>
OrangutanLCD lcd;
OrangutanAnalog analog;

// variables for the LDR
int left_ldr = 6;
int right_ldr = 7;
int left_value = 0;
int right_value = 0;
//related to switching states for the course
int motionState;
#define LDR_FOLLOW 1;
#define LINE_FOLLOW 2;
#define TILT_BALANCE 3;


void setup() {
	motionState = LDR_FOLLOW;
}

void loop() {
	//This is the main tree where all the functions get called to do thier job
	switch (motionState)
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
	//todo, merged from other branches.
}
void LINE_func(void) {
	//todo, merged from other branches.
}
void TILT_func(void) {
	//todo, merged from other branches.
}