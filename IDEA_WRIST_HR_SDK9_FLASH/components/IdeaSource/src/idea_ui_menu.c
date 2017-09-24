#include "idea_ui_menu.h"
#include "simple_uart.h"
#include "idea_event.h"
#include "app_scheduler.h"
#include "idea_charging.h"
#include "idea_display.h"
#include "idea_board_config_pins.h"
#include "idea_oled.h"
#include "idea_input_event_source.h"

static ui_menu_mode_list_t  ui_menu_mode;
static ui_menu_status_t     ui_menu_status;

static uint8_t m_ui_menue_charging_val = 0;                     // 用于显示充电信息时，电量值

static void ui_menu_event_input_fun(event_t evt);
static void ui_menu_display_mode(uint8_t mode, uint8_t secs);
static void ui_menu_switch_mode(void);
static void ui_menu_mode_init(uint8_t menu_id);

static ui_menu_status_t     ui_menu_status;


static void ui_menu_mode_init(uint8_t menu_id)
{
    DEBUG_PRINT("ui_menu_mode_init id = %d!\r\n", menu_id);

    ui_menu_status.last_ui_menu_id = ui_menu_status.ui_menu_id;
    ui_menu_status.ui_menu_id = menu_id;
    
    // 暂时先只实现NORMAL状态
    if (menu_id == UI_MENU_ID_NORMAL) {
        
        ui_menu_mode.count                              = 0;
        ui_menu_mode.list[ui_menu_mode.count++]         = UI_MENU_TIME_MODE;
        ui_menu_mode.list[ui_menu_mode.count++]         = UI_MENU_STEP_MODE;
        ui_menu_mode.list[ui_menu_mode.count++]         = UI_MENU_DISTANCE_MODE;
        ui_menu_mode.list[ui_menu_mode.count++]         = UI_MENU_CALORIES_MODE;
        #if USE_HEART_RATE
        ui_menu_mode.list[ui_menu_mode.count++]         = UI_MENU_HR_MODE;
        #endif
        
        
        // 闹钟和查找手机，暂时不写，后续添加
////        if (Alarm_has_on()) {
////           ui_menu_mode.list[ui_menu_mode.count++]      = UI_MENU_ALARM_MODE;
////        }
////        if (User_is_find_phone_on()) {
////            ui_menu_mode.list[ui_menu_mode.count++]     = UI_MENU_FIND_PHONE_MODE;
////        }
////        #if USE_HARDWARE_CHECK
////        if (app_init_get_hardware_init_err_code() != 0) {
////            ui_menu_mode.list[ui_menu_mode.count++] = UI_MENU_NOTIFY_INIT_FAIL_MODE;
////        }
////        #endif
        ui_menu_mode.pos = 0;
        ui_menu_status.current_mode                     = ui_menu_mode.list[ui_menu_mode.pos];
    }
}

static void ui_menu_event_input_fun_schedule(void * p_event_data, uint16_t event_size)
{
    uint8_t on_secs;                                                                // display time .... seconds
    event_t *event = (event_t *)p_event_data;
    PressType type;
    
    if (event_size != sizeof(event_t)) 
    {
        return;
    }
    DEBUG_PRINT("ui_menu_event_input_fun_schedule!\r\n");
    switch(event->id)
    {
        // 按键事件，if判断语句判断单击，双击，长按，长长按事件
        case EVENT_ID_BUTTON0:            
            if(event->type == EVENT_TYPE_BUTTON_CLICK)                              // 单击事件
            {
                if (ui_menu_status.show_status == UI_MENU_SHOW_ONLY_LOW_POWER)      // 电池电量较低，仅显示电量信息 
                {
                    if (ui_menu_status.status == UI_MENU_DISPLAY_OFF) {             // 如果是息屏状态，单击时显示低电量信息或充电信息
                        if(charger_status() == InCharging)
                        {
                            ui_menu_display_mode(UI_MENU_NOTIFY_CHARGING_MODE, UI_MENU_DISPLAY_ON_DEFAULT_SECS);            // display charging mode
                            DEBUG_PRINT("InCharging!!! \r\n");
                        } else if(charger_status() == ChargingComplete){                                                    // Incharge 
                            ui_menu_display_mode(UI_MENU_NOTIFY_CHARGING_FULL_MODE, UI_MENU_SHOW_ONLY_LOW_POWER_ON_SECS);   // display charge full mode
                            DEBUG_PRINT("Charge full!!! \r\n");
                        } else{
                            ui_menu_display_mode(UI_MENU_NOTIFY_LOW_POWER_MODE, UI_MENU_SHOW_ONLY_LOW_POWER_ON_SECS);       // display low power mode
                            DEBUG_PRINT("Low power!!! \r\n");
                        }
////                        if (battery_get_charging_status() != BATTERY_CHAGING) {
////                            ui_menu_display_mode(UI_MENU_NOTIFY_LOW_POWER_MODE, UI_MENU_SHOW_ONLY_LOW_POWER_ON_SECS);
////                        } else {
////                            ui_menu_display_mode(UI_MENU_NOTIFY_CHARGING_MODE, UI_MENU_DISPLAY_ON_DEFAULT_SECS);
////                        }
                    }
                    return;
                }
                
                if (ui_menu_status.show_status == UI_MENU_SHOW_NONE)      // 电池电量很低，单击时不显示任何信息 
                {
                    return;
                }
                // 电量非 UI_MENU_SHOW_ONLY_LOW_POWER和UI_MENU_SHOW_NONE状态，即为UI_MENU_SHOW_ALL状态 
                if (ui_menu_status.status == UI_MENU_DISPLAY_OFF) 
                {
                    if(charger_status() == InCharging)
                    {
                        ui_menu_status.current_mode = UI_MENU_NOTIFY_CHARGING_MODE;
                    } else if(charger_status() == ChargingComplete){                                                    // Incharge 
                        ui_menu_status.current_mode = UI_MENU_NOTIFY_CHARGING_FULL_MODE;
                        DEBUG_PRINT("UI_MENU_SHOW_ALL-->display ChargingComplete state!!! \r\n");
                    }
                    
                    if (ui_menu_status.current_mode == UI_MENU_HR_MODE) {
                        on_secs = UI_MENU_HR_DISPLAY_ON_SECS + 2;
////                        #if USE_HEART_RATE
////                        menu_hr_mode_secs = 2;
////                        #endif
                    }  else {
                        on_secs = UI_MENU_DISPLAY_ON_DEFAULT_SECS;
                    }
                    // 显示函数，此函数后续添加
                    ui_menu_display_mode(ui_menu_status.current_mode, on_secs);
                } else {                                                                            // 非熄屏状态下
                    // 切换模式，
                    ui_menu_switch_mode();
                    DEBUG_PRINT("ui_menu_switch_mode!!! \r\n");
                }
                
                
                
            } else if(event->type == EVENT_TYPE_BUTTON_PUSHED_LONG)         // 长按事件
            {
                 ;
            } else if(event->type == EVENT_TYPE_BUTTON_CLICK_DOUBLE)        // 双击事件
            {
                DEBUG_ERROR("short press!!\r\n");
                type = SHORT_PRESS;
                app_sched_event_put(&type,sizeof(PressType),input_event_handle_schedule);
            } else if(event->type == EVENT_TYPE_BUTTON_PUSHED_LONG_LONG)    // 长长按事件
            {
                 ;
            }
            break;
        default :
            break;
    }
}

static void ui_menu_switch_mode(void)
{
    uint8_t on_secs = UI_MENU_DISPLAY_ON_DEFAULT_SECS;

    // ble 开关广播，调功耗时用
////    ble_adv_activity_status(UI_MENU_DISPLAY_ON_DEFAULT_SECS);
////    ble_adv_config(true);

    if (ui_menu_status.current_mode == ui_menu_mode.list[ui_menu_mode.pos]) {
        ui_menu_mode.pos = (ui_menu_mode.pos < (ui_menu_mode.count - 1)) ? (ui_menu_mode.pos + 1) : 0;
        ui_menu_status.current_mode = ui_menu_mode.list[ui_menu_mode.pos];
    } else {
        ui_menu_status.current_mode = ui_menu_mode.list[ui_menu_mode.pos];
    }

    #if USE_HEART_RATE
    if (ui_menu_status.current_mode == UI_MENU_HR_MODE) {
        on_secs = UI_MENU_HR_DISPLAY_ON_SECS + 2;
////        menu_hr_mode_secs = 2;
    } else {
////        menu_hr_mode_secs = 0;
////        if (menu_hr_mode) {
////            User_hr_measurement_stop(USER_HR_MEASUREMENT_STATUS_TIMEOUT);
////            menu_hr_mode = false;
////        }
    }
    #endif

    ui_menu_display_mode(ui_menu_status.current_mode, on_secs);
}

static void ui_menu_event_input_fun(event_t evt)
{
    // 若此时电量很低，又不是bat low事件触发，则不进行任何显示，为了省电
    if (ui_menu_status.show_status == UI_MENU_SHOW_NONE && evt.id != EVENT_ID_BATT_LOW) {
        return;
    }
    DEBUG_PRINT("ui_menu_event_input_fun evt = %d , type = %d\r\n", evt.id, evt.type);
    app_sched_event_put(&evt, sizeof(event_t), ui_menu_event_input_fun_schedule);
} 

bool Ui_menu_get_display_status(void)
{
////    DEBUG_PRINT("ui display status = %d",ui_menu_status.status);

    return ui_menu_status.status == UI_MENU_DISPLAY_ON;
}

void Ui_menu_init(void)
{
////    Event_input_init();
    Event_input_cb_reg(ui_menu_event_input_fun);
    
    ui_menu_mode_init(UI_MENU_ID_NORMAL);                   // 主要对ui_menu_mode列表值初始化
}

// 显示函数，sec为显示的时间 
void ui_menu_display_on(uint8_t sec)
{
    if (ui_menu_status.status == UI_MENU_DISPLAY_OFF) {
        if (ui_menu_status.show_status == UI_MENU_SHOW_ALL) {
            ////            ble_adv_config(true);                               // 开广播，低功耗时，要开关广播，此处先暂时不处理
        }
////        Oled_display_on();
        OLED_Display_On();
        ui_menu_status.status = UI_MENU_DISPLAY_ON;
        ui_menu_status.display_on_secs = sec;
    } else {
        ui_menu_status.display_on_secs = sec;
    }
    return;
}

static void ui_menu_display_mode(uint8_t mode, uint8_t secs)
{
    ui_menu_display_on(secs);
    ui_menu_status.current_mode = mode;
    switch (mode)
    {
        case UI_MENU_TIME_MODE:
            Display_show_time_mode();
            break;
#if USE_HEART_RATE
        case UI_MENU_HR_MODE:
            Display_show_hr_mode();
            break;
#endif          // #if USE_HEART_RATE
        case UI_MENU_STEP_MODE:
            Display_show_step_mode();
            break;
        case UI_MENU_DISTANCE_MODE:
            Display_show_distance_mode();
            break;
        case UI_MENU_CALORIES_MODE:
            Display_show_calories_mode();
            break;
        case UI_MENU_NOTIFY_CHARGING_MODE:
            m_ui_menue_charging_val = 10;
            Display_show_charging_mode(m_ui_menue_charging_val);
            break;
        case UI_MENU_NOTIFY_LOW_POWER_MODE:
            Display_show_low_power_mode();
            break;
        case UI_MENU_NOTIFY_CHARGING_FULL_MODE:
            Display_show_charging_mode(100);
            break;
        
        default:
            break;
    }
}

void Ui_menu_display_off(void)
{
    ////Oled_display_off();
    OLED_Display_Off();
    ui_menu_status.status = UI_MENU_DISPLAY_OFF;
    if (ui_menu_status.current_mode == UI_MENU_NOTIFY_CALLING_MODE || ui_menu_status.current_mode == UI_MENU_NOTIFY_SMS_MODE || 
        ui_menu_status.current_mode == UI_MENU_NOTIFY_ALARMING_MODE || ui_menu_status.current_mode == UI_MENU_NOTIFY_LONG_SIT_MODE || 
        ui_menu_status.current_mode == UI_MENU_NOTIFY_ANTI_LOST_MODE || 
        ui_menu_status.current_mode == UI_MENU_NOTIFY_ACHIEVE_TARGET_MODE) {
        ui_menu_status.display_off_secs = 0;
    } else {
        ui_menu_status.display_off_secs = 5;
    }

}

void Ui_menu_1s(void)
{
    // 得到当前心率的值，并刷新显示
////    heart_rate_data_t hr_d;
////    hr_d.rate = hr_g_get_hr();
////    ui_menue_refresh_hr_fun(hr_d);
    // 已经熄屏，熄屏一段时间后current mode 置为0
    if (ui_menu_status.status == UI_MENU_DISPLAY_OFF && ui_menu_status.display_on_secs < 1) 
    {
        if (ui_menu_status.display_off_secs > 0) {
            ui_menu_status.display_off_secs--;
            if (ui_menu_status.display_off_secs == 0) {
                ui_menu_mode.pos = 0;
                ui_menu_status.current_mode = ui_menu_mode.list[0];
            }
        }
    } else {
        ui_menu_status.display_on_secs--;
        if (ui_menu_status.display_on_secs == 0) {
//            Flicker_enable_disable(false);
            Ui_menu_display_off();
        }
        if(ui_menu_status.current_mode == UI_MENU_TIME_MODE)
        {
            refresh_time();
        }
#if USE_HEART_RATE
        else if (ui_menu_status.current_mode == UI_MENU_HR_MODE)
        {
            refresh_heart_rate();
        }
#endif                      // #if USE_HEART_RATE
        
    }
}

