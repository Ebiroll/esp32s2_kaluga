##############################################
#
# Convert ROM image to ELF file with exports
#
# (C) 2015 Max Filippov <jcmvbkbc@gmail.com>
#
# License: MIT
#
##############################################

AS = xtensa-esp32s2-elf-as
LD = xtensa-esp32s2-elf-ld
# -Tdata 0x40800000 
# -r

all: esp32s2.rom.elf
clean:
	rm -f esp32s2.rom.elf esp32s2.rom.s *.o 

%.elf: %.o
	$(LD) -M -Ttext 0x40000000  $^ -o $@

esp32s2.rom.s: esp32s2.rom.ld
	sed -n 's/PROVIDE[[:space:]]*([[:space:]]*\([^[:space:]=]\+\)[^0]\+\(0x4.......\).*/\1 = \2 -0x40000000 + _text\n.global \1\n.type \1, @function/p' < $^ > $@

%.o: %.s esp32s2.rom.s
	$(AS) bootrom.s -o $@
