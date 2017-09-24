#include "idea_event.h"
#include "simple_uart.h"
#include "idea_input_event_source.h"
#include "idea_board_config_pins.h"
#include "string.h"
#include "idea_display.h"


#define EVENT_STOPPED       0
#define EVENT_STARTING      1

typedef struct {
    uint8_t id;
    uint8_t flag;
    uint8_t type;
    uint8_t timeout_seconds;
    uint8_t level;
} event_data_t;

event_data_t m_event = {.flag = EVENT_STOPPED};

event_input_cb_t event_input_cb = NULL;

static event_t event;

inline static void event_interrupt(uint8_t event_id, uint8_t event_level);

void event_button_fun(uint8_t key, uint8_t flag)
{
////    if (event_input_cb == NULL || m_event_enable == false || Battery_low_power_get_status() > BATTERY_LOW_POWER_NOTIFY_LEVEL2) {
////        return;
////    }
    
    if (key == IDEA_BUTTON_0) {
        event.id = EVENT_ID_BUTTON0;
    } else {
        return;
    }

    if (flag == BUTTON_CLICK) {
        event.type = EVENT_TYPE_BUTTON_CLICK;
    } else if (flag == BUTTON_LONG_CLICK) {
        event.type = EVENT_TYPE_BUTTON_PUSHED_LONG;
    } else if (flag == BUTTON_LONG_LONG_CLICK) {
        event.type = EVENT_TYPE_BUTTON_PUSHED_LONG_LONG;
    }  else if (flag == BUTTON_DOUBLE_CLICK) {
        event.type = EVENT_TYPE_BUTTON_CLICK_DOUBLE;
    } else {
        return;
    }

    if (Ui_menu_get_display_status() == true) {
        if(m_event.flag == EVENT_STARTING && 
            (m_event.id == EVENT_ID_CALL || m_event.id == EVENT_ID_SMS || m_event.id == EVENT_ID_GOAL || 
             m_event.id == EVENT_ID_ALARM || m_event.id == EVENT_ID_ANTI_LOST_ALERT || m_event.id == EVENT_ID_LONG_SIT)) 
        {
            if (flag == BUTTON_CLICK) {
                event_interrupt(event.id, EVENT_LEVEL_0);
            }
        } else {
            if (flag == BUTTON_CLICK) {
                event_interrupt(event.id, EVENT_LEVEL_0);
            }
            event_input_cb(event);
        }
    } else {
        event_input_cb(event);
    }
}

inline static void event_interrupt(uint8_t event_id, uint8_t event_level)
{
    event_t evt;

    if (m_event.flag == EVENT_STOPPED || m_event.id == event_id || event_level > m_event.level) {
        return;
    } else {
        DEBUG_PRINT("event_interrupt\r\n");
        evt.id = m_event.id;
        evt.type = EVENT_TYPE_INTERRUPT;
        event_input_cb(evt);
        m_event.flag = EVENT_STOPPED;
        m_event.timeout_seconds = 0;
    }
}

void Event_input_cb_reg(event_input_cb_t fun)
{
    event_input_cb = fun;
}
