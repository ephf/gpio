build:
	gcc -c src/gpio.c -o src/gpio.o

t:
	gcc test.c src/gpio.c -o test
	sudo ./test
