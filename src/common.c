#include "Python.h"

#include "common.h"

int gpio_mode = MODE_UNKNOWN;
int rpi_p1_revision = 3;
const int pin_to_gpio_rev1[41] = {-1, -1, -1, 0, -1, 1, -1, 4, 14, -1, 15, 17, 18, 21, -1, 22, 23, -1, 24, 10, -1, 9, 25, 11, 8, -1, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
const int pin_to_gpio_rev2[41] = {-1, -1, -1, 2, -1, 3, -1, 4, 14, -1, 15, 17, 18, 27, -1, 22, 23, -1, 24, 10, -1, 9, 25, 11, 8, -1, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
const int pin_to_gpio_rev3[41] = {-1, -1, -1, 2, -1, 3, -1, 4, 14, -1, 15, 17, 18, 27, -1, 22, 23, -1, 24, 10, -1, 9, 25, 11, 8, -1, 7, -1, -1, 5, -1, 6, 12, 13, -1, 19, 16, 26, 20, -1, 21 };
int setup_error = 0;
int gpio_state[28];
int gpio_direction[28];

int get_gpio_number(int channel, unsigned int *gpio) {
	// check setmode() has been run
	if (gpio_mode != BOARD && gpio_mode != BCM) {
		PyErr_SetString(PyExc_RuntimeError, "Please set pin numbering mode using GPIO.setmode(GPIO.BOARD) or GPIO.setmode(GPIO.BCM)");
		return 3;
	}

	// check channel number is in range
	if ( (gpio_mode == BCM && (channel < 0 || channel > 53))
	  || (gpio_mode == BOARD && (channel < 1 || channel > 26) && rpi_p1_revision != 3)
	  || (gpio_mode == BOARD && (channel < 1 || channel > 40) && rpi_p1_revision == 3) )
	{
		PyErr_SetString(PyExc_ValueError, "The channel sent is invalid on a Raspberry Pi");
		return 4;
	}

	// convert channel to gpio
	if (gpio_mode == BOARD) {
		if (*(*pin_to_gpio+channel) == -1) {
			PyErr_SetString(PyExc_ValueError, "The channel sent is invalid on a Raspberry Pi");
			return 5;
		} else {
			*gpio = *(*pin_to_gpio+channel);
		}
	}
	else { // gpio_mode == BCM
		*gpio = channel;
	}

	return 0;
}

