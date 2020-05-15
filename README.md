# esp32s2_kaluga
My exepriences with the esp32s2 kaluga dev board

# Mystery box
I was able to order one of 10 kaluga boards from Digikey on the 13/5-2020. I could not find much documentation about what I had actually ordered, however I did find the schematics located here. https://github.com/espressif/esp-dev-kits/tree/master/esp32-s2-kaluga-1/docs It seems like we have an FT2232HL on board. Also I expect the LCD to be in the box. Actually it has arrived 15/5 and it contained the ESP32-S2-WROVER module. The touch board, and camera was also included. Nice.


![mainbody](docs/_static/ESP32-S2-Kaluga_V1.0_mainbody.png)

    1. Pay attention that Module may use OSPI mode. Thus, 33-37 can not be used as other function.
    2. If psram was soldered, GPIO26 will be used as CS, which can't be used as other functions

# JTAG debugging
I assume these instructions will help.
https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-guides/jtag-debugging/index.html
I would assume there is support in for the https://github.com/espressif/openocd-esp32 . 
It does not seem like that so we, have to create the configuration file ourselfs.

![jtag](img/jtag_schematics.png)


<table class="docutils align-default">
<colgroup>
<col style="width: 8%" />
<col style="width: 59%" />
<col style="width: 33%" />
</colgroup>
<thead>
<tr class="row-odd"><th class="head"></th>
<th class="head"><p>ESP32-S2 Pin</p></th>
<th class="head"><p>JTAG Signal</p></th>
</tr>
</thead>
<tbody>
<tr class="row-even"><td><p>1</p></td>
<td><p>CHIP_PU</p></td>
<td><p>TRST_N</p></td>
</tr>
<tr class="row-odd"><td><p>2</p></td>
<td><p>MTDO / GPIO40</p></td>
<td><p>TDO</p></td>
</tr>
<tr class="row-even"><td><p>3</p></td>
<td><p>MTDI / GPIO41</p></td>
<td><p>TDI</p></td>
</tr>
<tr class="row-odd"><td><p>4</p></td>
<td><p>MTCK / GPIO39</p></td>
<td><p>TCK</p></td>
</tr>
<tr class="row-even"><td><p>5</p></td>
<td><p>MTMS / GPIO42</p></td>
<td><p>TMS</p></td>
</tr>
<tr class="row-odd"><td><p>6</p></td>
<td><p>GND</p></td>
<td><p>GND</p></td>
</tr>
</tbody>
</table>

# First try
```
USB to serial drivers found.
[  401.064721] usbcore: registered new interface driver ftdi_sio
[  401.064738] usbserial: USB Serial support registered for FTDI USB Serial Device
[  401.065190] ftdi_sio 2-2.3:1.0: FTDI USB Serial Device converter detected
[  401.065256] usb 2-2.3: Detected FT2232H
[  401.067172] usb 2-2.3: FTDI USB Serial Device converter now attached to ttyUSB0
[  401.067222] ftdi_sio 2-2.3:1.1: FTDI USB Serial Device converter detected
[  401.067249] usb 2-2.3: Detected FT2232H
[  401.067871] usb 2-2.3: FTDI USB Serial Device converter now attached to ttyUSB1
```

screen /dev/ttyUSB1 115200

```
ESP-ROM:esp32s2-rc4-20191025
Build:Oct 25 2019
rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3ffe8100,len:0x4
load:0x3ffe8104,len:0x1974
load:0x40050000,len:0x1898
load:0x40054000,len:0x210c
entry 0x40050314
I (46) boot: ESP-IDF v4.2-dev-1035-ge07eb8eca-dirty 2nd stage bootloader
I (46) boot: compile time 21:05:26
I (46) boot: chip revision: 0
I (50) qio_mode: Enabling default flash chip QIO
I (55) boot.esp32s2: SPI Speed      : 80MHz
I (60) boot.esp32s2: SPI Mode       : QIO
I (64) boot.esp32s2: SPI Flash Size : 4MB
I (69) boot: Enabling RNG early entropy source...
I (75) boot: Partition Table:
I (78) boot: ## Label            Usage          Type ST Offset   Length
I (85) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (93) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (100) boot:  2 factory          factory app      00 00 00010000 00100000
I (108) boot: End of partition table
I (112) esp_image: segment 0: paddr=0x00010020 vaddr=0x3f000020 size=0x1b43c (111676) map
I (141) esp_image: segment 1: paddr=0x0002b464 vaddr=0x3ffc01e0 size=0x02050 (  8272) load
I (143) esp_image: segment 2: paddr=0x0002d4bc vaddr=0x40024000 size=0x00404 (  1028) load
I (148) esp_image: segment 3: paddr=0x0002d8c8 vaddr=0x40024404 size=0x02750 ( 10064) load
I (159) esp_image: segment 4: paddr=0x00030020 vaddr=0x40080020 size=0x308a8 (198824) map
I (202) esp_image: segment 5: paddr=0x000608d0 vaddr=0x40026b54 size=0x0968c ( 38540) load
I (218) boot: Loaded app from partition at offset 0x10000
I (218) boot: Disabling RNG early entropy source...
I (219) cache: Instruction cache 	: size 8KB, 4Ways, cache line size 32Byte
I (226) cache: Data cache 		: size 8KB, 4Ways, cache line size 32Byte
I (233) spiram: Found 16MBit SPI RAM device
I (238) spiram: SPI RAM mode: sram 80m
I (238) spiram: SPI RAM mode: sram 80m
I (242) spiram: PSRAM initialized, cache is in normal (1-core) mode.
I (249) cpu_start: Pro cpu up.
I (253) cpu_start: Application information:
I (258) cpu_start: Project name:     esp32-s2-hmi
I (263) cpu_start: App version:      338cedb-dirty
I (269) cpu_start: Compile time:     Apr 13 2020 21:05:17
I (275) cpu_start: ELF file SHA256:  1a45da0eedee610c...
I (281) cpu_start: ESP-IDF:          v4.2-dev-1035-ge07eb8eca-dirty
I (288) cpu_start: Single core mode
I (511) spiram: SPI SRAM memory test OK
I (511) heap_init: Initializing. RAM available for dynamic allocation:
I (511) heap_init: At 3FFD8BA8 len 00023458 (141 KiB): D/IRAM
I (517) heap_init: At 3FFFC000 len 00003A10 (14 KiB): DRAM
I (524) cpu_start: Pro cpu start user code
I (528) spiram: Adding pool of 2048K of external SPI memory to heap allocator
I (536) spiram: Reserving pool of 32K of internal memory for DMA/internal allocations
I (597) spi_flash: detected chip: generic
I (597) spi_flash: flash io: qio
I (597) cpu_start: Starting scheduler on PRO CPU.
I (601) cam: cam_xclk_pin setup
I (601) cam: cam_buffer_size: 51200, cam_dma_size: 3200, cam_dma_node_cnt: 16, cam_total_cnt: 6
```

# Test openocd
. ~/esp/esp-idf/export.sh
I have prepared a bord config file in the openpcd dir
openocd -f openocd/board/esp32-kaluga-1-3.3v.cfg
```
Open On-Chip Debugger  v0.10.0-esp32-20200420 (2020-04-20-16:15)
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.org/doc/doxygen/bugs.html
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : ftdi: if you experience problems at higher adapter clocks, try the command "ftdi_tdo_sample_edge falling"
Info : clock speed 20000 kHz
Error: JTAG scan chain interrogation failed: all ones
Error: Check JTAG interface, timings, target power, etc.
Error: Trying to use configured scan chain anyway...
Error: esp32s2.cpu: IR capture error; saw 0x1f not 0x01
Warn : Bypassing JTAG setup events due to errors
Info : Listening on port 3333 for gdb connections
```
This indicates that I have not configured the switches yes.


# esp32s2-wroom
This one was not mounted on my board.
https://www.espressif.com/sites/default/files/documentation/esp32-s2-wroom_esp32-s2-wroom-i_datasheet_en.pdf

# esp32s2-wrover
This was found on my board. 4MB of flash and 2MB of PSRAM.
https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_en.pdf


# esp32 technical reference.
https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_en.pdf

# USB
Pins are broken out on the board, but an USB connector would have been nice.
The ESP32-S2 chip needs to be in bootloader mode for the detection as a DFU device and flashing. I find that 5V is on the opposite side on PIN 19 & 20. Now this is inonvenient. Mhy did they not put an USB OTG connector on the board?

<table class="docutils align-default">
<colgroup>
<col style="width: 32%" />
<col style="width: 68%" />
</colgroup>
<thead>
<tr class="row-odd"><th class="head"><p>GPIO</p></th>
<th class="head"><p>USB</p></th>
</tr>
</thead>
<tbody>
<tr class="row-even"><td><p>20</p></td>
<td><p>D- (green)</p></td>
</tr>
<tr class="row-odd"><td><p>19</p></td>
<td><p>D+ (white)</p></td>
</tr>
<tr class="row-even"><td><p>GND</p></td>
<td><p>GND (black)</p></td>
</tr>
<tr class="row-odd"><td><p>+5V</p></td>
<td><p>+5V (red)</p></td>
</tr>
</tbody>
</table>


https://electronics.stackexchange.com/questions/73295/in-a-usb-cable-is-it-ok-to-swap-the-d-and-d-wires/73297#73297
