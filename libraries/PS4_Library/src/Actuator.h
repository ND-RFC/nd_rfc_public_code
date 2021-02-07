/* Acuator.h
*----------------------------------------------
* This contains the abstract class for an actuator.  This can be used to inherit for any actuator
* including but not exclusive to motors, servos, and mecanum wheels.  This can also be used in
* a control system class to utilize all of these actuation devices.
* ---------------------------------------------------------------------------------
* Author: Joe Rudy (jrudy1@nd.edu)
* Date:	3/12/2013
*
*/
#ifndef ACTUATOR_H
#define ACTUATOR_H
namespace ND
{
	class Actuator
	{
		public:
			virtual void Move(int variable) = 0; // if you just want to move the actuation to a specific speed or position
			virtual void Stop() = 0; // stops the actuation completely at whatever zero point
			virtual void Calibrate(int val, int what) = 0; // calibration is always necessary
				// for example, the motors that drive the 2 wheels on your robot will probably not spin
				// at exactly the same speeds even though you place the same values in the write function
				// ... do not fret, this is what calibration is for
	};
};
#endif