# esp32s2_kaluga
My exepriences with the esp32s2 kaluga dev board

# Mystery box
I was able to order one of 10 kaluga boards from Digikey on the 13/5, however I could not find much documentation about what I had actually ordered. Hovever I did find the schematics located here. https://github.com/espressif/esp-dev-kits/tree/master/esp32-s2-kaluga-1/docs It seems like we have an FT2232HL on board. Also I expect the LCD to be in the box. Actually it has arrived 15/5 and it contained the ESP32-S2-WROVER module. The touch board, and camera was also included. Nice.


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

# esp32s2-wroom
This one was not mounted on my board.
https://www.espressif.com/sites/default/files/documentation/esp32-s2-wroom_esp32-s2-wroom-i_datasheet_en.pdf

# esp32s2-wroom
This was found on my board. 4MB of flash and 2MB of PSRAM.
https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_en.pdf


# esp32 technical reference.
https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_en.pdf

# USB
Pins are broken out on the board, but an USB connector would have been nice.
The ESP32-S2 chip needs to be in bootloader mode for the detection as a DFU device and flashing.

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
