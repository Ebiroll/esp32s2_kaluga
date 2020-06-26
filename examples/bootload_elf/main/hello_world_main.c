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


#define TEST_DPORT_MMU_ACCESS_FLASH   0x800
#define TEST_DPORT_MMU_ACCESS_SPIRAM  0x300

int Cache_Dbus_MMU_Set(uint32_t ext_ram,uint32_t vaddr,uint32_t paddr,uint32_t psize,uint32_t num,
                      uint32_t fixed);

int Cache_Ibus_MMU_Set(uint32_t ext_ram,uint32_t vaddr,uint32_t paddr,uint32_t psize,uint32_t num,
                      uint32_t fixed);


/**
  * @brief Set DCache mmu mapping.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t ext_ram : DPORT_MMU_ACCESS_FLASH for flash, DPORT_MMU_ACCESS_SPIRAM for spiram, DPORT_MMU_INVALID for invalid.
  *
  * @param  uint32_t vaddr : virtual address in CPU address space.
  *                              Can be DRam0, DRam1, DRom0, DPort and AHB buses address.
  *                              Should be aligned by psize.
  *
  * @param  uint32_t paddr : physical address in external memory.
  *                              Should be aligned by psize.
  *
  * @param  uint32_t psize : page size of DCache, in kilobytes. Should be 64 here.
  *
  * @param  uint32_t num : pages to be set.

  * @param  uint32_t fixed : 0 for physical pages grow with virtual pages, other for virtual pages map to same physical page.
  *
  * @return uint32_t: error status
  *                   0 : mmu set success
  *                   2 : vaddr or paddr is not aligned
  *                   3 : psize error
  *                   4 : vaddr is out of range
  */
int Test_Cache_Dbus_MMU_Set(uint32_t ext_ram,uint32_t vaddr,uint32_t paddr,uint32_t psize,uint32_t num,
                      uint32_t fixed)

{
  int ret_val;
  int offset_ix;
  uint *mmu_table_ptr;
  uint map_val;
  uint uVar1;
  uint uVar2;
  
  ret_val = 2;
  if (((0xffff >> (((char)(0x40 / psize) - 1U) & 0x1f) & (vaddr | paddr)) == 0) &&
     (ret_val = 3, psize == 0x40)) {
    do {
      if (num == 0) {
        return 0;
      }
      uVar1 = vaddr + (0x400000 & 0xffc00000);
      ret_val = uVar1 - vaddr;
      uVar2 = ret_val >> 0x10;
      if (num <= uVar2) {
        uVar2 = num;
      }
      if (vaddr + 0xc0400000 < 0x400000) {
        offset_ix = (vaddr >> 0x10 & 0x3f) + 0xc0;
      }
      else {
        if (vaddr + 0xc0800000 < 0x400000) {
          offset_ix = (vaddr >> 0x10 & 0x3f) + 0x100;
        }
        else {
          if (0x3fffff < vaddr + 0xc0c00000) {
            return 4;
          }
          offset_ix = (vaddr >> 0x10 & 0x3f) + 0x140;
        }
      }
      mmu_table_ptr = (uint *)(0x61801000 + offset_ix * 4);
      map_val = 0;
      while (map_val != uVar2) {
        if (fixed == 0) {
          *mmu_table_ptr = map_val + ((paddr >> 0x10) | ext_ram);
        }
        else {
          *mmu_table_ptr = paddr >> 0x10 | ext_ram;
        }
        map_val = map_val + 1;
        mmu_table_ptr = mmu_table_ptr + 1;
      }
      num = num - map_val;
      vaddr = uVar1;
      if (fixed == 0) {
        paddr = paddr + ret_val;
      }
    } while( true );
  }
  return ret_val;
}

/**
  * @brief Set ICache mmu mapping.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t ext_ram : DPORT_MMU_ACCESS_FLASH for flash, DPORT_MMU_ACCESS_SPIRAM for spiram, DPORT_MMU_INVALID for invalid.
  *
  * @param  uint32_t vaddr : virtual address in CPU address space.
  *                              Can be Iram0,Iram1,Irom0,Drom0 and AHB buses address.
  *                              Should be aligned by psize.
  *
  * @param  uint32_t paddr : physical address in external memory.
  *                              Should be aligned by psize.
  *
  * @param  uint32_t psize : page size of ICache, in kilobytes. Should be 64 here.
  *
  * @param  uint32_t num : pages to be set.
  *
  * @param  uint32_t fixed : 0 for physical pages grow with virtual pages, other for virtual pages map to same physical page.
  *
  * @return uint32_t: error status
  *                   0 : mmu set success
  *                   2 : vaddr or paddr is not aligned
  *                   3 : psize error
  *                   4 : vaddr is out of range
  */
int Test_Cache_Ibus_MMU_Set(uint32_t ext_ram,uint32_t vaddr,uint32_t paddr,uint32_t psize,uint32_t num,
                      uint32_t fixed)

{
  int ret_val;
  uint offset_ix;
  uint *mmu_table_ptr;
  uint uVar1;
  uint end_ix;
  
  ret_val = 2;
  if (((0xffff >> (((char)(0x40 / psize) - 1U) & 0x1f) & (vaddr | paddr)) == 0) &&
     (ret_val = 3, psize == 0x40)) {
    do {
      if (num == 0) {
        return 0;
      }
      uVar1 = (vaddr + 0x400000) & 0xffc00000;
      ret_val = uVar1 - vaddr;
      end_ix = ret_val >> 0x10;
      if (num <= end_ix) {
        end_ix = num;
      }
      if (vaddr   >= 0xC0C00000) {
        offset_ix = (vaddr >> 0x10 & 0x3f) + 0x80;
      } else {
        if (vaddr >= 0xBFC00000) {
          offset_ix = vaddr >> 0x10 & 0x3f;
        } else {
          if ( vaddr >= 0xBF800001) {
            return 4;
          }
          offset_ix = (vaddr >> 0x10 & 0x3f) + 0x40;
        }
      }
      mmu_table_ptr = (uint *)(0x61801000 + offset_ix * 4);
      offset_ix = 0;
      while (offset_ix != end_ix) {
        if (fixed == 0) {
          *mmu_table_ptr = (offset_ix + (paddr >> 0x10)) | ext_ram;
        }
        else {
          *mmu_table_ptr = paddr >> 0x10 | ext_ram;
        }
        offset_ix = offset_ix + 1;
        mmu_table_ptr = mmu_table_ptr + 1;
      }
      num = num - offset_ix;
      vaddr = uVar1;
      if (fixed == 0) {
        paddr = paddr + ret_val;
      }
    } while( true );
  }
  return ret_val;
}

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


//int Cache_Dbus_MMU_Set(uint32_t ext_ram,uint32_t vaddr,uint32_t paddr,uint32_t psize,uint32_t num,
//                      uint32_t fixed);

//    Test_Cache_Dbus_MMU_Set(TEST_DPORT_MMU_ACCESS_FLASH,)



        
    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
