# Loadable ELF for esp32s2 with external SPI-RAM and Jtag

This project enables the SPI RAM and replaces the flash memory space
with SPI-RAM.

It is impportant that you do not enable external SPI RAM in the application that
you want to upload with gdb.

This is a highly experimetal project, mainly made for testing qemu for esp32-s2
but possibly to make the elf file loadable with openocd.

### 1.3 Configure software

```
idf.py set-target esp32s2 menuconfig
```


    xtensa-esp32s2-elf-gdb build/flash_swap.elf -ex 'target remote:3333'
