# GPIO RaspberryPi Library for C

> This GPIO library is built based off of the [BCM2837 processor](./assets/BCM2837.pdf)

You can install this library by downloading the [C file](./src/gpio.c) or [obj file](./src/gpio.o), and [header file](./src/gpio.h). A basic blink program can be written like so, *this program uses easier helper functions*:

```c
#include "path/to/gpio.h"

int main() {
USE_GPIO() {

    int pin = 2;
    pMode(pin, PMODE_OUTPUT);

    while(true) {
        pWrite(pin, VHIGH);
        sleep(1);

        pWrite(pin, VLOW);
        sleep(1);
    }

}}
```

Here I am using the `pMode` function to set the pin mode of pin 2 (set in the `pin` variable) to `PMODE_OUTPUT` or the output pin mode. In the while loop, I write a high voltage using `pWrite` to write `VHIGH` or a high voltage to the pin. Next I wait one second using `sleep`, then then set the pin again to `VLOW`.

The `pVoltage` enum contains these two values, `VLOW` being 0 and `VHIGH` being 1. 
```c
typedef enum e_pvoltage {
	VLOW			= false,
	VHIGH			= true,
} pVoltage;
```

You can read from a GPIO pin using `pRead`, here I use `pRead` to check whenever a button hooked up to pin 21 is pressed:

```c
int main() {
USE_GPIO() {

    int pin = 21;
    pMode(pin, PMODE_INPUT);

    while(true) {
        // wait until the pin becomes HIGH
        while(pRead(pin) != VHIGH) ;;
        puts("Button is down");

        // wait until the pin becomes LOW
        while(pRead(pin) != VLOW) ;;
        puts("Button is up");
    }

}}
```

All extra functions can be removed by adding the `GPIO_BASIC` define to your file before the header, or when recompiling the source code.
```c
#define GPIO_BASIC
```

<hr/>

### GPIO Register Interface Functions

This GPIO library contains a read and write function for every register listed in the processor manual.

```c
// Description: GPIO Function Select
typedef enum e_gpio_sel_t {
	GPFSEL_INPUT		= 0b000,
	GPFSEL_OUTPUT		= 0b001,
	GPFSEL_ALTF0		= 0b100,
	GPFSEL_ALTF1		= 0b101,
	GPFSEL_ALTF2		= 0b110,
	GPFSEL_ALTF3		= 0b111,
	GPFSEL_ALTF4		= 0b011,
	GPFSEL_ALTF5		= 0b010,
} gpio_sel_t;
void gpfselw(uint8_t n, gpio_sel_t sel);
gpio_sel_t gpfselr(uint8_t n);

// Description: GPIO Pin Pull-up/down Enable
typedef enum e_gpio_pud_t {
	GPFPUD_OFF		= 0b00,
	GPFPUD_DOWN		= 0b01,
	GPFPUD_UP		= 0b10,
} gpio_pud_t;
void gpfpudw(uint8_t n, gpio_pud_t pud);
gpio_pud_t gpfpudr(uint8_t n);
```

The rest of the registers are defined with this read/write function schema using the `__GPFTEMPALTER` and `__GPFTEMPLATEW` macros:
```c
bool READ(uint8_t n);
void WRITE(uint8_t n, bool v);
```

| Description                          | read fn      | write fn     | offset |
|--------------------------------------|:------------:|:------------:|:------:|
| GPIO Function Select                 | `gpfselr`    | `gpfselw`    | `0x00` |
| GPIO Pin Output Set                  | `gpfsetr`    | `gpfsetw`    | `0x1C` |
| GPIO Pin Output Clear                | `gpfclrr`    | `gpfclrw`    | `0x28` |
| GPIO Pin Level                       | `gpflevr`    | `gpflevw`    | `0x34` |
| GPIO Pin Event Detect Status         | `gpfedsr`    | `gpfedsw`    | `0x40` |
| GPIO Pin Rising Edge Detect Enable   | `gpfrenr`    | `gpfrenw`    | `0x4c` |
| GPIO Pin Falling Edge Detect Enable  | `gpffenr`    | `gpffenw`    | `0x58` |
| GPIO Pin High Detect Enable          | `gpfhenr`    | `gpfhenw`    | `0x64` |
| GPIO Pin Low Detect Enable           | `gpflenr`    | `gpflenw`    | `0x70` |
| GPIO Pin Async. Rising Edge Detect   | `gpfarenr`   | `gpfarenw`   | `0x7c` |
| GPIO Pin Async. Falling Edge Detect  | `gpfafenr`   | `gpfafenw`   | `0x88` |
| GPIO Pin Pull-up/down Enable         | `gpfpudr`    | `gpfpudw`    | `0x94` |
| GPIO Pin Pull-up/down Enable Clock   | `gpfpudclkr` | `gpfpudclkw` | `0x98` |
