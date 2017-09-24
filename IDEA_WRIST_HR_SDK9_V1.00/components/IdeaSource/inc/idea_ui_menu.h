#ifndef __IDEA_UI_MENU_H
#define __IDEA_UI_MENU_H

#include <stdint.h>
#include <stdbool.h>

#define UI_MENU_DISPLAY_OFF                         0
#define UI_MENU_DISPLAY_ON                          1

#define UI_MENU_SHOW_ALL                            0       // 电量足够，可以显示任意界面
#define UI_MENU_SHOW_ONLY_LOW_POWER                 1       // 电量较低，只可显示lowpower界面
#define UI_MENU_SHOW_NONE                           2       // 电量很低，只有bat_low 事件才能触发显示

#define UI_MENU_DISPLAY_ON_DEFAULT_SECS             4       // 默认显示时间：4s
#define UI_MENU_SHOW_LOGO_ON_SECS                   2
#define UI_MENU_SHOW_ONLY_LOW_POWER_ON_SECS         3
#define UI_MENU_DISPLAY_ON_DEFAULT_SHORT_SECS       3
#define UI_MENU_DISPLAY_ON_DEFAULT_LONG_SECS        7
#define UI_MENU_GESTURE_DISPLAY_ON_SECS             3
#define UI_MENU_HR_GESTURE_DISPLAY_ON_SECS          3
#define UI_MENU_PHOTO_GESTURE_DISPLAY_ON_SECS       3
#define UI_MENU_CALLING_DISPLAY_ON_SECS             9
#define UI_MENU_SMS_DISPLAY_ON_SECS                 7

#define UI_MENU_HR_DISPLAY_ON_SECS                  60      // 心率模式显示时间：60s

#define UI_MENU_ID_NORMAL                           1       // 正常模式
#define UI_MENU_ID_SPORT                            2       // 跑步模式
#define UI_MENU_ID_PHOTO                            3       // 拍照模式

#define UI_MENU_TIME_MODE                           1
#define UI_MENU_HR_MODE                             2
#define UI_MENU_DATA_MODE_START                     2
#define UI_MENU_STEP_MODE                           3
#define UI_MENU_DISTANCE_MODE                       4
#define UI_MENU_CALORIES_MODE                       5
#define UI_MENU_DATA_MODE_END                       6
#define UI_MENU_ALARM_MODE                          6
#define UI_MENU_FIND_PHONE_MODE                     7
#define UI_MENU_FINDING_PHONE_MODE                  8
#define UI_MENU_SPORT_MODE_START                    9
#define UI_MENU_SPORT_HR_MODE                       10
#define UI_MENU_SPORT_DATA_MODE_START               10
#define UI_MENU_SPORT_STEP_MODE                     11
#define UI_MENU_SPORT_DISTANCE_MODE                 12
#define UI_MENU_SPORT_CALORIES_MODE                 13
#define UI_MENU_SPORT_DATA_MODE_END                 14
#define UI_MENU_SPORT_TIME_MODE                     14
#define UI_MENU_SPORT_MODE_END                      15
#define UI_MENU_CAMERA_MODE                         16
#define UI_MENU_PHOTOING_MODE                       17
#define UI_MENU_SHIPMENT_MODE                       18
#define UI_MENU_SPORT_NOTIFY_MODE                   19
#define UI_MENU_ERROR_MODE                          20


#define UI_MENU_NOTIFY_CALLING_MODE                 21
#define UI_MENU_NOTIFY_ALARMING_MODE                22
#define UI_MENU_NOTIFY_LONG_SIT_MODE                23
#define UI_MENU_NOTIFY_ANTI_LOST_MODE               24
#define UI_MENU_NOTIFY_ACHIEVE_TARGET_MODE          25
#define UI_MENU_NOTIFY_LOW_POWER_MODE               26
#define UI_MENU_NOTIFY_CHARGING_MODE                27
#define UI_MENU_NOTIFY_CHARGING_FULL_MODE           28
#define UI_MENU_NOTIFY_OTA_MODE                     29
#define UI_MENU_NOTIFY_INIT_FAIL_MODE               30
#define UI_MENU_NOTIFY_SMS_MODE                     31
#define UI_MENU_FACTORY_TEST_MODE                   32


#define UI_MENU_MODE_MAX                            6


typedef struct {
    uint8_t status;             // off dark on 屏幕是否被点亮状态
    uint8_t show_status;        // 根据电量高低，决定要显示的内容
    uint8_t current_mode;
    uint8_t last_ui_menu_id;    // 上一次的状态值
    uint8_t ui_menu_id;         // 本次的状态值
    uint8_t display_on_secs;
    uint8_t display_off_secs;
    bool flicker_switch;
} ui_menu_status_t;

typedef struct {
    uint8_t list[UI_MENU_MODE_MAX + 3];
    uint8_t pos;
	uint8_t count;
} ui_menu_mode_list_t;

void Ui_menu_init(void);              
bool Ui_menu_get_display_status(void);
void Ui_menu_init(void);
void Ui_menu_1s(void);

#endif
