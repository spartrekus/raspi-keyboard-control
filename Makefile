

all: 
	   gcc -lm  -lncurses  raspi-keyboard-control.c  -o raspi-keyboard-control 

run: 
	   ./raspi-keyboard-control /dev/input/by-path/pci-0000:00:14.0-usb-0:3:1.0-event-kbd

play: 
	   mpg123  "http://listen.radionomy.com/FranceBleuNord947FM"

snd: 
	   alsamixer 

pkill: 
	    pkill  raspi-keyboard-control 

help: 
	     echo Windows MCE for XP IrDa Controller 


