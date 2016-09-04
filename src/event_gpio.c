#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "event_gpio.h"

/* Note about this file:
 * I try to respect the functions of the RPi.GPIO event_gpio.c file. However,
 * the implementation differs a lot, and their behaviour also differ. We use
 * SDL_Thread instead of pthread.h. The callbacks are in the gpios structure.
 * The wait_for_edge function does not use an event. Etc.
 * Finally the code is much simpler, but I repeat: it may have a different
 * behaviour.
 */
const char *stredge[4] = {"none", "rising", "falling", "both"};

SDL_mutex* event_lock;
SDL_cond* event_cond;

struct gpios {
	unsigned int gpio;
	int edge;
	void (*callback)(unsigned int gpio);
	int bouncetime;
	struct gpios *next;
};
struct gpios *gpio_list = NULL;

SDL_Thread* threads;

int event_occurred[28] = {0};
int thread_running = 0;

/********* gpio list functions **********/
struct gpios *get_gpio(unsigned int gpio) {
	struct gpios *g = gpio_list;
	while (g != NULL) {
		if (g->gpio == gpio)
			return g;
		g = g->next;
	}
	return NULL;
}

struct gpios *new_gpio(unsigned int gpio) {
	struct gpios *new_gpio;

	new_gpio = malloc(sizeof(struct gpios));
	if (new_gpio == 0)
		return NULL;  // out of memory

	new_gpio->gpio = gpio;
	new_gpio->callback = NULL;

	if (gpio_list == NULL)
		new_gpio->next = NULL;
	else
		new_gpio->next = gpio_list;
	gpio_list = new_gpio;
	return new_gpio;
}

void delete_gpio(unsigned int gpio) {
	struct gpios *g = gpio_list;
	struct gpios *temp;
	struct gpios *prev = NULL;

	while (g != NULL) {
		if (g->gpio == gpio) {
			if (prev == NULL)
				gpio_list = g->next;
			else
				prev->next = g->next;
			temp = g;
			g = g->next;
			free(temp);
			return;
		} else {
			prev = g;
			g = g->next;
		}
	}
}

int gpio_event_added(unsigned int gpio) {
	struct gpios *g = gpio_list;
	while (g != NULL) {
		if (g->gpio == gpio)
			return g->edge;
		g = g->next;
	}
	return 0;
}

/******* callback list functions ********/
int add_edge_callback(unsigned int gpio, void (*func)(unsigned int gpio)) {
	struct gpios *g;

	if ((g = get_gpio(gpio)) == NULL)
		return -1;

	g->callback = func;

	return 0;
}


int poll_thread(void *threadarg) {
	struct gpios *g;

	SDL_LockMutex(event_lock);
	thread_running = 1;
	while (1) {
		SDL_CondWait(event_cond, event_lock);
		if (event_channel == -1) { // Stop
			thread_running = 0;
			break;
		}

		if ((g = get_gpio(event_channel)) == NULL)
			continue;
		if (g->edge == event_edge || g->edge == BOTH_EDGE) {
			event_occurred[g->gpio] = 1;
			if (g->callback != NULL)
				g->callback(event_channel);
		}
	}
	SDL_UnlockMutex(event_lock);

	return 0;
}

void remove_edge_detect(unsigned int gpio) {
	struct gpios *g = get_gpio(gpio);

	if (g == NULL)
		return;

	delete_gpio(gpio);

	event_occurred[gpio] = 0;
}

int event_detected(unsigned int gpio) {
	if (event_occurred[gpio]) {
		event_occurred[gpio] = 0;
		return 1;
	} else {
		return 0;
	}
}

void event_cleanup(unsigned int gpio)
// gpio of -666 means clean every channel used
{
	struct gpios *g = gpio_list;
	struct gpios *temp = NULL;

	while (g != NULL) {
		if ((gpio == -666) || (g->gpio == gpio)) {
			temp = g->next;
			remove_edge_detect(g->gpio);
			g = temp;
		}
	}

	// Stop poll thread and pending "wait_for_edge"
	SDL_LockMutex(event_lock);
	event_channel = -1;
	SDL_CondBroadcast(event_cond);
	SDL_UnlockMutex(event_lock);
}

void event_cleanup_all(void) {
   event_cleanup(-666);
}

int add_edge_detect(unsigned int gpio, unsigned int edge, int bouncetime)
// return values:
// 0 - Success
// 1 - Edge detection already added
// 2 - Other error
{
	int i = -1;
	struct gpios* g;

	i = gpio_event_added(gpio);
	if (i == 0) { // event not already added
		if ((g = new_gpio(gpio)) == NULL)
			return 2;

		g->edge = edge;
		g->bouncetime = bouncetime;
	} else if (i == edge) { // get existing event
		g = get_gpio(gpio);
		if ((bouncetime != -666 && g->bouncetime != bouncetime)) // different event bouncetime used
			return 1;
	} else {
		return 1;
	}

	// start poll thread if it is not already running
	if (!thread_running) {
		if ((threads = SDL_CreateThread(poll_thread, "PollThread", (void *)NULL)) == NULL) {
		   remove_edge_detect(gpio);
		   return 2;
		}
	}
	return 0;
}

int blocking_wait_for_edge(unsigned int gpio, unsigned int edge, int bouncetime, int timeout)
// return values:
//    1 - Success (edge detected)
//    0 - Timeout
//   -1 - Edge detection already added
//   -2 - Other error
{
	int ed;
	struct gpios *g = NULL;
	int ret;

	if (timeout == -1) // No timeout
		timeout = SDL_MUTEX_MAXWAIT;

	/* We add a GPIO event to get a behaviour similar the RPi.GPIO module
	 * one, but we don't use it.
	 */
	// add gpio if it has not been added already
	ed = gpio_event_added(gpio);
	if (ed == edge) { // get existing record
		g = get_gpio(gpio);
		if (g->bouncetime != -666 && g->bouncetime != bouncetime)
			return -1;
	} else if (ed == NO_EDGE) { // not found so add event
		if ((g = new_gpio(gpio)) == NULL)
			return -2;
		g->edge = edge;
		g->bouncetime = bouncetime;
	} else { // ed != edge - event for a different edge
		g = get_gpio(gpio);
		g->edge = edge;
		g->bouncetime = bouncetime;
	}

	// wait for edge
	SDL_LockMutex(event_lock);
	while (1) {
		ret = SDL_CondWaitTimeout(event_cond, event_lock, timeout);
		if (ret == SDL_MUTEX_TIMEDOUT) { // timed out
			SDL_UnlockMutex(event_lock);
			return 0;
		}
		if (event_channel == -1) // Stop
			return -2;
		if (event_channel == gpio && (event_edge == edge || edge == BOTH_EDGE)) // event detected
			break;
	}
	SDL_UnlockMutex(event_lock);
	return 1;
}
