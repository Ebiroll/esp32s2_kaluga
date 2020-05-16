/*!< Led strip test
 *
 *  This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 *  Unless required by applicable law or agreed to in writing, this
 *  software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *  CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/touch_pad.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "driver/rmt.h"
#include "led_strip.h"


#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define LEDC_COLOR 200        /*!< Initializes the led light value （0～255）*/
#define LEDC_RANGE 10         /*!< Light adjustment amplitude */

static const char *TAG = "Led test";


typedef enum {
    LED_NOOE = 0,
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_MAX,
} type_lcd_t;

led_strip_t *strip;



/**
  * @brief Initialize the WB2812
  *
  * @param gpio_num: GPIO of WB2812
  * @param led_number: The number of leds
  * @param rmt_channel: RMT channel number
  *
  * @return
  *      - ESP_OK: Initialize successfully
  *      - ESP_FAIL: Initialize fail
  */
esp_err_t example_rmt_init(uint8_t gpio_num, int led_number, uint8_t rmt_channel)
{
    ESP_LOGI(TAG, "Initializing WS2812");
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(gpio_num, rmt_channel);

    /*!< set counter clock to 40MHz */
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(led_number, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);

    if (!strip) {
        ESP_LOGE(TAG, "install WS2812 driver failed");
        return ESP_FAIL;
    }

    /*!< Clear LED strip (turn off all LEDs) */
    ESP_ERROR_CHECK(strip->clear(strip, 100));
    /*!< Show simple rainbow chasing pattern */

    return ESP_OK;
}


static void led_task(void *pvParameter)
{
    uint8_t red   = 0;
    uint8_t green = 0;
    uint8_t blue  = 0;

    /*!< Wait touch sensor init done */
    for (;;) {
        vTaskDelay(100 / portTICK_RATE_MS);
        {
            ESP_ERROR_CHECK(strip->set_pixel(strip, 0, LEDC_COLOR, 0, 0));
            red   = LEDC_COLOR;
            green = 0;
            blue  = 0;
            ESP_ERROR_CHECK(strip->set_pixel(strip, 0, red, green, blue));
            ESP_ERROR_CHECK(strip->refresh(strip, 0));
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
        {
            ESP_ERROR_CHECK(strip->set_pixel(strip, 0, 0, LEDC_COLOR, 0));
            red   = 0;
            green = LEDC_COLOR;
            blue  = 0;
            ESP_ERROR_CHECK(strip->set_pixel(strip, 0, red, green, blue));
            ESP_ERROR_CHECK(strip->refresh(strip, 0));
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
        {
            ESP_ERROR_CHECK(strip->set_pixel(strip, 0, 0, 0, LEDC_COLOR));
            red   = 0;
            green = 0;
            blue  = LEDC_COLOR;
            ESP_ERROR_CHECK(strip->set_pixel(strip, 0, red, green, blue));
            ESP_ERROR_CHECK(strip->refresh(strip, 0));
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

#define MAX_ALLOCS 20

void app_main(void)
{
    /*!< Initialize the WS2812 */
    ESP_ERROR_CHECK(example_rmt_init(CONFIG_EXAMPLE_RMT_TX_GPIO, CONFIG_EXAMPLE_STRIP_LED_NUMBER, RMT_CHANNEL_0));
    /*!< Initialize the touch pad */

    /*!< Start a task to show what pads have been touched */
    xTaskCreate(&led_task, "led_task", 2048, NULL, 5, NULL);


    void *ptr[MAX_ALLOCS];
    size_t idx = 0, size = 1024*1024, sum = 0;

    //nvs_flash_init();
    
    printf("starting\n");
    uint32_t test=esp_get_free_heap_size();
    printf("Free heap size %d\n",test);


    while (idx < MAX_ALLOCS && size > 64) {
        ptr[idx] = malloc(size);
        if (ptr[idx] == NULL) {
            size /= 2;
            continue;
        }
        idx++;
        sum += size;
        printf("ALLOCS: %d/%d, SIZE %d/%d\n", idx, MAX_ALLOCS, size, sum);
    }
    test=esp_get_free_heap_size();
    printf("Free heap size %d\n",test);

    while (idx--) {
        printf("free idx %d\n", idx);
        free(ptr[idx]);
    }
    
    test=esp_get_free_heap_size();
    printf("Free heap size %d\n",test);
}
