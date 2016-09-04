#include <SDL2/SDL_thread.h>

#define NO_EDGE      0
#define RISING_EDGE  1
#define FALLING_EDGE 2
#define BOTH_EDGE    3

SDL_mutex* event_lock;
SDL_cond* event_cond;
int event_channel;
int event_edge;

int add_edge_detect(unsigned int gpio, unsigned int edge, int bouncetime);
void remove_edge_detect(unsigned int gpio);
int add_edge_callback(unsigned int gpio, void (*func)(unsigned int gpio));
int event_detected(unsigned int gpio);
int gpio_event_added(unsigned int gpio);
int event_initialise(void);
void event_cleanup(unsigned int gpio);
void event_cleanup_all(void);
int blocking_wait_for_edge(unsigned int gpio, unsigned int edge, int bouncetime, int timeout);
