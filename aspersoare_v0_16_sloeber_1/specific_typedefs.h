#pragma once

typedef struct
{
  unsigned int h;
  unsigned int m;
  unsigned int s;
} clock_type;

typedef struct
{
	unsigned int h;
	unsigned int m;
	unsigned int s;
	unsigned int d;
	unsigned int mo;
	unsigned int y;
	unsigned int wd;
} clock_and_date_type;

typedef struct
{
  byte nbMaxPrograms;
  byte timeSetting[5][4];
  byte daysOfWeek;
} zoneSettings;
