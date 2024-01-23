/************* EEPROM MAP ***********
 *  0 - Reset counter (any reason)
 *  1 - WiFi connection counter
 *  2 - REL_1 last state
 *  3 - REL_1 last state (redundancy)
 *  4 - REL_2 last state
 *  5 - REL_2 last state (redundancy)
 *  6 - REL_3 last state
 *  7 - REL_3 last state (redundancy)
 ***********************************/
#define EEPROM_ADDR_MAX									4095
#define EEPROM_ADDR_RST_COUNTER                			0
#define EEPROM_ADDR_WIFI_CONN_COUNTER          			1
#define EEPROM_ADDR_REL_1_LAST_STATE           			2
#define EEPROM_ADDR_REL_1_LAST_STATE_INV       			3
#define EEPROM_ADDR_REL_2_LAST_STATE           			4
#define EEPROM_ADDR_REL_2_LAST_STATE_INV       			5
#define EEPROM_ADDR_REL_3_LAST_STATE           			6
#define EEPROM_ADDR_REL_3_LAST_STATE_INV       			7
#define EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS	   			8
#define EEPROM_ADDR_MENU_NB_SCH_DAILY					9
#define EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS			10
#define EEPROM_ADDR_MENU_NB_SCH_ONETIME					11
#define EEPROM_ADDR_MENU_IN_PROGRESS		   			12
#define EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED		13
#define EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_DAY	14
#define EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_HOUR	15
#define EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_MIN	16
#define EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_SEC	17

#define EEPROM_TOTAL_NB_OF_DEFINED_BYTES       			18

#define EEPROM_GET_REL_STATE_RETURN_ERROR				2