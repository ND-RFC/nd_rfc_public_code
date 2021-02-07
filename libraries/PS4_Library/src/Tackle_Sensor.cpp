/* Tackle_Sensor.cpp
* ---------------------------------------
* The header file is found under Sensor.h
* ---------------------------------------
*  Author : Joe Rudy
*  Date	:	3/14/2013
*/
#include "Sensor.h"
using namespace ND;

#define TACKLED 0
#define NOT_TACKLED 1

Tackle_Sensor::Tackle_Sensor(int pinnum)
{
	pin = pinnum;
	pinMode(pin,INPUT_PULLUP); // Mark the Tackle sensor as an input and you need to pull it up
	// because the tackle sensor works on 3.3V logic so pull it up to 5V which is Arduino logic
	for (int i = 0; i < 3; i++) val[i] = TACKLED; // set to 0
	tackld = false;
	just_tackld = false;
	just_untackld = false;
}

int Tackle_Sensor::Read()
{
	val[2] = val[1]; // save the previous values
	val[1] = val[0]; // save the previous values
	val[0] = digitalRead(pin); // read a new value in
	update(); // update the status of your robot (tackled or not)
	return val[0];
}

void Tackle_Sensor::update()
{
	if (val[0] == TACKLED || val[1] == TACKLED || val[2] == TACKLED) // pin got pulled low
	{
		if (val[0] == NOT_TACKLED) { // if the current value is high you are not tackled anymore
			just_tackld = false;
			tackld = false;
			just_untackld = true;
		} else if (val[2] == NOT_TACKLED || val[1] == NOT_TACKLED) { // if any of the previous values are high, you are still tackled
			tackld = true;
			just_tackld = true;
			just_untackld = false;
		}
	}
}
/*
* just_tackld becomes false when you read from it
*/
bool Tackle_Sensor::just_tackled()
{
	bool temp;
	temp = just_tackld; just_tackld = false;
	return temp;
}

bool Tackle_Sensor::just_untackled()
{
	bool temp = just_untackld;
	just_untackld = false;
	return temp;
}
bool Tackle_Sensor::tackled()
{
	return tackld;
}