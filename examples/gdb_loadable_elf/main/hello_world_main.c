/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#ifdef CONFIG_IDF_TARGET_ESP32S2

#else
#define RTC_CNTL_RESET_STATE_REG    0x3ff48034
#define RTC_CNTL_CLK_CONF_REG       0x3ff48070
#define RTC_CNTL_STORE4_REG         0x3ff480b0
#define RTC_CNTL_STORE5_REG         0x3ff480b4
//#define WDT_WKEY_VALUE              0x50D83AA1
#define TIMG0_WDTWPROTECT_REG       0x3ff5f064
#define TIMG0_WDTCONFIG0_REG        0x3ff5f048
#define TIMG1_WDTWPROTECT_REG       0x3FF60064
#define TIMG1_WDTCONFIG0_REG        0x3ff60048
#define RTC_CNTL_WDTCONFIG0_REG     0x3ff4808c
#define RTC_CNTL_WDTWPROTECT_REG    0x3ff480a4
#define JTAG_ENABLE_REG             0x3ff5a1fc
#define RTC_CNTL_OPTIONS0_REG       0x3ff48000
//#define RTC_CNTL_OPTIONS0_DEF       0x1c492000
#define RTC_CNTL_SW_SYS_RST         0x80000000
#define DPORT_APPCPU_CTRL_A_REG     0x3ff0002c
#define DPORT_APPCPU_CTRL_B_REG     0x3ff00030
#define DPORT_APPCPU_CTRL_C_REG     0x3ff00034
#define DPORT_APPCPU_CTRL_D_REG     0x3ff00038
#endif



void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Free heap: %d\n", esp_get_free_heap_size());


    int *test=(int *)RTC_CNTL_RESET_STATE_REG;
    printf( "#define RTC_CNTL_RESET_STATE_REG %08X=%08X\n",RTC_CNTL_RESET_STATE_REG, *test);

    test=(int *)RTC_CNTL_CLK_CONF_REG;
    printf( "#define RTC_CNTL_CLK_CONF_REG %08X=%08X\n",RTC_CNTL_CLK_CONF_REG, *test);
    
    test=(int *)RTC_CNTL_STORE4_REG;
    printf( "#define RTC_CNTL_STORE4_REG %08X=%08X\n",RTC_CNTL_STORE4_REG, *test);

    test=(int *)RTC_CNTL_STORE5_REG;
    printf( "#define RTC_CNTL_STORE5_REG %08X=%08X\n",RTC_CNTL_STORE5_REG, *test);

    test=(int *)TIMG0_WDTWPROTECT_REG;
    printf( "#define TIMG0_WDTWPROTECT_REG %08X=%08X\n",TIMG0_WDTWPROTECT_REG, *test);

    test=(int *)TIMG0_WDTCONFIG0_REG;
    printf( "#define TIMG0_WDTCONFIG0_REG %08X=%08X\n",TIMG0_WDTCONFIG0_REG, *test);        
    
    test=(int *)TIMG1_WDTWPROTECT_REG;
    printf( "#define TIMG1_WDTWPROTECT_REG %08X=%08X\n",TIMG1_WDTWPROTECT_REG, *test);

    test=(int *)TIMG1_WDTCONFIG0_REG;
    printf( "#define TIMG1_WDTCONFIG0_REG %08X=%08X\n",TIMG1_WDTCONFIG0_REG, *test);            

    test=(int *)RTC_CNTL_WDTCONFIG0_REG;
    printf( "#define RTC_CNTL_WDTCONFIG0_REG %08X=%08X\n",RTC_CNTL_WDTCONFIG0_REG, *test);                

    test=(int *)RTC_CNTL_WDTWPROTECT_REG;
    printf( "#define RTC_CNTL_WDTWPROTECT_REG %08X=%08X\n",RTC_CNTL_WDTWPROTECT_REG, *test);                

    // Only available on rev 3 of chip
    test=(int *)JTAG_ENABLE_REG;
    printf( "#define JTAG_ENABLE_REG %08X=%08X\n",JTAG_ENABLE_REG, *test);                    

    test=(int *)RTC_CNTL_OPTIONS0_REG;
    printf( "#define RTC_CNTL_OPTIONS0_REG %08X=%08X\n",RTC_CNTL_OPTIONS0_REG, *test);

    
    // Write only
    //#define RTC_CNTL_SW_SYS_RST         0x80000000

    test=(int *)DPORT_APPCPU_CTRL_A_REG;
    printf( "#define DPORT_APPCPU_CTRL_A_REG %08X=%08X\n",DPORT_APPCPU_CTRL_A_REG, *test);

    test=(int *)DPORT_APPCPU_CTRL_B_REG;
    printf( "#define DPORT_APPCPU_CTRL_B_REG %08X=%08X\n",DPORT_APPCPU_CTRL_B_REG, *test);

    test=(int *)DPORT_APPCPU_CTRL_C_REG;
    printf( "#define DPORT_APPCPU_CTRL_C_REG %08X=%08X\n",DPORT_APPCPU_CTRL_C_REG, *test);

    test=(int *)DPORT_APPCPU_CTRL_D_REG;
    printf( "#define DPORT_APPCPU_CTRL_D_REG %08X=%08X\n",DPORT_APPCPU_CTRL_D_REG, *test);                                    
        
    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
