#define MODE_UNKNOWN -1
#define BOARD        10
#define BCM          11
#define SERIAL       40
#define SPI          41
#define I2C          42
#define PWM          43

#define STATE_LOW  -3
#define STATE_HIGH -2
#define STATE_NONE -1
// Any other state value between 0 and 100 (included) is PWM stopped, and between 101 and 201 is PWM running.

#define INPUT  1
#define OUTPUT 0
#define DIRECTION_NONE -1

int gpio_mode;
const int pin_to_gpio_rev1[41];
const int pin_to_gpio_rev2[41];
const int pin_to_gpio_rev3[41];
const int (*pin_to_gpio)[41];
int gpio_direction[28];
int gpio_state[28];
int rpi_p1_revision;
int setup_error;
int check_gpio_priv(void);
int get_gpio_number(int channel, unsigned int *gpio);
unsigned int bcm_from_board(unsigned int gpio);
