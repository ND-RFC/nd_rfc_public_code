/* Tackle_Sensor.h
*  --------------------------------
*  This includes the abstract class for a sensor.  Usually a sensor has some sort of read function,
*  and at least 1 pin number attached to it
*  --------------------------------
*  Author: Joe Rudy
*  Date: 3/13/2013
*
*/
#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

namespace ND
{
	class Sensor
	{
		public:
			virtual int Read()= 0;// reads the sensor and saves the value
			int getValue(int index) 
			{ 
				if (index < 3 && index >= 0) return val[index]; 
				else return 0;
			}
		protected:
			int val[3]; // this gives the previous 4 values that the sensor has read
	};
	/*	Title	:	Tackle_Sensor
	*	Purpose	:	incorporates the Tackle sensor into a class
	*				tackled only occurs at the transition between a 1 and a 0 and an untackling event
	*				occurs at a transition between 0 and 1
	*/
	class Tackle_Sensor : public Sensor
	{
		public:
			Tackle_Sensor(int pinnum);
			int Read();
			bool tackled();
			bool just_tackled(); // changes to false if just_tackled is read
			bool just_untackled(); // changes to false if just_untackled is read
		protected:
			int pin; // this gives the pin number
			bool tackld;
			bool just_tackld;
			bool just_untackld;
			void update(); // updates the tackld state to determine if you were tackled or not
	
	};
};

#endif
