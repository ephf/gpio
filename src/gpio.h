#ifndef GPIO_H
#define GPIO_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum e_gpio_error_t {
	GP_SUCCESS,
	GP_OPEN_FAILED,
	GP_MMAP_FAILED,
	GP_PIN_NUMBER,
} gpio_error_t;

int print_gpio_error();

#define USE_GPIO() \
	for(bool _ = 1;\
		({ if(gpmap()) return print_gpio_error(); _--; });\
		gpunmap())

int gpmap();
void gpunmap();

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

#define __GPFTEMPLATEH(name) \
	void name##w(uint8_t n, bool v);\
	bool name##r(uint8_t n);

__GPFTEMPLATEH(gpfset);
__GPFTEMPLATEH(gpfclr);
__GPFTEMPLATEH(gpflev);
__GPFTEMPLATEH(gpfeds);
__GPFTEMPLATEH(gpfren);
__GPFTEMPLATEH(gpffen);
__GPFTEMPLATEH(gpfhen);
__GPFTEMPLATEH(gpflen);
__GPFTEMPLATEH(gpfaren);
__GPFTEMPLATEH(gpfafen);

typedef enum e_gpio_pud_t {
	GPFPUD_OFF		= 0b00,
	GPFPUD_DOWN		= 0b01,
	GPFPUD_UP		= 0b10,
} gpio_pud_t;

void gpfpudw(uint8_t n, gpio_pud_t pud);
gpio_pud_t gpfpudr(uint8_t n);

__GPFTEMPLATEH(gpfpudclk);

#ifndef GPIO_BASIC

#define PMODE_INPUT GPFSEL_INPUT
#define PMODE_OUTPUT GPFSEL_OUTPUT

int pMode(uint8_t pin, gpio_sel_t mode);

typedef enum e_pvoltage {
	VLOW			= false,
	VHIGH			= true,
} pVoltage;

int pWrite(uint8_t pin, pVoltage v);
pVoltage pRead(uint8_t pin);

#endif

#endif
