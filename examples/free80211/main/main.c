#include "freertos/FreeRTOS.h"

#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"

#include "nvs_flash.h"
#include "string.h"

#include "esp_attr.h"


void IRAM_ATTR my_patched_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq,uint32_t original_fun)
{
__asm__ volatile (
   "extui      a5,a5,0x0,0x8\n\t"
   "mov        a13,a5\n\t"
   "mov        a12,a4\n\t"
   "mov        a11,a3\n\t"
   "mov        a10,a2\n\t"
   "movi.n     a10,0x0\n\t"
   "movi a10,0\n\t"
   "addmi a6,a6,0x14\n\t"
   "jx  a6\n\t");
}


IRAM_ATTR unsigned char my_wifi_80211_tx[]=
{
  0x36,
  0x41,
  0x00,
  0x50,
  0x50,
  0x74,
  0x40,
  0xc4,
  0x20,
  0x50,
  0xd5,
  0x20,
  0xbd,
  0x03,
  0x20,
  0xa2,
  0x20,
	// j=17
  0x0c,
  0x0a,
  0xe2,
  0xce,
  0x14,
  0xa0,
  0x0e,
  0x00
};


/*
 * This is the (currently unofficial) 802.11 raw frame TX API,
 * defined in esp32-wifi-lib's libnet80211.a/ieee80211_output.o
 *
 * This declaration is all you need for using esp_wifi_80211_tx in your own application.
 */
esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);



typedef esp_err_t *(patched_wifi_80211_tx)(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq,uint32_t original_fun);



uint8_t beacon_raw[] = {
	0x80, 0x00,							// 0-1: Frame Control
	0x00, 0x00,							// 2-3: Duration
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff,				// 4-9: Destination address (broadcast)
	0xba, 0xde, 0xaf, 0xfe, 0x00, 0x06,				// 10-15: Source address
	0xba, 0xde, 0xaf, 0xfe, 0x00, 0x06,				// 16-21: BSSID
	0x00, 0x00,							// 22-23: Sequence / fragment number
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,			// 24-31: Timestamp (GETS OVERWRITTEN TO 0 BY HARDWARE)
	0x64, 0x00,							// 32-33: Beacon interval
	0x31, 0x04,							// 34-35: Capability info
	0x00, 0x00, /* FILL CONTENT HERE */				// 36-38: SSID parameter set, 0x00:length:content
	0x01, 0x08, 0x82, 0x84,	0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24,	// 39-48: Supported rates
	0x03, 0x01, 0x01,						// 49-51: DS Parameter set, current channel 1 (= 0x01),
	0x05, 0x04, 0x01, 0x02, 0x00, 0x00,				// 52-57: Traffic Indication Map
	
};

char *rick_ssids[] = {
	"01 Never gonna give you up",
	"02 Never gonna let you down",
	"03 Never gonna run around",
	"04 and desert you",
	"05 Never gonna make you cry",
	"06 Never gonna say goodbye",
	"07 Never gonna tell a lie",
	"08 and hurt you"
};

#define BEACON_SSID_OFFSET 38
#define SRCADDR_OFFSET 10
#define BSSID_OFFSET 16
#define SEQNUM_OFFSET 22
#define TOTAL_LINES (sizeof(rick_ssids) / sizeof(char *))

esp_err_t event_handler(void *ctx, system_event_t *event) {
	return ESP_OK;
}

void spam_task(void *pvParameter) {
	uint8_t line = 0;
	//patched_wifi_80211_tx *fun=(patched_wifi_80211_tx *)my_wifi_80211_tx;

	// Keep track of beacon sequence numbers on a per-songline-basis
	uint16_t seqnum[TOTAL_LINES] = { 0 };

	

	for (;;) {
		vTaskDelay(100 / TOTAL_LINES / portTICK_PERIOD_MS);

		// Insert line of Rick Astley's "Never Gonna Give You Up" into beacon packet
		printf("%i %i %s\r\n", strlen(rick_ssids[line]), TOTAL_LINES, rick_ssids[line]);

		uint8_t beacon_rick[200];
		memcpy(beacon_rick, beacon_raw, BEACON_SSID_OFFSET - 1);
		beacon_rick[BEACON_SSID_OFFSET - 1] = strlen(rick_ssids[line]);
		memcpy(&beacon_rick[BEACON_SSID_OFFSET], rick_ssids[line], strlen(rick_ssids[line]));
		memcpy(&beacon_rick[BEACON_SSID_OFFSET + strlen(rick_ssids[line])], &beacon_raw[BEACON_SSID_OFFSET], sizeof(beacon_raw) - BEACON_SSID_OFFSET);

		// Last byte of source address / BSSID will be line number - emulate multiple APs broadcasting one song line each
		beacon_rick[SRCADDR_OFFSET + 5] = line;
		beacon_rick[BSSID_OFFSET + 5] = line;

		// Update sequence number
		beacon_rick[SEQNUM_OFFSET] = (seqnum[line] & 0x0f) << 4;
		beacon_rick[SEQNUM_OFFSET + 1] = (seqnum[line] & 0xff0) >> 4;
		seqnum[line]++;
		if (seqnum[line] > 0xfff)
			seqnum[line] = 0;

		//esp_wifi_80211_tx(WIFI_IF_AP, beacon_rick, sizeof(beacon_raw) + strlen(rick_ssids[line]), false);

		//vTaskDelay(1000 / portTICK_PERIOD_MS);

	    //esp_err_t t=*fun(WIFI_IF_AP, beacon_rick, sizeof(beacon_raw) + strlen(rick_ssids[line]), false,esp_wifi_80211_tx);
		my_patched_wifi_80211_tx(WIFI_IF_AP, beacon_rick, sizeof(beacon_raw) + strlen(rick_ssids[line]), false,esp_wifi_80211_tx);
		//printf("%d",t);
		
		//vTaskDelay(2000 / portTICK_PERIOD_MS);

		if (++line >= TOTAL_LINES)
			line = 0;
	}
}

static inline uint32_t _getCycleCount(void) {
  uint32_t ccount;
  __asm__ __volatile__("rsr %0,ccount":"=a" (ccount));
  return ccount;
}



static inline uint32_t getSP(void) {
  uint32_t sp;
  __asm__ __volatile__("mov %0,a1":"=a" (sp));
  return sp;
}

void incMe(int* i) 
{
	uint32_t sp;
	(*i)++;
	printf("i after one inc %d\n",*i);
	(*i)++;
	printf("i after second inc %d\n",*i);
	sp=getSP();
	printf("stack_p %x\n",sp);

}

void patchIncMe(int* i,uint32_t fun_ptr_a3) {
__asm__ volatile (
   "addmi a3,a3,0x03\n\t"
   "jx  a3\n\t");
} 




void app_main(void) {
	uint32_t addr;
	int i=0;
	// only works in qemu
	//memcpy((char *)my_wifi_80211_tx,(char *)esp_wifi_80211_tx,0x1200);

	addr=getSP();
	printf("stack_main %x\n",addr);
	incMe(&i);
	incMe(&i);
	i=0;
	patchIncMe(&i,(uint32_t)patchIncMe);
	patchIncMe(&i,(uint32_t)patchIncMe);
	incMe(&i);


	addr=(uint32_t) &my_wifi_80211_tx[0];

	//<esp_wifi_80211_tx+20>\n\t"

	// Patch the function to skip sanity check
#if 0	
	int j=0;
	my_wifi_80211_tx[j++]=0x36;
	my_wifi_80211_tx[j++]=0x41;
	my_wifi_80211_tx[j++]=0x00;
	my_wifi_80211_tx[j++]=0x50;
	my_wifi_80211_tx[j++]=0x50;
	my_wifi_80211_tx[j++]=0x74;
	my_wifi_80211_tx[j++]=0x40;
	my_wifi_80211_tx[j++]=0xc4;
	my_wifi_80211_tx[j++]=0x20;
	my_wifi_80211_tx[j++]=0x50;
	my_wifi_80211_tx[j++]=0xd5;
	my_wifi_80211_tx[j++]=0x20;
	my_wifi_80211_tx[j++]=0xbd;
	my_wifi_80211_tx[j++]=0x03;
	my_wifi_80211_tx[j++]=0x20;
	my_wifi_80211_tx[j++]=0xa2;
	my_wifi_80211_tx[j++]=0x20;
	// j=17
	my_wifi_80211_tx[j++]=0x0c;
	my_wifi_80211_tx[j++]=0x0a;
	my_wifi_80211_tx[j++]=0xe2;
	my_wifi_80211_tx[j++]=0xce;
	my_wifi_80211_tx[j++]=0x14;
	my_wifi_80211_tx[j++]=0xa0;
	my_wifi_80211_tx[j++]=0x0e;
	my_wifi_80211_tx[j++]=0x00;
#endif

#if 0
	__asm__ volatile (
		"movi a10,0\n\t"
		"addmi a14,a14,0x14\n\t"
		"jx  a14\n\t");
#endif


	//printf("cc=%d %x",_getCycleCount(),addr);


	nvs_flash_init();
	tcpip_adapter_init();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	// Init dummy AP to specify a channel and get WiFi hardware into
	// a mode where we can send the actual fake beacon frames.
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	wifi_config_t ap_config = {
		.ap = {
			.ssid = "esp32-beaconspam",
			.ssid_len = 0,
			.password = "dummypassword",
			.channel = 1,
			.authmode = WIFI_AUTH_WPA2_PSK,
			.ssid_hidden = 1,
			.max_connection = 4,
			.beacon_interval = 60000
		}
	};

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

	xTaskCreate(&spam_task, "spam_task", 2048, NULL, 5, NULL);
}
