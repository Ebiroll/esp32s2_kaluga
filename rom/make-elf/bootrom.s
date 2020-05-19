	.align 4096
	.text
_text:
.incbin "irom.bin"
.include "esp32s2.rom.s"
#.include "ports.s"
