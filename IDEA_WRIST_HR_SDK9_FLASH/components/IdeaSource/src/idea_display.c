#include "idea_display.h"
#include "simple_uart.h"
#include "idea_oled.h"
#include "idea_sw_flash.h"
#include "idea_board_config_pins.h"
#include "idea_wall_clock_timer.h"
//#include "oledfont.h"
#include "idea_data_manager.h"
#include "step_counter.h"


extern uint8_t spim_dma_rxbuf[260];
static bool Display_colon_on                                = false;
static bool Display_heart_rate                              = false;

void Display_show_time_mode(void)
{
    UTCTimeStruct * current_time;
    current_time = get_wall_clock_time();
    OLED_Clear();
    SpiFlash_Read_Data(0,SPI_FLASH_PAGE_NUM_BATTERY_BLUETH,0,256);
    OLED_DrawBMP(1,0,8,0,&spim_dma_rxbuf[60+165]);                                                  // 蓝牙标志
    if(current_time->month >= 10)                                                                   // 月份是两位数
    {
        OLED_DrawBMP(11,0,15,0,&spim_dma_rxbuf[(current_time->month/10)*5]);                        // 字符0
        OLED_DrawBMP(17,0,21,0,&spim_dma_rxbuf[(current_time->month%10)*5]);                        // 字符4
    } else {                                                                                        // 月份是一位数
        OLED_DrawBMP(11,0,15,0,&spim_dma_rxbuf[0*5]);                                               // 字符0
        OLED_DrawBMP(17,0,21,0,&spim_dma_rxbuf[(current_time->month%10)*5]);                        // 字符4
    }
    OLED_DrawBMP(23,0,27,0,&spim_dma_rxbuf[10*5]);                                                  // 字符 '/'
    
    if(current_time->day >= 10)                                                                     // 日是两位数
    {
        OLED_DrawBMP(29,0,33,0,&spim_dma_rxbuf[(current_time->day/10)*5]);                          // 字符0
        OLED_DrawBMP(35,0,39,0,&spim_dma_rxbuf[(current_time->day%10)*5]);                          // 字符4
    } else {                                                                                        // 日是一位数
        OLED_DrawBMP(29,0,33,0,&spim_dma_rxbuf[0*5]);                                               // 字符0
        OLED_DrawBMP(35,0,39,0,&spim_dma_rxbuf[(current_time->day%10)*5]);                          // 字符4
    }
    OLED_DrawBMP(45,0,59,0,&spim_dma_rxbuf[60]);                                                    // battery
    
    
    SpiFlash_Read_Data(0,SPI_FLASH_PAGE_NUM_0824,0,256);
    if(current_time->hour >= 10)                                                                    // hour是两位数
    {
        OLED_DrawBMP(1,1,8,1,&spim_dma_rxbuf[(current_time->hour/10)*8+0]);                         // hour 1.1
        OLED_DrawBMP(1,2,8,2,&spim_dma_rxbuf[(current_time->hour/10)*8+85*1]);                      // hour 1.2
        OLED_DrawBMP(1,3,8,3,&spim_dma_rxbuf[(current_time->hour/10)*8+85*2]);                      // hour 1.3
        
        OLED_DrawBMP(11,1,18,1,&spim_dma_rxbuf[(current_time->hour%10)*8]);                         // hour 2.1--0
        OLED_DrawBMP(11,2,18,2,&spim_dma_rxbuf[(current_time->hour%10)*8+85*1]);                    // hour 2.2
        OLED_DrawBMP(11,3,18,3,&spim_dma_rxbuf[(current_time->hour%10)*8+85*2]);                    // hour 2.3
    } else {
        OLED_DrawBMP(1,1,8,1,&spim_dma_rxbuf[0*8+0]);                                               // hour 1.1
        OLED_DrawBMP(1,2,8,2,&spim_dma_rxbuf[0*8+85*1]);                                            // hour 1.2
        OLED_DrawBMP(1,3,8,3,&spim_dma_rxbuf[0*8+85*2]);                                            // hour 1.3
        
        OLED_DrawBMP(11,1,18,1,&spim_dma_rxbuf[(current_time->hour%10)*8]);                         // hour 2.1--0
        OLED_DrawBMP(11,2,18,2,&spim_dma_rxbuf[(current_time->hour%10)*8+85*1]);                    // hour 2.2
        OLED_DrawBMP(11,3,18,3,&spim_dma_rxbuf[(current_time->hour%10)*8+85*2]);                    // hour 2.3
    }
    Display_colon_on = true;
    OLED_DrawBMP(21,1,25,1,&spim_dma_rxbuf[10*8]);                                                  // : 
    OLED_DrawBMP(21,2,25,2,&spim_dma_rxbuf[10*8+85*1]);                                             // : 
    OLED_DrawBMP(21,3,25,3,&spim_dma_rxbuf[10*8+85*2]);                                             // : 
    
    if(current_time->minutes >= 10)
    {
        OLED_DrawBMP(26,1,33,1,&spim_dma_rxbuf[(current_time->minutes/10)*8]);                      // hour 2.1--5
        OLED_DrawBMP(26,2,33,2,&spim_dma_rxbuf[(current_time->minutes/10)*8+85*1]);                 // hour 2.2
        OLED_DrawBMP(26,3,33,3,&spim_dma_rxbuf[(current_time->minutes/10)*8+85*2]);                 // hour 2.3
        
        OLED_DrawBMP(36,1,43,1,&spim_dma_rxbuf[(current_time->minutes%10)*8]);                      // hour 2.1--6
        OLED_DrawBMP(36,2,43,2,&spim_dma_rxbuf[(current_time->minutes%10)*8+85*1]);                 // hour 2.2
        OLED_DrawBMP(36,3,43,3,&spim_dma_rxbuf[(current_time->minutes%10)*8+85*2]);     
    } else {
        OLED_DrawBMP(26,1,33,1,&spim_dma_rxbuf[0*8]);                                               // hour 2.1--5
        OLED_DrawBMP(26,2,33,2,&spim_dma_rxbuf[0*8+85*1]);                                          // hour 2.2
        OLED_DrawBMP(26,3,33,3,&spim_dma_rxbuf[0*8+85*2]);                                          // hour 2.3
        
        OLED_DrawBMP(36,1,43,1,&spim_dma_rxbuf[(current_time->minutes%10)*8]);                      // hour 2.1--6
        OLED_DrawBMP(36,2,43,2,&spim_dma_rxbuf[(current_time->minutes%10)*8+85*1]);                 // hour 2.2
        OLED_DrawBMP(36,3,43,3,&spim_dma_rxbuf[(current_time->minutes%10)*8+85*2]);                 // hour 2.3
    }
    
}

bool Display_colon_on_statues(void)
{
    return Display_colon_on;
}

bool Display_heart_rate_statues(void)
{
    return Display_heart_rate;
}

void refresh_time(void)
{
    // 显示时间界面最多只显示4s，可认为4s内hour 和 minutes都不变 ，只刷新":"
    if(Display_colon_on_statues())                                          // 若显示":",刷新为空
    {
        Display_colon_on = false;
        OLED_Clear_xy(21,1,25,3);
//        OLED_DrawBMP(21,1,25,2,&spim_dma_rxbuf[10*8]);                                                  // : 
//        OLED_DrawBMP(21,2,25,3,&spim_dma_rxbuf[10*8+85*1]);                                             // : 
//        OLED_DrawBMP(21,3,25,4,&spim_dma_rxbuf[10*8+85*2]);                                             // : 
    } else {                                                                // 若不显示":",刷新为":"
        Display_colon_on = true;
        show_colon();
    }
}

#if USE_HEART_RATE
void Display_show_hr_mode(void)
{
    uint8_t current_heart_rate = 65;
    
    OLED_Clear();
    
    // step 数字
    SpiFlash_Read_Data(0,SPI_FLASH_PAGE_NUM_0824,0,256);
    OLED_DrawBMP(37,1,44,1,&spim_dma_rxbuf[((current_heart_rate / 100)%10)*8+85*0]);                // 3
    OLED_DrawBMP(37,2,44,2,&spim_dma_rxbuf[((current_heart_rate / 100)%10)*8+85*1]);                // 3
    OLED_DrawBMP(37,3,44,3,&spim_dma_rxbuf[((current_heart_rate / 100)%10)*8+85*2]);                // 3
    
    OLED_DrawBMP(45,1,52,1,&spim_dma_rxbuf[((current_heart_rate / 10)%10)*8+85*0]);                // 4
    OLED_DrawBMP(45,2,52,2,&spim_dma_rxbuf[((current_heart_rate / 10)%10)*8+85*1]);                // 4
    OLED_DrawBMP(45,3,52,3,&spim_dma_rxbuf[((current_heart_rate / 10)%10)*8+85*2]);                // 4
    
    OLED_DrawBMP(53,1,60,1,&spim_dma_rxbuf[(current_heart_rate %10)*8+85*0]);                      // 5
    OLED_DrawBMP(53,2,60,2,&spim_dma_rxbuf[(current_heart_rate %10)*8+85*1]);                      // 5
    OLED_DrawBMP(53,3,60,3,&spim_dma_rxbuf[(current_heart_rate %10)*8+85*2]);                      // 5
    
    // step 图标
    SpiFlash_Read_Data(0,SPI_FLASH_PAGE_HATERATE,0,256);
    Display_heart_rate = true;
    OLED_DrawBMP(0,0,31,0,&spim_dma_rxbuf[32+0]);
    OLED_DrawBMP(0,1,31,1,&spim_dma_rxbuf[32+64*1]);
    OLED_DrawBMP(0,2,31,2,&spim_dma_rxbuf[32+64*2]);
    OLED_DrawBMP(0,3,31,3,&spim_dma_rxbuf[32+64*3]);
    
}

void refresh_heart_rate(void)
{
    if(Display_heart_rate_statues())
    {
        Display_heart_rate = false;
        OLED_DrawBMP(0,0,31,0,&spim_dma_rxbuf[0]);
        OLED_DrawBMP(0,1,31,1,&spim_dma_rxbuf[64*1]);
        OLED_DrawBMP(0,2,31,2,&spim_dma_rxbuf[64*2]);
        OLED_DrawBMP(0,3,31,3,&spim_dma_rxbuf[64*3]);
    } else {
        Display_heart_rate = true;
        OLED_DrawBMP(0,0,31,0,&spim_dma_rxbuf[32+0]);
        OLED_DrawBMP(0,1,31,1,&spim_dma_rxbuf[32+64*1]);
        OLED_DrawBMP(0,2,31,2,&spim_dma_rxbuf[32+64*2]);
        OLED_DrawBMP(0,3,31,3,&spim_dma_rxbuf[32+64*3]);
    }
}
#endif          // #if USE_HEART_RATE

void Display_show_step_mode(void)
{
    uint32_t current_steps = 0;
    uint32_t target_steps = 0;
    uint8_t step_percentage;
    
    current_steps = get_global_step_counts_today();
    target_steps = get_daily_target();
    step_percentage = (uint8_t)(current_steps * 100 / target_steps);
    
    OLED_Clear();
    // step 图标
    SpiFlash_Read_Data(0,SPI_FLASH_PAGE_STEP_DISTANCE_CALORIES,0,256);
    OLED_DrawBMP(0,0,19,0,&spim_dma_rxbuf[0]);
    OLED_DrawBMP(0,1,19,1,&spim_dma_rxbuf[64*1]);
    OLED_DrawBMP(0,2,19,2,&spim_dma_rxbuf[64*2]);
    OLED_DrawBMP(0,3,19,3,&spim_dma_rxbuf[64*3]);
    
    // 百分比
    SpiFlash_Read_Data(0,SPI_FLASH_PAGE_NUM_BATTERY_BLUETH,0,60);
    OLED_DrawBMP(30,0,34,0,&spim_dma_rxbuf[((step_percentage/100)%10)*5]);                          // 百位
    OLED_DrawBMP(35,0,39,0,&spim_dma_rxbuf[((step_percentage/10)%10)*5]);                           // 十位
    OLED_DrawBMP(40,0,44,0,&spim_dma_rxbuf[((step_percentage/1)%10)*5]);                            // 个位
    OLED_DrawBMP(45,0,49,0,&spim_dma_rxbuf[11*5]);                                                  // %
    
    // step 数字
    SpiFlash_Read_Data(0,SPI_FLASH_PAGE_NUM_0824,0,256);
    OLED_DrawBMP(21,1,28,1,&spim_dma_rxbuf[(current_steps / 10000)*8+85*0]);                    // 1
    OLED_DrawBMP(21,2,28,2,&spim_dma_rxbuf[(current_steps / 10000)*8+85*1]);                    // 1
    OLED_DrawBMP(21,3,28,3,&spim_dma_rxbuf[(current_steps / 10000)*8+85*2]);                    // 1
    
    OLED_DrawBMP(29,1,36,1,&spim_dma_rxbuf[((current_steps / 1000)%10)*8+85*0]);                // 2
    OLED_DrawBMP(29,2,36,2,&spim_dma_rxbuf[((current_steps / 1000)%10)*8+85*1]);                // 2
    OLED_DrawBMP(29,3,36,3,&spim_dma_rxbuf[((current_steps / 1000)%10)*8+85*2]);                // 2
    
    OLED_DrawBMP(37,1,44,1,&spim_dma_rxbuf[((current_steps / 100)%10)*8+85*0]);                // 3
    OLED_DrawBMP(37,2,44,2,&spim_dma_rxbuf[((current_steps / 100)%10)*8+85*1]);                // 3
    OLED_DrawBMP(37,3,44,3,&spim_dma_rxbuf[((current_steps / 100)%10)*8+85*2]);                // 3
    
    OLED_DrawBMP(45,1,52,1,&spim_dma_rxbuf[((current_steps / 10)%10)*8+85*0]);                // 4
    OLED_DrawBMP(45,2,52,2,&spim_dma_rxbuf[((current_steps / 10)%10)*8+85*1]);                // 4
    OLED_DrawBMP(45,3,52,3,&spim_dma_rxbuf[((current_steps / 10)%10)*8+85*2]);                // 4
    
    OLED_DrawBMP(53,1,60,1,&spim_dma_rxbuf[(current_steps %10)*8+85*0]);                      // 5
    OLED_DrawBMP(53,2,60,2,&spim_dma_rxbuf[(current_steps %10)*8+85*1]);                      // 5
    OLED_DrawBMP(53,3,60,3,&spim_dma_rxbuf[(current_steps %10)*8+85*2]);                      // 5
    
}

void Display_show_calories_mode(void)
{
    uint32_t current_calory = 0;
    current_calory = get_calory();
    
    OLED_Clear();
    // step 图标
    SpiFlash_Read_Data(0,SPI_FLASH_PAGE_STEP_DISTANCE_CALORIES,0,256);
    OLED_DrawBMP(0,0,19,0,&spim_dma_rxbuf[40+0]);
    OLED_DrawBMP(0,1,19,1,&spim_dma_rxbuf[40+64*1]);
    OLED_DrawBMP(0,2,19,2,&spim_dma_rxbuf[40+64*2]);
    OLED_DrawBMP(0,3,19,3,&spim_dma_rxbuf[40+64*3]);
    
    // step 数字
    SpiFlash_Read_Data(0,SPI_FLASH_PAGE_NUM_0824,0,256);
    OLED_DrawBMP(21,1,28,1,&spim_dma_rxbuf[(current_calory / 10000)*8+85*0]);                    // 1
    OLED_DrawBMP(21,2,28,2,&spim_dma_rxbuf[(current_calory / 10000)*8+85*1]);                    // 1
    OLED_DrawBMP(21,3,28,3,&spim_dma_rxbuf[(current_calory / 10000)*8+85*2]);                    // 1
    
    OLED_DrawBMP(29,1,36,1,&spim_dma_rxbuf[((current_calory / 1000)%10)*8+85*0]);                // 2
    OLED_DrawBMP(29,2,36,2,&spim_dma_rxbuf[((current_calory / 1000)%10)*8+85*1]);                // 2
    OLED_DrawBMP(29,3,36,3,&spim_dma_rxbuf[((current_calory / 1000)%10)*8+85*2]);                // 2
    
    OLED_DrawBMP(37,1,44,1,&spim_dma_rxbuf[((current_calory / 100)%10)*8+85*0]);                // 3
    OLED_DrawBMP(37,2,44,2,&spim_dma_rxbuf[((current_calory / 100)%10)*8+85*1]);                // 3
    OLED_DrawBMP(37,3,44,3,&spim_dma_rxbuf[((current_calory / 100)%10)*8+85*2]);                // 3
    
    OLED_DrawBMP(45,1,52,1,&spim_dma_rxbuf[((current_calory / 10)%10)*8+85*0]);                // 4
    OLED_DrawBMP(45,2,52,2,&spim_dma_rxbuf[((current_calory / 10)%10)*8+85*1]);                // 4
    OLED_DrawBMP(45,3,52,3,&spim_dma_rxbuf[((current_calory / 10)%10)*8+85*2]);                // 4
    
    OLED_DrawBMP(53,1,60,1,&spim_dma_rxbuf[(current_calory %10)*8+85*0]);                      // 5
    OLED_DrawBMP(53,2,60,2,&spim_dma_rxbuf[(current_calory %10)*8+85*1]);                      // 5
    OLED_DrawBMP(53,3,60,3,&spim_dma_rxbuf[(current_calory %10)*8+85*2]);                      // 5
    
    
}

void Display_show_distance_mode(void)
{
    uint8_t current_stride = 0;
    uint32_t current_steps = 0;
    uint32_t current_distance = 0;
    
    current_steps       = get_global_step_counts_today();
    //current_stride      = get_user_stride();
    current_stride      = 70;
    current_distance    = (current_steps * current_stride) / 100;                           // cm->m
    DEBUG_ERROR("current_steps is:%d,current_stride is:%d,current_distance is %d\r\n",current_steps,current_stride,current_distance);
    OLED_Clear();
    // distance 图标
    SpiFlash_Read_Data(0,SPI_FLASH_PAGE_STEP_DISTANCE_CALORIES,0,256);
    OLED_DrawBMP(0,0,19,0,&spim_dma_rxbuf[0+20]);
    OLED_DrawBMP(0,1,19,1,&spim_dma_rxbuf[64*1+20]);
    OLED_DrawBMP(0,2,19,2,&spim_dma_rxbuf[64*2+20]);
    OLED_DrawBMP(0,3,19,3,&spim_dma_rxbuf[64*3+20]);
    
    // step 数字
    SpiFlash_Read_Data(0,SPI_FLASH_PAGE_NUM_0824,0,256);
    OLED_DrawBMP(21,1,28,1,&spim_dma_rxbuf[(current_distance / 10000)*8+85*0]);                    // 1
    OLED_DrawBMP(21,2,28,2,&spim_dma_rxbuf[(current_distance / 10000)*8+85*1]);                    // 1
    OLED_DrawBMP(21,3,28,3,&spim_dma_rxbuf[(current_distance / 10000)*8+85*2]);                    // 1
    
    OLED_DrawBMP(29,1,36,1,&spim_dma_rxbuf[((current_distance / 1000)%10)*8+85*0]);                // 2
    OLED_DrawBMP(29,2,36,2,&spim_dma_rxbuf[((current_distance / 1000)%10)*8+85*1]);                // 2
    OLED_DrawBMP(29,3,36,3,&spim_dma_rxbuf[((current_distance / 1000)%10)*8+85*2]);                // 2
    
    OLED_DrawBMP(37,1,44,1,&spim_dma_rxbuf[((current_distance / 100)%10)*8+85*0]);                // 3
    OLED_DrawBMP(37,2,44,2,&spim_dma_rxbuf[((current_distance / 100)%10)*8+85*1]);                // 3
    OLED_DrawBMP(37,3,44,3,&spim_dma_rxbuf[((current_distance / 100)%10)*8+85*2]);                // 3
    
    OLED_DrawBMP(45,1,52,1,&spim_dma_rxbuf[((current_distance / 10)%10)*8+85*0]);                // 4
    OLED_DrawBMP(45,2,52,2,&spim_dma_rxbuf[((current_distance / 10)%10)*8+85*1]);                // 4
    OLED_DrawBMP(45,3,52,3,&spim_dma_rxbuf[((current_distance / 10)%10)*8+85*2]);                // 4
    
    OLED_DrawBMP(53,1,60,1,&spim_dma_rxbuf[(current_distance %10)*8+85*0]);                      // 5
    OLED_DrawBMP(53,2,60,2,&spim_dma_rxbuf[(current_distance %10)*8+85*1]);                      // 5
    OLED_DrawBMP(53,3,60,3,&spim_dma_rxbuf[(current_distance %10)*8+85*2]);                      // 5
    
    //OLED_Clear();
    //SpiFlash_Read_Data(0,SPI_FLASH_PAGE_DISTANCE,0,256);
    //OLED_DrawBMP(0,0,63,4,spim_dma_rxbuf);
}

void Display_show_charging_mode(uint8_t val)
{
    if(100 == val)
    {
        OLED_Clear();
        SpiFlash_Read_Data(0,7,0,256);
        OLED_DrawBMP(0,0,63,4,spim_dma_rxbuf);
    } else {
        OLED_Clear();
        SpiFlash_Read_Data(0,6,0,256);
        OLED_DrawBMP(0,0,63,4,spim_dma_rxbuf);
    }
}

void Display_show_low_power_mode(void)
{
    OLED_Clear();
    SpiFlash_Read_Data(0,5,0,256);
    OLED_DrawBMP(0,0,63,4,spim_dma_rxbuf);
}

