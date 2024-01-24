#include "helper_functions.h"
#include "specific_typedefs.h"
#include <NTPClient.h>

extern unsigned long localTimeMs;
extern unsigned long localTime;
extern unsigned long syncTime;
extern NTPClient timeClient;

bool syncWithNTP();
void getDateFromNTP(String &date);
void getDateFromNTPToStruct(clock_and_date_type &timestruct);
