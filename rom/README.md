# Dumping the ROM with gdb
    (gdb) dump binary memory drom0.bin 0x3F400000 0x3F400000+4194304
    (gdb) dump binary memory irom0.bin 0x40800000 0x40800000+4194304

# Save a copy of the flash

    esptool.py -p /dev/ttyUSB1 --chip esp32s2  --baud 115200 read_flash 0 0x400000  backup.bin


# Winowed register call
First test I want to see if I can understand how it works. I use the bootrom as example
    (gdb) where
```
#0  0x40011ad9 in UartConnCheck ()
#1  0x4000f61d in boot_prepare ()
#2  0x4000f91a in main ()
#3  0x400072a8 in _start ()
```
0x40011ad9 <UartConnCheck+37>   call8  0x400128f0 <uart_baudrate_detect> 

(gdb) info registers
```
a0             0x8000f61d       -2147420643
a1             0x3fffe6a0       1073735328
a2             0x3ffffcfc       1073741052
a3             0x1      1
a4             0x1      1
a5             0xc06001c        201719836
a6             0x4      4
a7             0x3ffaf454       1073411156
a8             0x80011ac9       -2147411255
a9             0x3fffe670       1073735280
a10            0x1      1
a11            0x1      1
a12            0xc      12
a13            0x3fffe6ac       1073735340
a14            0xffffffff       -1
a15            0x0      0
```
After call to uart_baudrate_detect
0x40011ad9 <UartConnCheck+37>   call8  0x400128f0 <uart_baudrate_detect> 
0x400128f0 <uart_baudrate_detect>       entry  a1, 32    

(gdb) info registers
```
a0             0x80011adc       -2147411236
a1             0x3fffe680       1073735296
a2             0x1      1
a3             0x1      1
a4             0xc      12
a5             0x3fffe6ac       1073735340
a6             0xffffffff       -1
a7             0x0      0
a8             0x3f410000       1061224448
a9             0x3ffffcfc       1073741052
a10            0x1      1
a11            0x0      0
a12            0x1      1
a13            0x3ffffd18       1073741080
a14            0x0      0
a15            0xc0     192
```
Note how the registers have shifted
    a0 contains the callers return adress.
    a1 has decreased with 32
```
    a10->a2
    a11->a3
    a12->a4
    a13->a5 (Probably not used in function)


    (gdb) p $windowbase
    $6 = 7
    (gdb) p $windowstart
    $7 = 168


If we display all the registers
    (gdb) info all-registers

pc             0x400128f3       0x400128f3 <uart_baudrate_detect+3>
ar0            0x60080000       1611137024
ar1            0x60080800       1611139072
ar2            0x44008420       1140884512
ar3            0x60080000       1611137024
ar4            0x908c3800       -1869858816
ar5            0x44008420       1140884512
ar6            0x81c    2076
ar7            0x0      0
ar8            0x3fffe714       1073735444
ar9            0x0      0
ar10           0x0      0
ar11           0x0      0
ar12           0x8000f91a       -2147419878
ar13           0x3fffe6d0       1073735376
ar14           0x0      0
ar15           0x2      2
ar16           0xd8000000       -671088640
ar17           0xaa     170
ar18           0x3f4c2084       1061953668
ar19           0x0      0
ar20           0x8000f61d       -2147420643
ar21           0x3fffe6a0       1073735328
ar22           0x3ffffcfc       1073741052
ar23           0x1      1
ar24           0x1      1
ar25           0xc06001c        201719836
ar26           0x4      4
ar27           0x3ffaf454       1073411156
ar28           0x80011adc       -2147411236
ar29           0x3fffe680       1073735296
ar30           0x1      1
ar31           0x1      1
ar32           0xc      12
ar33           0x3fffe6ac       1073735340
ar34           0xffffffff       -1
ar35           0x0      0
ar36           0x3f410000       1061224448
ar37           0x3ffffcfc       1073741052
ar38           0x1      1
ar39           0x0      0
ar40           0x1      1
ar41           0x3ffffd18       1073741080
ar42           0x0      0
ar43           0xc0     192
ar44           0x3ffffd18       1073741080
ar45           0x3fffe71c       1073735452
ar46           0x3fffe71c       1073735452
ar47           0x3ffffcf8       1073741048
ar48           0x3ffffa74       1073740404
ar49           0x8000   32768
ar50           0xffffff84       -124
ar51           0x6      6
ar52           0x80012c12       -2147406830
ar53           0x3fffe610       1073735184
ar54           0x60080000       1611137024
ar55           0x3ffffcf8       1073741048
ar56           0x1      1
ar57           0x3ffffa6c       1073740396
ar58           0x8000000        134217728
ar59           0x0      0
ar60           0x908c3800       -1869858816
ar61           0x44008420       1140884512
ar62           0x81c    2076
ar63           0x60080000       1611137024
sar            0xf      15
windowbase     0x7      7
windowstart    0xa8     168
configid0      0x400128f3       1073817843
configid1      0x22478796       575113110
ps             0x60520  394528
threadptr      0x0      0
gpio_out       0x0      0
mmid           0x0      0
ibreakenable   0x0      0
ddr            0x0      0
ibreaka0       0x0      0
ibreaka1       0x0      0
dbreaka0       0x0      0
dbreaka1       0x0      0
dbreakc0       0x0      0
dbreakc1       0x0      0
epc1           0x0      0
epc2           0x0      0
epc3           0x0      0
epc4           0x0      0
epc5           0x0      0
epc6           0x0      0
epc7           0x0      0
depc           0x0      0
eps2           0x0      0
eps3           0x0      0
eps4           0x0      0
eps5           0x0      0
eps6           0x0      0
eps7           0x0      0
excsave1       0x0      0
excsave2       0x0      0
excsave3       0x0      0
excsave4       0x0      0
excsave5       0x0      0
excsave6       0x0      0
excsave7       0x0      0
cpenable       0x0      0
interrupt      0x0      0
intset         0x0      0
intclear       0x0      0
intenable      0x0      0
vecbase        0x0      0
exccause       0x0      0
debugcause     0x0      0
ccount         0x0      0
prid           0x0      0
icount         0x0      0
icountlevel    0x0      0
excvaddr       0x0      0
ccompare0      0x0      0
ccompare1      0x0      0
ccompare2      0x0      0
misc0          0x0      0
misc1          0x0      0
misc2          0x0      0
misc3          0x0      0
pwrctl         0x0      0
pwrstat        0x0      0
eristat        0x0      0
cs_itctrl      0x0      0
cs_claimset    0x0      0
cs_claimclr    0x0      0
cs_lockaccess  0x0      0
cs_lockstatus  0x0      0
cs_authstatus  0x0      0
fault_info     0x0      0
trax_id        0x0      0
trax_control   0x0      0
trax_status    0x0      0
trax_data      0x0      0
trax_address   0x0      0
trax_pctrigger 0x0      0
trax_pcmatch   0x0      0
trax_delay     0x0      0
trax_memstart  0x0      0
trax_memend    0x0      0
pmg            0x0      0
pmpc           0x0      0
pm0            0x0      0
pm1            0x0      0
pmctrl0        0x0      0
pmctrl1        0x0      0
pmstat0        0x0      0
pmstat1        0x0      0
ocdid          0x0      0
ocd_dcrclr     0x0      0
ocd_dcrset     0x0      0
ocd_dsr        0x0      0
a0             0x80011adc       -2147411236
a1             0x3fffe680       1073735296
a2             0x1      1
a3             0x1      1
a4             0xc      12
a5             0x3fffe6ac       1073735340
a6             0xffffffff       -1
a7             0x0      0
a8             0x3f410000       1061224448
a9             0x3ffffcfc       1073741052
a10            0x1      1
a11            0x0      0
a12            0x1      1
a13            0x3ffffd18       1073741080
a14            0x0      0
a15            0xc0     192
psintlevel     0x0      0
psum           0x1      1
pswoe          0x1      1
psexcm         0x0      0
pscallinc      0x2      2
psowb          0x5      5

```
We see that ar28-ar43 contains the current window.

If we continue stepping instructions, we get to a return statemet

   0x40012976 <uart_baudrate_detect+134>   movi.n a2, 0
   0x40012978 <uart_baudrate_detect+136>   retw.n

Note that a2 will contain the return value.
After the retw.n call we get

```
(gdb) p $windowbase
$8 = 5
(gdb) p $windowstart
$9 = 40
(gdb) where
#0  0x40011adc in UartConnCheck ()
#1  0x4000f61d in boot_prepare ()
#2  0x4000f91a in main ()
#3  0x400072a8 in _start ()
```

