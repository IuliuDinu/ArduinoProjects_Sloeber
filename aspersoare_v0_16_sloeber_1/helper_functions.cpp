#include "helper_functions.h"


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
