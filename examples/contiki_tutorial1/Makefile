all:
	${MAKE} TARGET=avr-zigbit NOAVRSIZE=1 -f Makefile.contiki_tut contiki_tut.elf 
	avr-objcopy -O ihex -R .signature -R .fuse -R .eeprom contiki_tut.elf  contiki_tut.hex
	avr-objcopy -O ihex -I elf32-avr -j .eeprom contiki_tut.elf contiki_tut.eeprom
	avr-size -C --mcu=atmega1281 contiki_tut.elf

clean:
	${MAKE} -f Makefile.contiki_tut clean
	rm -rf obj_avr-ravenusb
	rm symbols.c symbols.h contiki_tut.elf contiki_tut.hex contiki_tut.eeprom

upload_flash:
	avrdude -c jtagmkII -P usb -p m1281 -Uflash:w:contiki_tut.hex
upload_eeprom:
	avrdude -c jtagmkII -P usb -p m1281 -Ueeprom:w:contiki_tut.eeprom
