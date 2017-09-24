/* Copyright (c) [2014 Baidu]. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * File Name          : 
 * Author             : 
 * Version            : $Revision:$
 * Date               : $Date:$
 * Description        : 
 *                      
 * HISTORY:
 * Date               | Modification                    | Author
 * 28/03/2014         | Initial Revision                | 
 
 */
#include "string.h"
#include "stdint.h"

#include "config.h"
#include "idea_input_event_source.h"
#include "board_config_pins.h"
#include "idea_ble_nus.h"
#include "idea_buzzer.h"
#include "app_button.h"
#include "app_timer.h"
#include "idea_communicate_protocol.h"
#include "step_counter.h"
#include "idea_wall_clock_timer.h"
#include "string.h"
#include "simple_uart.h"
#include "idea_private_bond.h"
#include "idea_switch_sleep.h"
#include "idea_interaction.h"
#include "idea_level_drive_motor.h"
#include "idea_charging.h"
#include "idea_led_flash.h"
#include "idea_sync_data.h"
#include "idea_board_config_pins.h"

#define BUTTON_DETECTION_DELAY               		APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)
#define BUTTON_TIMER_LONG_PUSHED_INTERVAL           APP_TIMER_TICKS(100,APP_TIMER_PRESCALER)
#define BUTTON_TIMER_DOUBLE_CLICK_INTERVAL          APP_TIMER_TICKS(350,APP_TIMER_PRESCALER)

// 长按超时
#define BUTTON_LONG_TIMEOUT						(1700 / 100) 
#define BUTTON_LONG_LONG_TIMEOUT				(7000 / 100) 

static app_button_sched_event_handler_t button_handler_fun = NULL;

extern ble_nus_t                        m_nus;
#ifdef HAS_BUTTON
extern app_timer_id_t  btn_identi;
#endif

extern BLUETOOTH_BOND_STATE private_bond_machine;

static bool             key_down_flag                   = false;
static volatile bool    m_button_long_ok                = false;
static volatile bool    m_button_double_triger          = false;
static volatile bool    button_double_click_is_on       = false;
static volatile bool    button_double_click_is_start    = false;
static uint8_t          button_pushed_flag;
static uint16_t         button_timer_count;
static button_handle    button_handle_fun               = NULL;

static app_timer_id_t   m_button_timer;
static app_timer_id_t   m_button_double_click_timer;

/////////////////////////////////////////////////////////////////
#ifdef TEST_SHOW_WALL_CLOCK

void report_current_time(void)
{
    // uint8_t outputArr[30];
    UTCTimeStruct * tm = get_wall_clock_time();
    // sprintf((char *)outputArr,"%d-%d-%d %d:%d:%d \r\n",tm->year,tm->month,tm->day,tm->hour,tm->minutes,tm->seconds);
    // simple_uart_putstring((const uint8_t *)outputArr);
    DEBUG_PRINT("%d-%d-%d %d:%d:%d \n",tm->year,tm->month,tm->day,tm->hour,tm->minutes,tm->seconds);
}

#endif

void input_event_handle_schedule(void * p_event_data, uint16_t event_size)
{
    if(event_size != sizeof(PressType)) {
        return;
    }

    PressType * type = (PressType * )p_event_data;

    switch(*type) {

        case SHORT_PRESS: {
                uint8_t notif = notification_status();
#ifdef DEBUG_LOG
                DEBUG_PRINT("input_event_handle_schedule:[%d]\n",notif);
#endif

                if(notif == NOTIFICATION_BONDING ) { // bond wait knock
                    bond_press_handle();
                } else if(!check_has_bonded()) {
                    break;
                } else if(notif == NOTIFICATION_CALLING) { //phone comming, knock to stop
                    notification_stop();
                } else if(notif == NOTIFICATION_ALARM) { //clock alarm ,knock to stop
                    notification_stop();
                } else if(notif == NOTIFICATION_STATE) { //knock to view step conter status
#ifdef DEBUG_LOG
//                    DEBUG_PRINT("NOTIFICATION_STATE\n");
#endif

                    notification_start(NOTIFICATION_STATE,0);
                }
                //else just igore
#ifdef DEBUG_LOG
//                DEBUG_PRINT("input_event_handle_schedule:[%d]\n",notif);
#endif

            }
            break;

        case LONG_PRESS: {
#ifdef DEBUG_LOG
//                DEBUG_PRINT("input_event_handle_schedule:LONG_PRESS\n");
#endif

                if(check_has_bonded()) {
                    //can not switch sleep mode, when charging
                    if( charger_status() != NoCharge ) {
                        break;
                    }
                    uint8_t notif = notification_status();
                    if(notif == NOTIFICATION_CALLING) { //phone comming, knock to stop
                        notification_stop();
                    } else if(notif == NOTIFICATION_ALARM) { //clock alarm ,knock to stop
                        notification_stop();
                    } else { //knock to view step conter status
                        two_Double_tap_handle();
                        send_all_data(false);
                    }
                }
            }
            break;
        default:
            break;


    }
}
#ifdef HAS_BUTTON
/* button identifier timer  hanler*/
void btn_timeout_handler(void * p_context)
{
    uint8_t *key = (uint8_t *)p_context;

    if(*key == NO_PRESS) { // long press time out handler
        *key = LONG_PRESS; // Long press identify

        PressType type = LONG_PRESS;
        app_sched_event_put(&type,sizeof(PressType),input_event_handle_schedule);

        return;
    }
}
#endif

/* button event handler*/
#ifdef FEATURE_BUTTON
void function_button_event_handler(uint8_t pin_no,uint8_t button_action)
{  
//    static uint8_t pressType = NO_PRESS;
    click_event_t * event;
    void * user_data;
    uint32_t err_code;
    if((button_action == APP_BUTTON_RELEASE) && (key_down_flag == true))
    {
        if (pin_no == IDEA_BUTTON_0 && button_handle_fun && m_button_long_ok == false && m_button_double_triger == false) 
        {
            button_handle_fun(IDEA_BUTTON_0, BUTTON_CLICK);
            #if USE_VBUS
                if(button_dtm_mode == true)
                {
                    struct vbus_dtm_key key;
                    key.id = 0;
                    key.type = 0x01;
                    vbus_tx_data(VBUS_EVT_DTM_KEY_SEND,&key,sizeof(key));
                }
            #endif      // #if USE_VBUS
        }
    }
    
    if(button_action != APP_BUTTON_PUSH)
    {
        return ;
    }
    
    switch (pin_no)
    {
        
        case IDEA_BUTTON_0 :
            button_pushed_flag = 1;
            button_timer_count = 0;
            m_button_long_ok  = false;
            m_button_double_triger = false;
            break;
        default:
            APP_ERROR_HANDLER(pin_no);
            break;
    }
    
    if(button_pushed_flag)
    {
        err_code = app_timer_stop(m_button_timer);
        APP_ERROR_CHECK(err_code);
        err_code = app_timer_start(m_button_timer, BUTTON_TIMER_LONG_PUSHED_INTERVAL, NULL);
        APP_ERROR_CHECK(err_code);
////        if (button_double_click_is_on) {
            if (button_double_click_is_start == false) {
                err_code = app_timer_stop(m_button_double_click_timer);
                APP_ERROR_CHECK(err_code);
                err_code = app_timer_start(m_button_double_click_timer, BUTTON_TIMER_DOUBLE_CLICK_INTERVAL, NULL);
                APP_ERROR_CHECK(err_code);
                button_double_click_is_start = true;
            } else {
                key_down_flag = false;
                button_pushed_flag = 0;
                if (button_handle_fun) {
                    button_handle_fun(IDEA_BUTTON_0, BUTTON_DOUBLE_CLICK);
                    m_button_double_triger = true;
                }
                button_double_click_is_start = false;
                err_code = app_timer_stop(m_button_double_click_timer);
                APP_ERROR_CHECK(err_code);
            }
////        }
        return ;
    }

////    if(pin_no == IDEA_BUTTON_0) {
////        nrf_gpio_pin_set(13);
////        button_handler_fun(event, user_data);
////        nrf_gpio_pin_clear(13);
////        return;
////    }


}
#endif    // #ifdef FEATURE_BUTTON

static void button_timer_long_pushed_handle(void * p_contex)
{ 
    bool flag;

    app_button_is_pushed(0, &flag);

    // 按键未松开
    if (flag == true) {
        if(button_pushed_flag) {
            key_down_flag = true;
            button_timer_count ++;
            if (button_timer_count > BUTTON_LONG_LONG_TIMEOUT) {
                if (button_handle_fun != NULL) {
                    button_handle_fun(IDEA_BUTTON_0, BUTTON_LONG_LONG_CLICK);
                    key_down_flag = false;
                    app_timer_stop(m_button_timer);
                    button_pushed_flag = false;
                }
            } else if (button_timer_count > BUTTON_LONG_TIMEOUT && m_button_long_ok == false) {
                m_button_long_ok = true;
                if (button_handle_fun != NULL) {
                    button_handle_fun(IDEA_BUTTON_0, BUTTON_LONG_CLICK);
                }
            }
        }
    } else {
////        if (button_pushed_flag) {
            app_timer_stop(m_button_timer);
////        }
        
        button_pushed_flag = 0;
        button_timer_count = 0;
    }
}

static void button_timer_double_click_handle(void * p_contex)
{
    uint32_t err_code;
    button_double_click_is_start = false;
    err_code = app_timer_stop(m_button_double_click_timer);
    APP_ERROR_CHECK(err_code);
}

void button_init(button_handle fun)
{
    uint32_t err_code;

    err_code = app_timer_create(&m_button_timer, APP_TIMER_MODE_REPEATED, button_timer_long_pushed_handle);
    APP_ERROR_CHECK(err_code);
	
	err_code = app_timer_create(&m_button_double_click_timer, APP_TIMER_MODE_SINGLE_SHOT, button_timer_double_click_handle);
    APP_ERROR_CHECK(err_code);
	button_handle_fun = fun;
		
#if USE_VBUS
    button_vubs_init();
#endif
}


void register_click_button_callback(app_button_sched_event_handler_t handle)
{
#ifdef DEBUG_LOG
//    DEBUG_PRINT("register_battery_up_callback \r\n");
    //  simple_uart_putstring((const uint8_t *)"register_battery_up_callback \r\n");
#endif

    button_handler_fun = handle;
}

