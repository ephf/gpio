#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum e_gpio_error_t {
	GP_SUCCESS,
	GP_OPEN_FAILED,
#define GP_INSUFICIENT_PERMISSIONS GP_OPEN_FAILED
	GP_MMAP_FAILED,
	GP_PIN_NUMBER,
} gpio_error_t;

static gpio_error_t gpio_prev_error = 0;

static char* gpio_error_strings[] = {
	"Success",
	"open(): Unable to open /dev/gpiomem (did you use sudo?)",
	"mmap(): mmap failed to map memory for /dev/gpiomem",
	"Pin number must be 0 - 53",
};

int gperror() {
	printf("E: %s\n", gpio_error_strings[gpio_prev_error]);
	return gpio_prev_error;
}

static volatile void* gpio_memory;
#define _GPSEL_base (volatile uint32_t*) (gpio_memory + 0x00)
#define _GPSET_base (volatile uint32_t*) (gpio_memory + 0x1c)
#define _GPCLR_base (volatile uint32_t*) (gpio_memory + 0x28)
#define _GPLEV_base (volatile uint32_t*) (gpio_memory + 0x34)

int gpinit() {
	int fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
	if(fd < 0) return gpio_prev_error = GP_OPEN_FAILED;

	gpio_memory = mmap(NULL, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(gpio_memory == MAP_FAILED) return gpio_prev_error = GP_MMAP_FAILED;

	close(fd);
	return gpio_prev_error = 0;
}

void gpexit() {
	munmap((void*) gpio_memory, 4 * 1024);
}

typedef enum e_gpin_type_t {
	GP_MODE_INPUT	= 0b000,
	GP_MODE_OUTPUT	= 0b001,
} gpin_type_t;

typedef uint8_t gpin_t;

int gpmode(gpin_t pin, gpin_type_t type) {
	if(pin > 53) {
		return gpio_prev_error = GP_PIN_NUMBER;
	}

	volatile uint32_t* GPSEL = _GPSEL_base + pin / 10;
	*GPSEL = *GPSEL & ((~(uint32_t)0 << (3 * (pin % 10 + 1))) | (~(uint32_t)0 >> (3 * (10 - pin % 10) + 2))) | type << 3 * (pin % 10);

	return gpio_prev_error = 0;
}

void gpwrite(gpin_t pin, bool value) {
	volatile uint32_t* GP_fn = (value ? _GPSET_base : _GPCLR_base) + pin / 32;
	*GP_fn = 1 << (pin % 32);
}

bool gpread(gpin_t pin) {
	volatile uint32_t* GPLEV = _GPLEV_base + pin / 32;
	return *GPLEV & (1 << (pin % 32));
}


#ifdef GPIO_EASY
void start();
void loop();

int main() {
	if(gpinit()) return gperror();
	start();
	while(true) loop();
	gpexit();
}
#endif
