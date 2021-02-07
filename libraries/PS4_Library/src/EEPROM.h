/*
  EEPROM.h - EEPROM library
  Copyright (c) 2006 David A. Mellis.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/* Modified by Notre Dame for our purposes
* -----------------------------------------
* Include #defines for memory addresses of what we will be needing so that you
* should use these everytime you need some of that memory
* 
*/
#ifndef EEPROM_h
#define EEPROM_h

#define ADDR_FORWARD_OFFSET		1	// used for regular drive wheels and mecanum wheels
#define ADDR_REVERSE_OFFSET		2	// used for regular drive wheels and mecanum wheels
#define ADDR_FORWARD_OFFSET2	3	// used for mecanum wheels
#define ADDR_REVERSE_OFFSET2	4	// used for mecanum wheels
#define ADDR_FORWARD_OFFSET3	5	// used for a third motor
#define ADDR_REVERSE_OFFSET3	6 	// used for a third motor
#define ADDR_THROWING_OFFSET	7	// used to offset throwing wheels
#define ADDR_DIRECTION_OFFSET	8	// used to determine which motor is actually being offset
					// there are 4 directions stored in this variable (2 bits per direction

#define ADDR_LPWM1_1650			9   // this is time for the rotation of the left throwing wheel at PWM speed 1650 (bit1)
// addresses 10-16 will be reserved for increments of 1650 by 50’s up to 2000 (bit1)
#define ADDR_LPWM2_1650			17  // this is time for the rotation of the left throwing wheel at PWM speed 1650 (bit2)
// addresses 18-24 will be reserved for increments of 1650 by 50’s up to 2000 (bit2)
#define ADDR_RPWM1_1650			25   // this is time for the rotation of the right throwing wheel at PWM speed 1650 (bit1)
// addresses 26-32 will be reserved for increments of 1650 by 50’s up to 2000 (bit1)
#define ADDR_RPWM2_1650			33  // this is time for the rotation of the right throwing wheel at PWM speed 1650 (bit2)
// addresses 34-40 will be reserved for increments of 1650 by 50’s up to 2000 (bit2)

#define ADDR_LAST_ADDRESS		40  // change this everytime you change the last address in any board ... used in setup
// End of Modification by Notre Dame
#include <inttypes.h>

class EEPROMClass
{
  public:
    uint8_t read(int);
    void write(int, uint8_t);
};

extern EEPROMClass EEPROM;

#endif

