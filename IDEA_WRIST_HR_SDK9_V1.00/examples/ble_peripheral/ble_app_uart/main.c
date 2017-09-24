/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_uart_over_ble_main main.c
 * @{
 * @ingroup  ble_sdk_app_nus_eval
 * @brief    UART over BLE application main file.
 *
 * This file contains the source code for a sample application that uses the Nordic UART service.
 * This application uses the @ref srvlib_conn_params module.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf51_bitfields.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_button.h"
//#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp.h"
#include "bsp_btn_ble.h"

#ifdef IDEA_DEBUG
#include "simple_uart.h"
#endif


#include "ble_bas.h"
#include "idea_board_config_pins.h"
#include "idea_ble_nus.h"
#include "idea_buzzer.h"
#include "idea_charging.h"
#include "idea_battery.h"
#include "idea_communicate_protocol.h"
#include "idea_input_event_source.h"
#include "idea_led_flash.h"
#include "idea_work_state.h"
#include "step_counter.h"
#include "idea_private_bond.h"
#include "idea_level_drive_motor.h"
#include "idea_factory_test.h"
#include "idea_low_power_mode.h"
#include "idea_interaction.h"
#include "hal_acc.h"
#include "idea_switch_sleep.h"
#include "idea_wdt.h"
#include "idea_sync_data.h"
#include "idea_data_manager.h"
#include "idea_oled.h"
#include "app_gpiote.h"
////#include "ble_bondmngr.h"
#include "ble_radio_notification.h"
#include "ble_flash.h"
#include "ble_tps.h"
#include "ble_debug_assert_handler.h"
#include "ble_dis.h"
#include "idea_sw_flash.h"
#include "idea_event.h"
#include "idea_ui_menu.h"


#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                           /**< Include the service_changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

////#define DEVICE_NAME                     "Nordic_UART"                               /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         /**< The advertising timeout (in units of seconds). */

////#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
////#define APP_TIMER_MAX_TIMERS            16                                          /**< Maximum number of simultaneously created timers. */
////#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

////#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
////#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
////#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
////#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
////#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
////#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
////#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define START_STRING                    "Start...\n"                                /**< The string that will be sent over the UART when the application starts. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */

static ble_tps_t                        m_tps;                                      /**< Structure used to identify the TX Power service. */

ble_bas_t                               m_bas;                                            /**< Structure used to identify the battery service. */
ble_nus_t                               m_nus;                                      /**< Structure to identify the Nordic UART Service. */

static ble_gap_sec_params_t               m_sec_params;                             /**< Security requirements for this application. */
////static ble_advertising_mode_t             m_advertising_mode;                       /**< Variable to keep track of when we are advertising. */

static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */

static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};  /**< Universally unique service identifier. */

/******************************************************************************
 * timers for OTA delay
 ******************************************************************************/
app_timer_id_t  ota_delay_timer;
app_timer_id_t  one_second_delay_timer;

#define ONE_SECOND_TIMER                APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)

uint16_t global_connect_handle = BLE_CONN_HANDLE_INVALID;

/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse 
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of 
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_nus    Nordic UART Service structure.
 * @param[in] p_data   Data to be send to UART module.
 * @param[in] length   Length of the data.
 */
/**@snippet [Handling the data received over BLE] */
static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
////        while(app_uart_put(p_data[i]) != NRF_SUCCESS);
    }
////    while(app_uart_put('\n') != NRF_SUCCESS);
}
/**@snippet [Handling the data received over BLE] */


/* update the battery in main loop */
void battery_up(void * p_event_data, uint16_t event_size)
{
    (void)event_size;
    uint32_t err_code;
    uint8_t batt_lvl = *((uint8_t*)p_event_data);
    static int low_power_count = 0;

    err_code = ble_bas_battery_level_update(&m_bas, batt_lvl);
    if (
        (err_code != NRF_SUCCESS)
        &&
        (err_code != NRF_ERROR_INVALID_STATE)
        &&
        (err_code != BLE_ERROR_NO_TX_BUFFERS)
        &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
    ) {
        APP_ERROR_HANDLER(err_code);
    }
    if( batt_lvl <= 0 ) {
        low_power_count ++;
        if( ( low_power_count > 3 ) && ( charger_status() == NoCharge ) ) {
            //TODO Low Power
            //1. BT disable
            err_code = sd_ble_gap_disconnect(global_connect_handle, BLE_HCI_REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF);
            APP_ERROR_HANDLER(err_code);
            //2. Sync Data
            low_power_info_store();
            //3. System Off
            power_system_off();
        }
    } else {
        low_power_count = 0;
    }
    
    //update charging status, when battery level is changed.
    update_charging_status(0);
}

/**@brief Initialize Tx Power Service.
 */
static void tps_init(void)
{
    uint32_t       err_code;
    ble_tps_init_t tps_init_obj;

    memset(&tps_init_obj, 0, sizeof(tps_init_obj));
    tps_init_obj.initial_tx_power_level = TX_POWER_LEVEL;

    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&tps_init_obj.tps_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&tps_init_obj.tps_attr_md.write_perm);

    err_code = ble_tps_init(&m_tps, &tps_init_obj);
    APP_ERROR_CHECK(err_code);
}

void battery_up(void * p_event_data, uint16_t event_size);
/**@brief Initialize Battery Service.
 */
static void bas_init(void)
{
    uint32_t       err_code;
    ble_bas_init_t bas_init_obj;

    memset(&bas_init_obj, 0, sizeof(bas_init_obj));

    bas_init_obj.evt_handler          = NULL;
    bas_init_obj.support_notification = false; /* Warning: donot change it to true if you do not undstand what you are doing & what it will cause */
    bas_init_obj.p_report_ref         = NULL;
    bas_init_obj.initial_batt_level   = 100;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init_obj.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init_obj.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init_obj.battery_level_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init_obj.battery_level_report_read_perm);

    err_code = ble_bas_init(&m_bas, &bas_init_obj);
    APP_ERROR_CHECK(err_code);
    register_battery_up_callback(battery_up);
}

/****************************************************************
* ble_nus_service_init
*****************************************************************/
static void ble_nus_service_init()
{
    uint32_t err_code;
    static ble_nus_init_t nus_init;

    memset(&nus_init, 0, sizeof nus_init);
    nus_init.data_handler = L1_receive_data;
    nus_init.connect_handler = bluetooth_l0_reset;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}

/*******************************************************************************
* Device information init
********************************************************************************/
static void ble_devinfo_serv_init(void)
{
    uint32_t        err_code;
    ble_dis_init_t  dis_init;

    // Initialize Device Information Service
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);
    ble_srv_ascii_to_utf8(&dis_init.fw_rev_str, FW_REV_STR);
    ble_srv_ascii_to_utf8(&dis_init.model_num_str,MODULE_NUM);
    ble_srv_ascii_to_utf8(&dis_init.hw_rev_str,HW_REV_STR);


    uint8_t serial_num[33];
    serial_num[32] = 0;

    uint32_t *addr;
    ble_flash_page_addr(FLASH_PAGE_SN_FACTORY_FLAG, &addr );

    if(is_factory_test_done()) {//has do factory test
        for(uint8_t i = 0 ; i<SERIAL_NUMBER_LENGTH ; ++i) {
            serial_num[i] = *(((uint8_t *) (addr)) + i);
        }
        ble_srv_ascii_to_utf8(&dis_init.serial_num_str,(char *)serial_num);
    } else {
        ble_srv_ascii_to_utf8(&dis_init.serial_num_str,SERIAL_NUM);
    }

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
}



/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t       err_code;
    ble_nus_init_t nus_init;
    
    memset(&nus_init, 0, sizeof(nus_init));

////    nus_init.data_handler = nus_data_handler;
////    
////    err_code = ble_nus_init(&m_nus, &nus_init);
////    APP_ERROR_CHECK(err_code);
    tps_init();
    bas_init();
    ble_nus_service_init();
    ble_devinfo_serv_init();
}

/**@brief Initialize security parameters.
 */
static void sec_params_init(void)
{
    m_sec_params.bond         = SEC_PARAM_BOND;
    m_sec_params.mitm         = SEC_PARAM_MITM;
    m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    m_sec_params.oob          = SEC_PARAM_OOB;
    m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}

/**@brief Initialize Radio Notification event handler.
 */
static void radio_notification_init(void)
{
    uint32_t err_code;

    err_code = ble_radio_notification_init(NRF_APP_PRIORITY_HIGH,
                                           NRF_RADIO_NOTIFICATION_DISTANCE_4560US,
                                           ble_flash_on_radio_active_evt);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;
    
    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;
    
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;
    
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;
        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;
        default:
            break;
    }
}


/**@brief Function for the Application's S110 SoftDevice event handler.
 *
 * @param[in] p_ble_evt S110 SoftDevice event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t                         err_code;
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            err_code = bsp_indication_set(BSP_INDICATE_IDLE);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for dispatching a S110 SoftDevice event to all modules with a S110 SoftDevice 
 *        event handler.
 *
 * @details This function is called from the S110 SoftDevice event interrupt handler after a S110 
 *          SoftDevice event has been received.
 *
 * @param[in] p_ble_evt  S110 SoftDevice event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    if(p_ble_evt->header.evt_id == BLE_GAP_EVT_CONNECTED) {
        set_global_bluetooth_status(ShortConnectInterval);
    } else if(p_ble_evt->header.evt_id == BLE_GAP_EVT_DISCONNECTED) {
        set_global_bluetooth_status(NotWork); //disconnected and not start advertising
    }
    
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_bas_on_ble_evt(&m_bas, p_ble_evt);
    on_ble_evt(p_ble_evt);
////    ble_advertising_on_ble_evt(p_ble_evt);
////    bsp_btn_ble_on_ble_evt(p_ble_evt);
    
}


/**@brief Function for the S110 SoftDevice initialization.
 *
 * @details This function initializes the S110 SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;
    
    // Initialize SoftDevice.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);

    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
#ifdef S130
    ble_enable_params.gatts_enable_params.attr_tab_size   = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
#endif
    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    // Subscribe for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            err_code = ble_advertising_restart_without_whitelist();
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            break;
    }
}


/**@snippet [UART Initialization] */


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    ble_advdata_t scanrsp;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = false;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = m_adv_uuids;

    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = BLE_ADV_FAST_ENABLED;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, &scanrsp, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                                 APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), 
                                 bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for placing the application in low power state while waiting for events.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the GPIOTE handler module.
 */
static void gpiote_init(void)
{
    APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
}

/**@brief Initialize button handler module.
 */
static void gpiote_int_init(void)
{
    if(true/*is_factory_test_done()*/) {
        static app_button_cfg_t buttons[] = {
                {FUNCTION_BUTTON, false, NRF_GPIO_PIN_PULLUP ,function_button_event_handler },
#ifdef FEATURE_SENSOR_LIS3DH
                {LIS3DH_INT1, APP_BUTTON_ACTIVE_HIGH, NRF_GPIO_PIN_NOPULL ,LIS3DH_INT1_event_handler},
                {LIS3DH_INT2, APP_BUTTON_ACTIVE_HIGH, NRF_GPIO_PIN_NOPULL ,LIS3DH_INT2_event_handler},
#endif
                //charger GPIO init
                //Charger: Plug In -> GPIO Low; Plug Out -> GPIO High
                //ChargingComplete: FULL -> GPIO Low; otherwise -> GPIO High
#ifdef CHARGER_CONNECTED_PIN
                ////{CHARGER_CONNECTED_PIN, ACTIVE_BOTH, NRF_GPIO_PIN_NOPULL ,function_charger_event_handler},
                {CHARGER_CONNECTED_PIN, APP_BUTTON_ACTIVE_HIGH, NRF_GPIO_PIN_NOPULL ,function_charger_event_handler},
#endif
#ifdef CHARGER_CHARGING_PIN
                ////{CHARGER_CHARGING_PIN, ACTIVE_BOTH, NRF_GPIO_PIN_NOPULL ,function_chargingcomplete_event_handler},
                {CHARGER_CHARGING_PIN, APP_BUTTON_ACTIVE_HIGH, NRF_GPIO_PIN_NOPULL ,function_chargingcomplete_event_handler},
#endif
            };
////        APP_BUTTON_INIT(buttons, sizeof(buttons) / sizeof(buttons[0]), BUTTON_DETECTION_DELAY, false);
        app_button_init(buttons, sizeof(buttons) / sizeof(buttons[0]), BUTTON_DETECTION_DELAY);
    } else {
        static app_button_cfg_t buttons[] = {
                //charger GPIO init
                //Charger: Plug In -> GPIO Low; Plug Out -> GPIO High
                //ChargingComplete: FULL -> GPIO Low; otherwise -> GPIO High
#ifdef CHARGER_CONNECTED_PIN
                ////{CHARGER_CONNECTED_PIN, ACTIVE_BOTH, NRF_GPIO_PIN_NOPULL ,function_charger_event_handler},
            {CHARGER_CONNECTED_PIN, APP_BUTTON_ACTIVE_HIGH, NRF_GPIO_PIN_NOPULL ,function_charger_event_handler},
#endif
#ifdef CHARGER_CHARGING_PIN
                ////{CHARGER_CHARGING_PIN, ACTIVE_BOTH, NRF_GPIO_PIN_NOPULL ,function_chargingcomplete_event_handler},
            {CHARGER_CHARGING_PIN, APP_BUTTON_ACTIVE_HIGH, NRF_GPIO_PIN_NOPULL ,function_chargingcomplete_event_handler},
#endif
            };
        ////APP_BUTTON_INIT(buttons, sizeof(buttons) / sizeof(buttons[0]), BUTTON_DETECTION_DELAY, false);
            app_button_init(buttons, sizeof(buttons) / sizeof(buttons[0]), BUTTON_DETECTION_DELAY);
    }
    button_init(event_button_fun);
    //GPIO_WAKEUP_BUTTON_CONFIG(FUNCTION_BUTTON);
}


/**@brief LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static void leds_init(void)
{
    nrf_gpio_cfg_output(IDEA_LED_0);
    nrf_gpio_cfg_output(IDEA_LED_1);
    nrf_gpio_cfg_output(IDEA_LED_2);
    nrf_gpio_cfg_output(IDEA_LED_3);
    nrf_gpio_cfg_output(IDEA_LED_4);

    nrf_gpio_pin_set(IDEA_LED_0);
    nrf_gpio_pin_set(IDEA_LED_1);
    nrf_gpio_pin_set(IDEA_LED_2);
    nrf_gpio_pin_set(IDEA_LED_3);
    nrf_gpio_pin_set(IDEA_LED_4); 
}

void ota_time_out_handle(void * context);

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
////    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, true);
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, NULL);
}

/**@brief Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
extern app_timer_id_t m_erase_flash_timer_id;

extern app_timer_id_t m_double_tap_timer_id;

static void timers_create(void)
{
    uint32_t err_code;


#ifdef HAS_BUTTON

    err_code = app_timer_create(&btn_identi, APP_TIMER_MODE_SINGLE_SHOT, btn_timeout_handler);
    APP_ERROR_CHECK(err_code);
#endif

    err_code = led_flash_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_sensor_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                sensor_timer_handle);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_double_tap_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                tap_timer_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&ota_delay_timer,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                ota_time_out_handle);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_timer_create(&one_second_delay_timer,
                                APP_TIMER_MODE_REPEATED,
                                one_second_time_out_handle);
    APP_ERROR_CHECK(err_code);
    app_timer_start(one_second_delay_timer,ONE_SECOND_TIMER,NULL);

    create_send_data_timer();

    err_code = motor_control_framework_init();
    APP_ERROR_CHECK(err_code);

}

/**@brief Event Scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


void reset_check(void)
{

    uint32_t page_num;

    DEBUG_PRINT("RESETREAS:%x; POWER_RESETREAS_RESETPIN_Msk:%x",NRF_POWER->RESETREAS,POWER_RESETREAS_RESETPIN_Msk);
    if(NRF_POWER->RESETREAS & POWER_RESETREAS_RESETPIN_Msk) {
        DEBUG_PRINT("Must Erase All User Data!!!!");
        NRF_POWER->RESETREAS = POWER_RESETREAS_RESETPIN_Msk;

#ifndef INTERNAL_DEV
        //clear data block
        page_num = DATA_BLOCK_START_PAGE;
        for( ; page_num < (DATA_BLOCK_START_PAGE + PAGE_NUM_FOR_DATA); page_num ++) {
            ble_flash_page_erase(page_num);
        }

        //clear config block
        page_num = CONFIG_BLOCK_START_PAGE;
        for( ; page_num < (CONFIG_BLOCK_START_PAGE + PAGE_NUM_FOR_CONFIG); page_num++ ) {
            if(page_num != FLASH_PAGE_SN_FACTORY_FLAG) { // SN page should not be erased
                ble_flash_page_erase(page_num);
            }
        }
#endif

        /*        if(!charger_connected()){
                    hal_acc_PowerDown();
                    power_wfi();
                }
        */
    }
    DEBUG_PRINT("RESETREAS:%x; POWER_RESETREAS_RESETPIN_Msk:%x",NRF_POWER->RESETREAS,POWER_RESETREAS_RESETPIN_Msk);


    //notice wakeup
    nrf_gpio_pin_clear(IDEA_LED_0);
    nrf_delay_ms(400);
    nrf_gpio_pin_set(IDEA_LED_0);
}


void gpio_init()
{
    uint32_t i = 0;
    for(i = 0; i< 32 ; ++i ) {
        nrf_gpio_cfg_default(i);
    }
}

/**@brief Function for configuring the Low Frequency Clock on the nRF51 Series.
 *        It will return when the clock has started.
 *
 */
static void lfclk_config(void)
{
    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}

static void Clock_1s_cb(void)
{
    Ui_menu_1s();
////    Event_input_1s();
}

void init1()
{
    
#if DEBUG_INFO_ENABLE || DEBUG_ERROR_ENABLE || DEBUG_PRINT_ENABLE || DEBUG_FUN_ENABLE
    simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);
    DEBUG_PRINT(" init1 uart inited\n");
    //simple_uart_putstring((const uint8_t *)"init1 uart inited\r\n");
#endif

    leds_init();
    Ui_menu_init();
    motor_init();
    lfclk_config();
    timers_init();
    scheduler_init();
    charger_init();

    DEBUG_PRINT("charger_init inited\r\n");
    // simple_uart_putstring((const uint8_t *)"charger_init inited\r\n");

#ifndef ADC_WRONG

    battery_adc_dev_init();
    battery_measure_init();
#endif
#ifndef USE_DVK_6310
    hal_acc_init();
#endif
}

void init2()
{
    uint32_t err_code;

    DEBUG_PRINT("init2 inited\r\n");
    //simple_uart_putstring((const uint8_t *)"init2 uart inited\r\n");

    wdt_init();
    
    timers_create();
    gpiote_init();
    system_clock_init();

    //init ble stack
////    bond_manager_init();
    ble_stack_init();
    gap_params_init();
    services_init();
////    advertising_init(BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE);
    advertising_init();    
    conn_params_init();
    sec_params_init();
    radio_notification_init();
    set_global_bluetooth_status(NotWork);

    gpiote_int_init();
    charger_framework_init();

    //init bluetooth communicate protocol
    bluetooth_l0_init();
    err_code = bond_read_user_id();

    if(err_code == NRF_SUCCESS) {
        set_device_has_bonded(true);
        DEBUG_ERROR("bond crc check success\r\n");
    } else {
        set_device_has_bonded(false);
        DEBUG_ERROR("bond check fail!!!!\r\n");
    }

#ifndef USE_DVK_6310
    hal_acc_enable();
#endif
    
    err_code = Clock_cb_reg(Clock_1s_cb);
    if (err_code != NRF_SUCCESS) {
        DEBUG_PRINT("Clock_cb_reg: 1s error ..."); 
    }
    
    
#ifdef SPI_FLASH
    Spi_Flash_Init();
////    SPIFlash_Erase_Sector(0,0);
////    nrf_delay_ms(1000);
////    SpiFlash_Write_Data("HEL",0,0,0,3);
////    nrf_delay_ms(100);
    SpiFlash_Read_Data(0 ,0 ,0 ,3);

#endif      // #ifdef SPI_FLASH

#ifdef OLED
    OLED_Init();            //初始化OLED
    OLED_Clear();
    OLED_ShowString(0,0,"HELLO",7);
//////    OLED_DrawBMP(0,0,64,4,BMP1);
#endif          // #ifdef OLED


}

#ifdef FEATURE_PRE_INIT
/** RTC0 interrupt handler.
 * Triggered on TICK and COMPARE0 match.
 */
void RTC0_IRQHandler()
{
    NRF_RTC0->EVENTS_COMPARE[0] = 0;
    NRF_RTC0->TASKS_STOP = 1;
    NRF_RTC0->INTENCLR = RTC_INTENSET_COMPARE0_Msk;
    NVIC_DisableIRQ(RTC0_IRQn);
}

#define LFCLK_FREQUENCY           (32768UL)                 /*!< LFCLK frequency in Hertz, constant */
#define RTC_FREQUENCY             (8UL)                     /*!< required RTC working clock RTC_FREQUENCY Hertz. Changable */
#define COMPARE_COUNTERTIME       (1UL)                     /*!< Get Compare event COMPARE_TIME seconds after the counter starts from 0 */
#define COUNTER_PRESCALER         ((LFCLK_FREQUENCY/RTC_FREQUENCY) - 1)  /* f = LFCLK/(prescaler + 1) */

/** Configures the RTC with TICK for 100Hz and COMPARE0 to 10 sec
 */
static void wakeup_rtc_config(void)
{
    NVIC_EnableIRQ(RTC0_IRQn);                                 // Enable Interrupt for the RTC in the core
    NRF_RTC0->PRESCALER = COUNTER_PRESCALER;                   // Set prescaler to a TICK of RTC_FREQUENCY
    NRF_RTC0->CC[0] = COMPARE_COUNTERTIME * RTC_FREQUENCY;     // Compare0 after approx COMPARE_COUNTERTIME seconds

    // Enable COMPARE0 event and COMPARE0 interrupt:
    //  NRF_RTC0->EVTENSET = RTC_EVTENSET_COMPARE0_Msk;
    NRF_RTC0->INTENSET = RTC_INTENSET_COMPARE0_Msk;
    NRF_RTC0->TASKS_START = 1;
}

void pre_init()
{
    //NRF_GPIO->DIRSET = 0;
    lfclk_config();
    wakeup_rtc_config();
    __WFI();
}
#endif      // #ifdef FEATURE_PRE_INIT

/**@brief Application main function.
 */
void start_click_algorithm(void);

/**@brief Application main function.
 */
int main(void)
{
    // 0. init all gpio to reduce power consumption
    gpio_init();            // 初始化全部IO口为初始状态（输入、关上拉、关复用）
    
    // 1. wait for power suply to be stable
#ifdef FEATURE_PRE_INIT
    pre_init();             // 初始化时钟与RTC0并进行待机，待RTC0中断唤醒。用此方式待电源稳定
#endif  // #ifdef FEATURE_PRE_INIT
    
    // 2. first stage initialize
    init1();                // 初始化LED、MOTOR、TIMER、ADC、G-SENSOR等硬件。初始化TIMER、系统调度等相关服务
    
#ifndef ADC_WRONG
    // 3. pre-charging
//////////    if(!voltage_detect_n_precharging()) {
//////////        power_system_off();
//////////    }
#endif
    
    DEBUG_PRINT("voltage_detect_n_precharging ed\n");
    //  simple_uart_putstring((const uint8_t *)"\t\t[main]voltage_detect_n_precharging ed\r\n");
    
    // 4. Bootup Check
    bootup_check();
    
    DEBUG_PRINT("bootup_check ed\n");
    // simple_uart_putstring((const uint8_t *)"\t\t[main]bootup_check ed\r\n");
    
    // 5. WFI
    reset_check();
    
    DEBUG_PRINT("reset_check ed\n");
    //simple_uart_putstring((const uint8_t *)"\t\t[main]reset_check ed\r\n");
    
    // 6. second stage initialize
    init2();
    

    uint32_t err_code;
    err_code = restore_backup_info_from_flash();
    if(err_code == NRF_SUCCESS) {
        restore_SleepSettingsFromFlash();
        // if has profile, start step counter after set time
        if(check_has_bonded()) {
            DEBUG_PRINT(" restart_health_algorithm \n");

            load_alarm();
            load_user_profile();
            load_daily_target();
            restart_health_algorithm();
        }
    }
    else {
        DEBUG_PRINT(" no time read from flash\n");
        //   simple_uart_putstring((const uint8_t *) "\t\t[main] no time read from flash\r\n");
    }
    
    check_need_erase_sleep_page_for_when_boot();
    check_need_erase_sport_page_for_when_boot();
    // check the format of data in flash
    check_sports_data_in_flash();
    check_sleep_data_in_flash();


    start_click_algorithm();

    sensor_timers_start();

    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);

    DEBUG_PRINT("advertising_start ed\n");
    
    //always enable button event
    app_button_enable();
//    battery_measure_timer_start(BATTERY_LEVEL_MEAS_INTERVAL);

    wdt_start();
    
    // Enter main loop.
    for (;;)
    {
        app_sched_execute();
        wdt_feed();
        power_manage();
        DEBUG_PRINT(".");
    }
}


/** 
 * @}
 */
