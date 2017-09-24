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
#ifndef _IDEA_INPUT_EVENT_SOURCE_H_
#define _IDEA_INPUT_EVENT_SOURCE_H_

#include <stdint.h>
#include "config.h"
#include "click-algorithm.h"

#define BUTTON_CLICK						1
#define BUTTON_LONG_CLICK					2
#define BUTTON_DOUBLE_CLICK					3
#define BUTTON_UP                   		4
#define BUTTON_LONG_LONG_CLICK				5

#define THREE_SECOND_INTERVAL        APP_TIMER_TICKS(2000, APP_TIMER_PRESCALER)        /**< long press timer delay . */
#define SHORT_PRESS_DELAY       APP_TIMER_TICKS(500, APP_TIMER_PRESCALER)

typedef void (*app_button_sched_event_handler_t)(click_event_t *event, void* user_data);
typedef void (*button_handle)(uint8_t key,uint8_t flag);

/* enum defination */
typedef enum {
    NO_PRESS = 0,
    SHORT_PRESS,
    LONG_PRESS
}PressType;

/*****************************************************************
* control input action
******************************************************************/
typedef enum {
    INPUT_ACCEPT_BOND               = 1,
    INPUT_CANCEL_PHONE_CALL         = 2,
    INPUT_CANCEL_ALARM              = 3,
    INPUT_SHOW_STEP_COUNT           = 4,
    INPUT_SWAP_SLEEP_MODE           = 5,
    INPUT_DEFAULT_MODE              = 0xFF,             //default mode for input action flags
}InputAction;

#ifdef HAS_BUTTON
/* button identifier timer  hanler*/
void btn_timeout_handler(void * p_context);
#endif
/* button event handler*/
void function_button_event_handler(uint8_t pin_no,uint8_t button_action);

void input_event_handle_schedule(void * p_event_data, uint16_t event_size);

void register_click_button_callback(app_button_sched_event_handler_t handle);

void button_init(button_handle fun);
//input task register
//extern InputAction global_short_press_action ;
//extern InputAction global_long_press_action ;

//void reset_short_press_action_SM(InputAction currentAction);
#endif //_INPUT_EVENT_SOURCE_H_
