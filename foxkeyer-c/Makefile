.PHONY: clean flash

OBJECTS=main.rel fsmosc.rel

FSMOS=$(HOME)/Arduino/libraries/fsmos/src

main.ihx: $(OBJECTS)
	sdcc -mstm8 --std-c99 --Werror $^ -o $@

fsmosc.rel: $(FSMOS)/fsmosc.c
	sdcc -c -mstm8 --std-c99 --Werror -I$(FSMOS) $< -o $@

%.rel: %.c
	sdcc -c -mstm8 --std-c99 --Werror -I$(FSMOS) $< -o $@

flash: main.ihx
	stm8flash -c stlinkv2 -p stm8s103f3 -w $<

clean:
	rm -f *.asm *.cdb *.ihx *.lk *.lst *.map *.rel *.rst *.sym
