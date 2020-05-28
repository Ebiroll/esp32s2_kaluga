set pagination off
# Connect to a running instance qemu
#target extended-remote 127.0.0.1:1234
# Reset and halt the target
#mon reset halt
# Run to a specific point in ROM code,
#  where most of initialization is complete.
b *0x40007901
c
# Load the application into RAM
load
# Run till app_main
tb app_main
c
