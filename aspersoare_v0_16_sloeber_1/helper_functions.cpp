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

void resetAllLoads()
{
	rel1_status = LOW;
	rel2_status = LOW;
	rel3_status = LOW;
	digitalWrite(REL_1, rel1_status);
	digitalWrite(REL_2, rel2_status);
	digitalWrite(REL_3, rel3_status);
}

void convertFromSecToStructHMS(unsigned long ul_sec, clock_type *hms_var)
{
  hms_var->h = 0;
  hms_var->m = 0;
  hms_var->s = 0;

  if (ul_sec > 3600)
  {
    hms_var->h = ul_sec/3600;

    if (hms_var->h >= 24)
    {
      hms_var->h -=24;
    }

    ul_sec -= (hms_var->h*3600);
  }

  if (ul_sec > 60)
  {
    hms_var->m = ul_sec/60;
    ul_sec -= (hms_var->m*60);
  }

  hms_var->s = ul_sec;
}

void get24HMaxMillis()
{
  gul_max24hMillis = millis();
  if (gul_max24hMillis > ONE_DAY_IN_MILISECONDS)
  {
    unsigned long multiplier = gul_max24hMillis/ONE_DAY_IN_MILISECONDS;
    gul_max24hMillis -= (multiplier*ONE_DAY_IN_MILISECONDS);
  }
}
