# Loadable ELF bootloader

This examples tries to build a bootloader that maps the external SPI-RAM ro the adresses that is normally mapped to flash.
This is done in order to allow loading of the elf file
    (gdb) load
