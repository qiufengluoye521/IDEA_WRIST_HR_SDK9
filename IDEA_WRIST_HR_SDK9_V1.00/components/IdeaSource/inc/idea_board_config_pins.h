#ifndef IDEA_BOARD_CONFIG_H__
#define IDEA_BOARD_CONFIG_H__

#include "stdint.h"


// Device Information

//#define DEVICE_NAME       "iCre Band"   /**< Name of device. Will be included in the advertising data. */
#define DEVICE_NAME       "Boom Band"   /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME "Baidu Inc"   /**< Manufacturer. Will be passed to Device Information Service. */
#define FW_REV_STR        "1.0.0"
#define MODULE_NUM        "du100"     /**< Model Number String. */
#define SERIAL_NUM        "0123456789abcde"   /**< Serial Number String. */
#define HW_REV_STR        "1.0.0"     /**< Hardware Revision String. */

#define BATTERY_VOLTAGE_ADJUSTMENT              60     /**< Adjustment for charging */

#define USE_HEART_RATE                          1       /*< is use heart_rate or not*/

#define FEATURE_BUTTON
#define FEATURE_LED_PROGRESS
//#define FEATURE_LED_CLOCK
#define FEATURE_MOTOR
//#define FEATURE_MOTOR_LRA
//#define FEATURE_TWI
#define FEATURE_SENSOR_LIS3DH
#define FEATURE_PRE_INIT
//#define FEATURE_AUTO_SLEEP
#define FEATURE_AUTO_WAKEUP
#define USER_BUTTON0

/*********************************************************************
* pre_init feature control 
**********************************************************************/


//GPIO pin number for LEDs
#ifdef FEATURE_LED_PROGRESS
  #define IDEA_LED_0           6
  #define IDEA_LED_1           6 
  #define IDEA_LED_2           6
  #define IDEA_LED_3           6
  #define IDEA_LED_4           6
#endif

//GPIO pin number for MOTOR
#ifdef FEATURE_MOTOR
  #define BAIDU_MOTOR_0         30
#endif
//GPIO pin number for LRA
#ifdef FEATURE_MOTOR_LRA
  #define LRA_EN_PIN      13 /*!< GPIO pin number for LRA_EN           */
  #define LRA_PWM_PIN     14 /*!< GPIO pin number for LRA_PWM           */
  #define LRA_SCL_PIN     15 /*!< GPIO pin number for LRA_SCL           */
  #define LRA_SDA_PIN     16 /*!< GPIO pin number for LRA_SDA           */

//I2C define
#define TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER LRA_SCL_PIN
  #define TWI_MASTER_CONFIG_DATA_PIN_NUMBER LRA_SDA_PIN
#endif

//GPIO pin number for BUTTON
#ifdef FEATURE_BUTTON
  #define IDEA_BUTTON_0  0       /*P0.30 */
#endif

//XXX
#define ADVERTISING_LED_PIN_NO      IDEA_LED_0
#define CONNECTED_LED_PIN_NO        IDEA_LED_1
#define ASSERT_LED_PIN_NO           IDEA_LED_2

//GPIO pin number for Charging
#define CHARGER_CONNECTED_PIN 25
#define CHARGER_CHARGING_PIN  21  /*!< GPIO pin number for Charging State           */

/* GPIO pin number for SPI */
#define SPI_PSELSCK0              18   /*!< GPIO pin number for SPI clock (note that setting this to 31 will only work for loopback purposes as it not connected to a pin) */
#define SPI_PSELMOSI0             16   /*!< GPIO pin number for Master Out Slave In    */
#define SPI_PSELMISO0             15   /*!< GPIO pin number for Master In Slave Out    */
#define SPI_PSELSS0               14   /*!< GPIO pin number for Slave Select           */

/* GPIO pin number for LIS3DH */
#ifdef FEATURE_SENSOR_LIS3DH
  #define LIS3DH_INT1     8 /*!< GPIO pin number for LIS3DH_INT1           */
  #define LIS3DH_INT2     9 /*!< GPIO pin number for LIS3DH_INT2          */
#endif


/*****************************************************************
* uart pin config
******************************************************************/
#if defined(DEBUG_LOG) || defined (DEBUG_ACC) || defined (DEBUG_PHILL)

#define RX_PIN_NUMBER   11    // UART RX pin number.
#define TX_PIN_NUMBER   17    // UART TX pin number.
//the follow pin is useless
#define CTS_PIN_NUMBER 43    // UART Clear To Send pin number. Not used if HWFC is set to false
#define RTS_PIN_NUMBER 43    // Not used if HWFC is set to false
#define HWFC           false // UART hardware flow control

#endif  //DEBUG_LOG

#endif  // IDEA_BOARD_CONFIG_PINS_H__
