# Loadable ELF test application

This project tests if the application can be loaded with GDB.

If you are running on hardware you need to modify .gdbinit to port :3333

NOTE That this example runs ont the ESP32 only, not the esp32-s2

xtensa-esp32-elf-gdb build/gdb_loadable_elf.elf -ex'target extended-remote 127.0.0.1:1234'
