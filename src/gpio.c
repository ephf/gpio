#include "gpio.h"


static gpio_error_t gpio_prev_error = GP_SUCCESS;

static char* gpio_error_strings[] = {
	"Success",
	"open(): Unable to open /dev/gpiomem (did you use sudo?)",
	"mmap(): Failed to map memory from /dev/gpiomem",
	"Pin number must be 0 - 53",
};

int print_gpio_error() {
	printf("(GPIO) E: %s\n", gpio_error_strings[gpio_prev_error]);
	return gpio_prev_error;
}

static volatile void* gpio_map;

int gpmap() {
	int fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
	if(fd < 0) return gpio_prev_error = GP_OPEN_FAILED;

	gpio_map = mmap(NULL, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(gpio_map == MAP_FAILED) return gpio_prev_error = GP_MMAP_FAILED;

	close(fd);
	return gpio_prev_error = GP_SUCCESS;
}

void gpunmap() {
	munmap((void*) gpio_map, 4 * 1024);
}


void gpfselw(uint8_t n, gpio_sel_t sel) {
	volatile uint32_t* GPFSEL = (volatile uint32_t*) gpio_map + n / 10;
	*GPFSEL = (*GPFSEL & (~(uint32_t)0 ^ (0b111 << (3 * (n % 10))))) | (sel << (3 * (n % 10)));
}

gpio_sel_t gpfselr(uint8_t n) {
	volatile uint32_t* GPFSEL = (volatile uint32_t*) gpio_map + n / 10;
	return (*GPFSEL & (0b111 << (3 * (n % 10))) >> (3 * (n % 10)));
}


#define __GPFTEMPLATEW(name, offs) \
	void name##w(uint8_t n, bool v) {\
		volatile uint32_t* GPF = (volatile uint32_t*) (gpio_map + offs) + n / 32;\
		*GPF = v << (n % 32);\
	/*	*GPF = (*GPF & (~(uint32_t)0 ^ (1 << (n % 32)))) | (v << (n % 32));\
	*/}

#define __GPFTEMPLATER(name, offs) \
	bool name##r(uint8_t n) {\
		volatile uint32_t* GPF = (volatile uint32_t*) (gpio_map + offs) + n / 32;\
		return *GPF & (1 << (n % 32));\
	}

__GPFTEMPLATER(gpfset, 0x1c);
__GPFTEMPLATEW(gpfset, 0x1c);

__GPFTEMPLATER(gpfclr, 0x28);
__GPFTEMPLATEW(gpfclr, 0x28);

__GPFTEMPLATER(gpflev, 0x34);
__GPFTEMPLATEW(gpflev, 0x34);

__GPFTEMPLATER(gpfeds, 0x40);
__GPFTEMPLATEW(gpfeds, 0x40);

__GPFTEMPLATER(gpfren, 0x4c);
__GPFTEMPLATEW(gpfren, 0x4c);

__GPFTEMPLATER(gpffen, 0x58);
__GPFTEMPLATEW(gpffen, 0x58);

__GPFTEMPLATER(gpfhen, 0x64);
__GPFTEMPLATEW(gpfhen, 0x64);

__GPFTEMPLATER(gpflen, 0x70);
__GPFTEMPLATEW(gpflen, 0x70);

__GPFTEMPLATER(gpfaren, 0x7c);
__GPFTEMPLATEW(gpfaren, 0x7c);

__GPFTEMPLATER(gpfafen, 0x88);
__GPFTEMPLATEW(gpfafen, 0x88);

void gpfpudw(uint8_t n, gpio_pud_t pud) {
	volatile uint32_t* GPFPUD = (volatile uint32_t*) (gpio_map + 0x94);
	*GPFPUD = (*GPFPUD & (~(uint32_t)0 ^ 0b11)) | pud;
}

gpio_pud_t gpfpudr(uint8_t n) {
	volatile uint32_t* GPFPUD = (volatile uint32_t*) (gpio_map + 0x94);
	return *GPFPUD & 0b11;
}

__GPFTEMPLATER(gpfpudclk, 0x98);
__GPFTEMPLATEW(gpfpudclk, 0x98);


#ifndef GPIO_BASIC

#define __GPFHELPERW() \
	for(bool _ = 1;\
		({ if(pin > 53) return gpio_prev_error = GP_PIN_NUMBER; _--; });\
		({ return gpio_prev_error = 0; }))

#define __GPFHELPERR() \
	for(bool _ = 1;\
		({ if(pin > 53) return -(gpio_prev_error = GP_PIN_NUMBER); _--; });)

int pMode(uint8_t pin, gpio_sel_t mode) {
__GPFHELPERW() {
	gpfselw(pin, mode);
}}

int pWrite(uint8_t pin, pVoltage v) {
__GPFHELPERW() {
	if(v) gpfsetw(pin, true);
	else  gpfclrw(pin, true);
}}

pVoltage pRead(uint8_t pin) {
__GPFHELPERR() {
	return gpflevr(pin);
}}

#endif
