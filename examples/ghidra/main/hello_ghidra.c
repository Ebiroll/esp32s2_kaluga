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
#include "my_type.h"

void one_ptr_arg(my_type *t){
  t->my_int=t->my_int+10;
}

int one_arg(int my_arg){
    char buffer[8];
    int i;
    my_type silly;
    silly.my_int=4;

    for (i = 0; i < my_arg; ++i) {
        buffer[i]=0x32;
    }
    one_ptr_arg(&silly);
    printf("My_arg=%d,%s",my_arg,buffer);

    printf("silly.my_int=%d\n",silly.my_int);
    return (silly.my_int);
}

int three_args(int arg1,int arg2,int arg3) {
    return(arg1+arg2+arg3);
}


void two_args(int my_arg,my_type *t)
{
    int my_local = my_arg + 2;
    int i;
    printf("my_int=%d\n",t->my_int);
    for (i = 0; i < my_local; ++i) {
        printf("i = %d\n", i);
    }

    int ret=one_arg(my_arg);
    printf("ret=%d\n",ret);

    ret=three_args(my_local,2,3);
    printf("ret=%d\n",ret);

}

int two_args_one_ret(int my_arg,int my_arg2)
{
    int my_local = my_arg + 2;
    int i;
    int ret=0;
    for (i = 0; i < my_local; ++i) {
      ret+=i;
    }
    return ret;
}



void app_main(void)
{
    my_type test = {
	  .my_int = 4
    }; 

    printf("Hello Ghidra!\n");

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

    two_args(2,&test);

    int ret=two_args_one_ret(2,3);
    
    printf("Restarting now. %d\n",ret);
    fflush(stdout);
    esp_restart();
}
