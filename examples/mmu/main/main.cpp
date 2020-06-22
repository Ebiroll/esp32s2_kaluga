#include "sdkconfig.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WebServer.h>
#include <stdio.h>
#include <string>
#include "RequestManager.h"

#include <freertos/event_groups.h>
#include <esp_system.h>
#include <esp_wifi.h>
//#include <esp_event_loop.h>
#include "esp_event.h"
#include <esp_log.h>
#include <esp_err.h>
#include "JSON.h"
#include "nvs_flash.h"

extern "C" {
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
}


#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

static char tag[] = "Mmu";

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = 0x00000001;


static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 3) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(tag, "retry to connect to the AP");
        } else {
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        }
        ESP_LOGI(tag,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(tag, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    }
}



//-------------------------------
static void initialise_wifi(void)
{
      ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(tag, "wifi_init_sta finished.");

    /* The event will not be processed after unregister */
    //ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    //ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
}

/*
//-------------------------------
static void initialize_sntp(void)
{
    ESP_LOGI(tag, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}
*/

void webserver_task(void* param) {


}

extern "C" {
	int app_main(void);
}

#if 0
#define SPIFFS_BASE_PATH "/spiffs"


extern int spiffs_is_registered;
extern int spiffs_is_mounted;

void vfs_spiffs_register();
int spiffs_mount();
int spiffs_unmount(int unreg);
void spiffs_fs_stat(uint32_t *total, uint32_t *used);
#endif


/* Flash MMU table for PRO CPU */
#define DPORT_PRO_FLASH_MMU_TABLE ((volatile uint32_t*) 0x3FF10000)

/* Flash MMU table for APP CPU */
#define DPORT_APP_FLASH_MMU_TABLE ((volatile uint32_t*) 0x3FF12000)


int app_main(void) {
    //spi_flash_init();
   void *ptr;
   nvs_flash_init(); 
   spi_flash_mmap_handle_t handle;
   spi_flash_mmap_handle_t handle2;
   spi_flash_mmap_handle_t handle3;

#define ESP_PARTITION_TABLE_ADDR 0x8000


#if 0

    DPORT_PRO_FLASH_MMU_TABLE[51]=0x6;
    DPORT_APP_FLASH_MMU_TABLE[51]=0x6;

// Note this is not recommended, only for test!!
    DPORT_PRO_FLASH_MMU_TABLE[70]=0x6;
    DPORT_APP_FLASH_MMU_TABLE[70]=0x6;

// Note this is not recommended, only for test!!
    DPORT_PRO_FLASH_MMU_TABLE[88]=0x6;
    DPORT_APP_FLASH_MMU_TABLE[88]=0x6;


// Note this is not recommended, only for test!!
    DPORT_PRO_FLASH_MMU_TABLE[92]=0x6;
    DPORT_APP_FLASH_MMU_TABLE[92]=0x6;


    DPORT_PRO_FLASH_MMU_TABLE[108]=0x7;
    DPORT_APP_FLASH_MMU_TABLE[109]=0x7;


    DPORT_PRO_FLASH_MMU_TABLE[109]=0x8;
    DPORT_APP_FLASH_MMU_TABLE[109]=0x8;

#endif



  //printf("mmap partition data\n");
  //esp_err_t err = spi_flash_mmap(ESP_PARTITION_TABLE_ADDR & 0xffff0000,
  //          SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle);
  //  if (err != ESP_OK) {
  //    printf("mmap fail\n");
  //  }
  // printf( "0x8000 mapped to %p\n", ptr);            

   esp_err_t err = spi_flash_mmap(0x180000 & 0xffff0000,
            SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle2);


    if (err != ESP_OK) {
      printf("mmap 0x180000 fail \n");
    }
   printf( "0x180000 mapped to %p\n", ptr);            


   spi_flash_mmap(0x000 & 0xffff0000,
            SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle3);


    if (err != ESP_OK) {
      printf("mmap 0x0000 fail \n");
    }
   printf( "0x0000 mapped to %p\n", ptr);            


   spi_flash_mmap(0x10000 & 0xffff0000,
            SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle2);
    if (err != ESP_OK) {
      printf("mmap 0x10000 fail \n");
    }
   printf( "0x10000 mapped to %p\n", ptr);            


   spi_flash_mmap(0x40000 & 0xffff0000,
            SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle2);
    if (err != ESP_OK) {
      printf("mmap 0x40000 fail \n");
    }
   printf( "0x40000 mapped to %p\n", ptr);            


   spi_flash_mmap(0x50000 & 0xffff0000,
            SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle2);
    if (err != ESP_OK) {
      printf("mmap 0x50000 fail \n");
    }
   printf( "0x50000 mapped to %p\n", ptr);            

   spi_flash_mmap(0x60000 & 0xffff0000,
            SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle2);
    if (err != ESP_OK) {
      printf("mmap 0x60000 fail \n");
    }
   printf( "0x60000 mapped to %p\n", ptr);            

   spi_flash_mmap(0x70000 & 0xffff0000,
            SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle2);
    if (err != ESP_OK) {
      printf("mmap 0x70000 fail \n");
    }
   printf( "0x70000 mapped to %p\n", ptr);            

   spi_flash_mmap(0x80000 & 0xffff0000,
            SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle2);
    if (err != ESP_OK) {
      printf("mmap 0x80000 fail \n");
    }
   printf( "0x80000 mapped to %p\n", ptr);            

   spi_flash_mmap(0x90000 & 0xffff0000,
            SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle2);
    if (err != ESP_OK) {
      printf("mmap 0x90000 fail \n");
    }
   printf( "0x90000 mapped to %p\n", ptr);            

   spi_flash_mmap(0xa0000 & 0xffff0000,
            SPI_FLASH_SEC_SIZE, SPI_FLASH_MMAP_DATA, (const void**) &ptr, &handle2);
    if (err != ESP_OK) {
      printf("mmap 0xa0000 fail \n");
    }
   printf( "0xa0000 mapped to %p\n", ptr);            


   RequestManager *manager=new RequestManager();

   //int test_buff[4];
#if 0
   if (spi_flash_read(0x1800fc, (void *)test_buff, 4) != 0) { 
         printf("Failed spi_flash_read\r\n\n");
   }

   printf("%08X\n\n",(unsigned int)test_buff[0]);

   //int write_buff[4];
#endif

   //test_buff[0]=4711;
   //printf("%08X\n\n",(unsigned int)test_buff[0]);

    esp_log_level_set("*", ESP_LOG_VERBOSE);

   /////////

   esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }


    esp_spiffs_mount();

    printf("Mounted \r\n");

    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
 
    initialise_wifi();

    printf("waiting \r\n");


    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);

    printf("Starting manager\r\n");

    manager->start();

        //xTaskCreatePinnedToCore(task_lwip_init, "loop", 4096, NULL, 14, NULL, 0);


    while (1) {
		vTaskDelay(1000 / portTICK_RATE_MS);
    }
	return 0;
}
