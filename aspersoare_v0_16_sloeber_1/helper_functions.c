#include "helper_functions.h"
//#include "Arduino.h"
//#include "aspersoare_v0_16_sloeber_1.ino"

//extern unsigned int REL_1;
//extern unsigned int REL_2;
//extern unsigned int REL_3;

void blinkAllLeds(byte nbOfTimes, byte period)
{
	for (int i=0; i<nbOfTimes; i++)
	{
		digitalWrite(REL_1, HIGH);
		delay(period/2);
		digitalWrite(REL_1, LOW);
		delay(period/2);
		digitalWrite(REL_2, HIGH);
		delay(period/2);
		digitalWrite(REL_2, LOW);
		delay(period/2);
		digitalWrite(REL_3, HIGH);
		delay(period/2);
		digitalWrite(REL_3, LOW);
		delay(period/2);
	}
}
