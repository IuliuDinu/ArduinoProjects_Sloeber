#include "EEPROM_functions.h"

byte eepromInitParticularByte(byte address)
{
	byte status = 0;
	EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //N bytes used
	delay(100);
	EEPROM.write(address, 0);
	delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte eepromInitParticularBytesFromTo(byte address_from, byte address_to)
{
	byte i;
	byte status = 0;
	EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //N bytes used
	delay(100);
	for (i = address_from; i <= address_to; i++)
	{
		EEPROM.write(i, 0);
	}
	delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

void eepromEraseAllDefinedBytes()
{
	int i;
	EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //N bytes used
	delay(100);
	for (i=0; i<EEPROM_TOTAL_NB_OF_DEFINED_BYTES; i++)
	{
		EEPROM.write(i, 0);
		delay(100);
	}
    EEPROM.commit();
    delay(100);
}

void eepromEraseAddr01()
{
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
    delay(100);
    EEPROM.write(0, 0);
    delay(100);
    EEPROM.write(1, 0);
    delay(100);
    EEPROM.commit();
    delay(100);
}

void eepromEraseRelLastState()
{
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //6 bytes used now
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_1_LAST_STATE, 0);
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_1_LAST_STATE_INV, 1);
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_2_LAST_STATE, 0);
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_2_LAST_STATE_INV, 1);
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_3_LAST_STATE, 0);
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_3_LAST_STATE_INV, 1);
  delay(100);
  EEPROM.commit();
  delay(100);
}

byte setEepromRel_1(byte state)
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_1_LAST_STATE, state);
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_1_LAST_STATE_INV, !(state));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEepromRel_2(byte state)
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_2_LAST_STATE, state);
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_2_LAST_STATE_INV, !(state));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEepromRel_3(byte state)
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_3_LAST_STATE, state);
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_3_LAST_STATE_INV, !(state));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_timerScheduledDaily(bool state) // save info in EEPROM: is daily timer scheduled or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
    if (state == TRUE) 	// set bit 0
    {
    	temp = temp | (0b00000001);
    }
    else				// reset bit 0
    {
    	temp = temp & (0b11111110);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledDaily_0(bool state) // save info in EEPROM: is load 0 scheduled for daily run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
    if (state == TRUE) 	// set bit 1
    {
    	temp = temp | (0b00000010);
    }
    else				// reset bit 1
    {
    	temp = temp & (0b11111101);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledDaily_1(bool state) // save info in EEPROM: is load 1 scheduled for daily run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
    if (state == TRUE) 	// set bit 2
    {
    	temp = temp | (0b00000100);
    }
    else				// reset bit 2
    {
    	temp = temp & (0b11111011);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledDaily_2(bool state) // save info in EEPROM: is load 2 scheduled for daily run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
    if (state == TRUE) 	// set bit 3
    {
    	temp = temp | (0b00001000);
    }
    else				// reset bit 3
    {
    	temp = temp & (0b11110111);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_menuNumberScheduledDaily(byte val) // save info in EEPROM: the number of daily-type scheduled menu
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_MENU_NB_SCH_DAILY, val);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_timerScheduledOneTime(bool state) // save info in EEPROM: is 1-time timer scheduled or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
    if (state == TRUE) 	// set bit 0
    {
    	temp = temp | (0b00000001);
    }
    else				// reset bit 0
    {
    	temp = temp & (0b11111110);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledOneTime_0(bool state) // save info in EEPROM: is load 0 scheduled for 1-time run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
    if (state == TRUE) 	// set bit 1
    {
    	temp = temp | (0b00000010);
    }
    else				// reset bit 1
    {
    	temp = temp & (0b11111101);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledOneTime_1(bool state) // save info in EEPROM: is load 1 scheduled for 1-time run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
    if (state == TRUE) 	// set bit 2
    {
    	temp = temp | (0b00000100);
    }
    else				// reset bit 2
    {
    	temp = temp & (0b11111011);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledOneTime_2(bool state) // save info in EEPROM: is load 2 scheduled for 1-time run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
    if (state == TRUE) 	// set bit 3
    {
    	temp = temp | (0b00001000);
    }
    else				// reset bit 3
    {
    	temp = temp & (0b11110111);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_menuNumberScheduledOneTime(byte val) // save info in EEPROM: the number of 1-time-type scheduled menu
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_MENU_NB_SCH_ONETIME, val);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_menuInProgress(byte val) // save info in EEPROM: the menu which is in progress (currently running)
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_MENU_IN_PROGRESS, val);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_lastMenuSuccessfullyEnded(byte val) // save info in EEPROM: the menu which successfully ended last time
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED, val);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_lastMenuSuccessfullyEnded_Day(byte val) // save info in EEPROM: the day on which the menu successfully ended last time
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_DAY, byte(val));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_lastMenuSuccessfullyEnded_Hour(byte val) // save info in EEPROM: the hour at which the menu successfully ended last time
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_HOUR, byte(val));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_lastMenuSuccessfullyEnded_Min(byte val) // save info in EEPROM: the minute at which the menu successfully ended last time
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_MIN, byte(val));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_lastMenuSuccessfullyEnded_Sec(byte val) // save info in EEPROM: the second at which the menu successfully ended last time
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_SEC, byte(val));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

void setEeprom_allParametersForScheduledOneTime()
{
	setEeprom_timerScheduledOneTime(timerScheduledOneTime);
	setEeprom_loadsScheduledOneTime_0(loadsScheduledOneTime[0]);
	setEeprom_loadsScheduledOneTime_1(loadsScheduledOneTime[1]);
	setEeprom_loadsScheduledOneTime_2(loadsScheduledOneTime[2]);
	setEeprom_menuNumberScheduledOneTime(menuNumberScheduledOneTime);
}

void setEeprom_allParametersForScheduledDaily()
{
	setEeprom_timerScheduledDaily(timerScheduledDaily);
	setEeprom_loadsScheduledDaily_0(loadsScheduledDaily[0]);
	setEeprom_loadsScheduledDaily_1(loadsScheduledDaily[1]);
	setEeprom_loadsScheduledDaily_2(loadsScheduledDaily[2]);
	setEeprom_menuNumberScheduledDaily(menuNumberScheduledDaily);
}

byte getEepromRel_1()
{
  byte state = 0, state_inv = 1;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
  state = EEPROM.read(EEPROM_ADDR_REL_1_LAST_STATE);
  delay(100);
  state_inv = EEPROM.read(EEPROM_ADDR_REL_1_LAST_STATE_INV);
  delay(100);
  Serial.print("EEPROM_ADDR_REL_1_LAST_STATE: ");
  Serial.println(state);
  Serial.print("EEPROM_ADDR_REL_1_LAST_STATE_INV: ");
  Serial.println(state_inv);
  if (state == !(state_inv))
    return state;
  else
    return EEPROM_GET_REL_STATE_RETURN_ERROR;
}

byte getEepromRel_2()
{
  byte state, state_inv;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
  state = EEPROM.read(EEPROM_ADDR_REL_2_LAST_STATE);
  delay(100);
  state_inv = EEPROM.read(EEPROM_ADDR_REL_2_LAST_STATE_INV);
  delay(100);
  Serial.print("EEPROM_ADDR_REL_2_LAST_STATE: ");
  Serial.println(state);
  Serial.print("EEPROM_ADDR_REL_2_LAST_STATE_INV: ");
  Serial.println(state_inv);
  if (state == !(state_inv))
    return state;
  else
    return EEPROM_GET_REL_STATE_RETURN_ERROR;
}

byte getEepromRel_3()
{
  byte state, state_inv;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
  state = EEPROM.read(EEPROM_ADDR_REL_3_LAST_STATE);
  delay(100);
  state_inv = EEPROM.read(EEPROM_ADDR_REL_3_LAST_STATE_INV);
  delay(100);
  Serial.print("EEPROM_ADDR_REL_3_LAST_STATE: ");
  Serial.println(state);
  Serial.print("EEPROM_ADDR_REL_3_LAST_STATE_INV: ");
  Serial.println(state_inv);
  if (state == !(state_inv))
    return state;
  else
    return EEPROM_GET_REL_STATE_RETURN_ERROR;
}

bool getEeprom_timerScheduledDaily()
{
  bool state;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS: ");
  Serial.println(temp);
  state = (bool)(temp & 0b00000001);
  return state;
}

bool getEeprom_loadsScheduledDaily_0()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00000010)>>1);
  return val;
}

bool getEeprom_loadsScheduledDaily_1()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00000100)>>2);
  return val;
}

bool getEeprom_loadsScheduledDaily_2()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00001000)>>3);
  return val;
}

byte getEeprom_menuNumberScheduledDaily()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_MENU_NB_SCH_DAILY);
  delay(100);
  Serial.print("EEPROM_ADDR_MENU_NB_SCH_DAILY: ");
  Serial.println(val);
  return val;
}

bool getEeprom_timerScheduledOneTime()
{
  bool state;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS: ");
  Serial.println(temp);
  state = (bool)(temp & 0b00000001);
  return state;
}

bool getEeprom_loadsScheduledOneTime_0()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00000010)>>1);
  return val;
}

bool getEeprom_loadsScheduledOneTime_1()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00000100)>>2);
  return val;
}

bool getEeprom_loadsScheduledOneTime_2()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00001000)>>3);
  return val;
}

byte getEeprom_menuNumberScheduledOneTime()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_MENU_NB_SCH_ONETIME);
  delay(100);
  Serial.print("EEPROM_ADDR_MENU_NB_SCH_ONETIME: ");
  Serial.println(val);
  return val;
}

byte getEeprom_menuInProgress()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_MENU_IN_PROGRESS);
  delay(100);
  Serial.print("EEPROM_ADDR_MENU_IN_PROGRESS: ");
  Serial.println(val);
  return val;
}

byte getEeprom_lastMenuSuccessfullyEnded()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED);
  delay(100);
  Serial.print("EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED: ");
  Serial.println(val);
  return val;
}

byte getEeprom_lastMenuSuccessfullyEnded_Day()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_DAY);
  delay(100);
  Serial.print("EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_DAY: ");
  Serial.println(val);
  return val;
}

byte getEeprom_lastMenuSuccessfullyEnded_Hour()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_HOUR);
  delay(100);
  Serial.print("EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_HOUR: ");
  Serial.println(val);
  return val;
}

byte getEeprom_lastMenuSuccessfullyEnded_Min()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_MIN);
  delay(100);
  Serial.print("EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_MIN: ");
  Serial.println(val);
  return val;
}

byte getEeprom_lastMenuSuccessfullyEnded_Sec()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_SEC);
  delay(100);
  Serial.print("EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_SEC: ");
  Serial.println(val);
  return val;
}

void eepromPrintAllVariables(WiFiClient client)
{
	client.println("*** Listing all EEPROM values **");
	client.print("EEPROM value for REL_1: ");
	client.println(getEepromRel_1());
	client.print("EEPROM value for REL_2: ");
	client.println(getEepromRel_2());
	client.print("EEPROM value for REL_3: ");
	client.println(getEepromRel_3());
	client.print("timerScheduledDaily: ");
	client.println(getEeprom_timerScheduledDaily());
	client.print("loadsScheduledDaily[0]: ");
	client.println(getEeprom_loadsScheduledDaily_0());
	client.print("loadsScheduledDaily[1]: ");
	client.println(getEeprom_loadsScheduledDaily_1());
	client.print("loadsScheduledDaily[2]: ");
	client.println(getEeprom_loadsScheduledDaily_2());
	client.print("menuNumberScheduledDaily: ");
	client.println(getEeprom_menuNumberScheduledDaily());
	client.print("timerScheduledOneTime: ");
	client.println(getEeprom_timerScheduledOneTime());
	client.print("loadsScheduledOneTime[0]: ");
	client.println(getEeprom_loadsScheduledOneTime_0());
	client.print("loadsScheduledOneTime[1]: ");
	client.println(getEeprom_loadsScheduledOneTime_1());
	client.print("loadsScheduledOneTime[2]: ");
	client.println(getEeprom_loadsScheduledOneTime_2());
	client.print("menuNumberScheduledOneTime: ");
	client.println(getEeprom_menuNumberScheduledOneTime());
	client.print("menuInProgress: ");
	client.println(getEeprom_menuInProgress());
	client.print("lastMenuSuccessfullyEnded: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded());
#ifndef ESPBOX1
	client.print("lastMenuSuccessfullyEnded_Day: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Day());
	client.print("lastMenuSuccessfullyEnded_Hour: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Hour());
	client.print("lastMenuSuccessfullyEnded_Minute: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Min());
	client.print("lastMenuSuccessfullyEnded_Second: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Sec());
#endif
#ifdef ESPBOX1
	client.print("lastTimerAction_Day: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Day());
	client.print("lastTimerAction_Hour: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Hour());
	client.print("lastTimerAction_Minute: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Min());
	client.print("lastTimerAction_Second: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Sec());
#endif
	client.println("*** ************** **");
}

void loadTimersDataFromEEPROM() // load saved timers configuration (of all types) from EEPROM
{
	timerScheduledDaily = getEeprom_timerScheduledDaily();
	loadsScheduledDaily[0] = getEeprom_loadsScheduledDaily_0();
	loadsScheduledDaily[1] = getEeprom_loadsScheduledDaily_1();
	loadsScheduledDaily[2] = getEeprom_loadsScheduledDaily_2();
	menuNumberScheduledDaily = getEeprom_menuNumberScheduledDaily();
	timerScheduledOneTime = getEeprom_timerScheduledOneTime();
	loadsScheduledOneTime[0] = getEeprom_loadsScheduledOneTime_0();
	loadsScheduledOneTime[1] = getEeprom_loadsScheduledOneTime_1();
	loadsScheduledOneTime[2] = getEeprom_loadsScheduledOneTime_2();
	menuNumberScheduledOneTime = getEeprom_menuNumberScheduledOneTime();
	menuInProgress = getEeprom_menuInProgress();
	lastMenuSuccessfullyEnded = getEeprom_lastMenuSuccessfullyEnded();
}

void saveLastMenuSuccessfullyEnded_Parameters()
{
	setEeprom_lastMenuSuccessfullyEnded_Day(byte(gs_last_successful_menu_run.d));
	setEeprom_lastMenuSuccessfullyEnded_Hour(byte(gs_last_successful_menu_run.h));
	setEeprom_lastMenuSuccessfullyEnded_Min(byte(gs_last_successful_menu_run.m));
	setEeprom_lastMenuSuccessfullyEnded_Sec(byte(gs_last_successful_menu_run.s));
}

void updateWifiConnectionCounter()
{
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used
    delay(100);
    nbOfWifiConnectedTimes = EEPROM.read(EEPROM_ADDR_WIFI_CONN_COUNTER);
    nbOfWifiConnectedTimes++;
    delay(100);
    EEPROM.write(EEPROM_ADDR_WIFI_CONN_COUNTER, nbOfWifiConnectedTimes);
    delay(100);
    if (EEPROM.commit())
    {
      Serial.println("EEPROM has been updated.");
    }
    else
    {
      Serial.println("EEPROM commit failed.");
    }
}
