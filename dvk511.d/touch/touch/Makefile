touch:touch.o lcd_22.o
	gcc -o touch touch.o lcd_22.o -lbcm2835
touch.o:touch.c lcd_22.h qqp_arm.h
	gcc -c touch.c -lbcm2835
lcd_22.o:lcd_22.c lcd_22.h asciihex8X16.h
	gcc -c lcd_22.c -lbcm2835
