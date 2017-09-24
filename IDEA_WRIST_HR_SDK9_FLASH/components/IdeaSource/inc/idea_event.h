#ifndef __IDEA_EVENT_H
#define __IDEA_EVENT_H

#include <stdint.h>

// 0: ota, photo, incoming call, button, charging, alarming, long-sit, achieve goal
// 1: end event
#define EVENT_LEVEL_0   0
#define EVENT_LEVEL_1   1
#define EVENT_LEVEL_2   2

#define EVENT_ID_NONE    0
#define EVENT_TYPE_NONE  0
#define EVENT_TYPE_INTERRUPT 255

#define EVENT_ID_BUTTON   0
#define EVENT_ID_BUTTON0  1
#define EVENT_ID_BUTTON1  2
#define EVENT_ID_BUTTON2  3
#define EVENT_ID_BUTTON3  4
#define EVENT_ID_BUTTON4  5
#define EVENT_ID_BUTTON5  6
#define EVENT_ID_BUTTON6  7
#define EVENT_ID_BUTTON7  8
#define EVENT_ID_BUTTON8  9
#define EVENT_ID_BUTTON9  10
#define EVENT_TYPE_BUTTON_CLICK 1
#define EVENT_TYPE_BUTTON_PUSHED_LONG 2
#define EVENT_TYPE_BUTTON_CLICK_DOUBLE 3
#define EVENT_TYPE_BUTTON_PUSHED_LONG_LONG 4


#define EVENT_ID_TAP    15
#define EVENT_TYPE_TAP_SINGLE 1
#define EVENT_TYPE_TAP_DOUBLE 2

#define EVENT_ID_LONG_SIT  16
#define EVENT_TYPE_LONG_SIT_START 1
#define EVENT_TYPE_LONG_SIT_STOP 2

#define EVENT_ID_ANTI_LOST_ALERT            17
#define EVENT_TYPE_ANTI_LOST_ALERT_START    1
#define EVENT_TYPE_ANTI_LOST_ALERT_STOP     2

#define EVENT_ID_BATTERY  20
#define EVENT_TYPE_BATTERY_CHAGING 1
#define EVENT_TYPE_BATTERY_CHAGING_FULL 2
#define EVENT_TYPE_BATTERY_NOT_CHAGING  3

#define EVENT_ID_CALL     21
#define EVENT_TYPE_CALL_START 1
#define EVENT_TYPE_CALL_START_WITH_CONTENT 2
#define EVENT_TYPE_CALL_STOP 5

#define EVENT_ID_ALARM              25
#define EVENT_TYPE_ALARM_STARTED    1
#define EVENT_TYPE_ALARM_STOPPED    2
#define EVENT_TYPE_ALARM_ADD        3
#define EVENT_TYPE_ALARM_DELETE     4
#define EVENT_TYPE_ALARM_MODIFY     5


#define EVENT_ID_OTA                26
#define EVENT_TYPE_OTA_START        1

#define EVENT_ID_PAIR               27
#define EVENT_TYPE_PAIR_START       1
#define EVENT_TYPE_PAIR_OK          2
#define EVENT_TYPE_PAIR_TIMEOUT     3
#define EVENT_TYPE_PAIR_FAIL        4
#define EVENT_TYPE_PAIR_UNPAIR      5


#define EVENT_ID_SLEEP_MONITOR              28
#define EVENT_TYPE_SLEEP_MONITOR_STOP       1

#define EVENT_ID_BLE                        29
#define EVENT_TYPE_BLE_CONNECT              1
#define EVENT_TYPE_BLE_DISCONNECT           2

#define EVENT_ID_REBOOT 					30

#define EVENT_ID_GOAL     40
#define EVENT_TYPE_ACHIEVE_STEP_GOAL_START 1
#define EVENT_TYPE_ACHIEVE_STEP_GOAL_STOP 1

#define EVENT_ID_PHOTO              50
#define EVENT_TYPE_PHOTO_START      1
#define EVENT_TYPE_PHOTO_STOP       2

#define EVENT_ID_SYNC 				51

#define EVENT_ID_FIND_PHONE_SET 			52
#define EVENT_TYPE_FIND_PHONE_SET_ON      	1
#define EVENT_TYPE_FIND_PHONE_SET_OFF       2

#define EVENT_ID_ONE_KEY_REBACK_DEFAULT 	53

#define EVENT_ID_SET_UNIT 					54

#define EVENT_ID_BATT_LOW                   55
#define EVENT_TYPE_LOW_LEV0                 1
#define EVENT_TYPE_LOW_LEV1                 2
#define EVENT_TYPE_LOW_LEV2                 3
#define EVENT_TYPE_LOW_LEV3                 4

#define EVENT_ID_SMS 						56
#define EVENT_TYPE_SMS_START 				1

typedef struct {
    uint8_t id;
    uint8_t type;
} event_t;

typedef void (*event_input_cb_t)(event_t);

void event_button_fun(uint8_t key, uint8_t flag);
void Event_input_cb_reg(event_input_cb_t fun);

#endif      // ifndef __IDEA_EVENT_H

