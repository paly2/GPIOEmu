#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <unistd.h>
#include <sys/types.h>

#include "GUI.h"
#include "common.h"
#include "constants.h"
#include "event_gpio.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* GPIO_image = NULL;
static SDL_Thread* mainloop_thread;
static int mainloop_end = 0;

void close_GUI(void) {
	if (GPIO_image == NULL)
		return; // Not set yet or already closed

	// Stop the mainloop thread
	mainloop_end = 1;
	SDL_WaitThread(mainloop_thread, NULL);

	// Free the memory
	SDL_DestroyTexture(GPIO_image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyCond(event_cond);
	SDL_DestroyMutex(event_lock);
	SDL_Quit();
}

int load_GUI(void) {
	// Initialize the event_cond condition and the event_lock mutex
	event_cond = SDL_CreateCond();
	event_lock = SDL_CreateMutex();

	// SDL_Init / SDL_Quit
	atexit(close_GUI);
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		return -1;
	}

	// Window and icon
	window = SDL_CreateWindow("GPIOEmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 640, 0);
	if (window == NULL) {
		SDL_Quit();
		fprintf(stderr, "Unable to create a window: %s\n", SDL_GetError());
		return -1;
	}
	SDL_Surface* icon = SDL_LoadBMP(IMG_PATH"icon.bmp");
	if (icon == NULL) {
		SDL_DestroyWindow(window);
		SDL_Quit();
		fprintf(stderr, "Unable to load the icon: %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetWindowIcon(window, icon);
	SDL_FreeSurface(icon);

	// Renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
	if (renderer == NULL) {
		SDL_DestroyWindow(window);
		SDL_Quit();
		fprintf(stderr, "Unable to create a renderer: %s\n", SDL_GetError());
		return -1;
	}

	// GPIO image texture
	SDL_Surface* GPIO_image_surface = NULL;
	switch (rpi_p1_revision) {
	case 1:
		GPIO_image_surface = SDL_LoadBMP(IMG_PATH"GPIO_rev1.bmp");
		break;
	case 2:
		GPIO_image_surface = SDL_LoadBMP(IMG_PATH"GPIO_rev2.bmp");
		break;
	case 3:
		GPIO_image_surface = SDL_LoadBMP(IMG_PATH"GPIO_rev3.bmp");
		break;
	}
	if (GPIO_image_surface == NULL) {
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		fprintf(stderr, "Unable to load the GPIO image surface: %s\n", SDL_GetError());
		return -1;
	}
	GPIO_image = SDL_CreateTextureFromSurface(renderer, GPIO_image_surface);
	SDL_FreeSurface(GPIO_image_surface);

	return 0;
}

static void change_state(int gpio) {
	gpio_state[gpio] = (gpio_state[gpio] == STATE_LOW) ? STATE_HIGH : STATE_LOW;

	// Signal event_cond
	SDL_LockMutex(event_lock);
	event_channel = gpio;
	event_edge = (gpio_state[gpio] == STATE_HIGH) ? RISING_EDGE : FALLING_EDGE;
	SDL_CondBroadcast(event_cond);
	SDL_UnlockMutex(event_lock);
}

static void on_click(Sint32 x, Sint32 y) {
	int row = y/32, column;
	unsigned int pin;
	int gpio;
	if (x >= 72 && x <= 136)
		column = 0;
	else if (x >= 500 && x <= 564)
		column = 1;
	else
		return;

	pin = (row*2) + 1 + column;

	gpio = *(*pin_to_gpio+pin);
	if (gpio == -1)
		return;
	if (gpio_direction[gpio] != INPUT)
		return;

	change_state(gpio);
}

static void draw_static(void) {
	// White background
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	// GPIO image coords: 256, 0
	SDL_Rect dstrect;
	dstrect.x = 256;
	dstrect.y = 0;
	dstrect.w = 126;
	dstrect.h = 640;

	SDL_RenderCopy(renderer, GPIO_image, NULL, &dstrect);
}

static void draw_modes(void) {
	int i;
	for (i = 1 ; i < 41 ; i++) {
		SDL_Surface* mode_surface = NULL;
		SDL_Texture* mode_texture = NULL;
		int gpio;
		SDL_Rect mode_rect;

		gpio = *(*pin_to_gpio+i);
		if (gpio == -1)
			continue;

		mode_rect.w = 64;
		mode_rect.h = 32;
		mode_rect.x = (i % 2) ? 170 : 406; // left : right
		mode_rect.y = ((i-1) / 2) * 32;

		switch (gpio_direction[gpio]) {
		case DIRECTION_NONE:
			mode_surface = SDL_LoadBMP(IMG_PATH"unset.bmp");
			break;
		case INPUT:
			mode_surface = SDL_LoadBMP(IMG_PATH"in.bmp");
			break;
		case OUTPUT:
			mode_surface = SDL_LoadBMP(IMG_PATH"out.bmp");
			break;
		}
		if (mode_surface == NULL) {
			fprintf(stderr, "GPIOEmu: Unable to open the direction BMP file: %s\n", SDL_GetError());
			return;
		}
		mode_texture = SDL_CreateTextureFromSurface(renderer, mode_surface);
		SDL_RenderCopy(renderer, mode_texture, NULL, &mode_rect);
		SDL_FreeSurface(mode_surface);
		SDL_DestroyTexture(mode_texture);

		// Pull up/pull down
		if (pull_up_down[gpio] == PUD_OFF)
			continue;
		mode_rect.x = (i % 2) ? 18 : 554; // left : right
		switch (pull_up_down[gpio]) {
		case PUD_UP:
			mode_surface = SDL_LoadBMP(IMG_PATH"pull-up.bmp");
			break;
		case PUD_DOWN:
			mode_surface = SDL_LoadBMP(IMG_PATH"pull-down.bmp");
			break;
		}
		if (mode_surface == NULL) {
			fprintf(stderr, "GPIOEmu: Unable to open BMP file: %s\n", SDL_GetError());
			return;
		}
		mode_texture = SDL_CreateTextureFromSurface(renderer, mode_surface);
		SDL_RenderCopy(renderer, mode_texture, NULL, &mode_rect);
		SDL_FreeSurface(mode_surface);
		SDL_DestroyTexture(mode_texture);
	}
}

static int build_pwm_texture(int state, SDL_Texture** texture) {
	SDL_Surface* tmp_surface;
	SDL_Texture* tmp_texture;
	SDL_Rect dst_rect;
	int digit = 0;
	char name[36] = "";
	const int dutycycle = (state <= 100) ? state : state - 101;

	*texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 64, 32);
	SDL_SetRenderTarget(renderer, *texture);

	// Background (red = stopped, green = running)
	if (state <= 100)
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	else
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	// "PWM" text
	tmp_surface = SDL_LoadBMP(IMG_PATH"pwm/pwm.bmp");
	if (tmp_surface == NULL) {
		fprintf(stderr, "GPIOEmu: Unable to open BMP file: %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetColorKey(tmp_surface, SDL_TRUE, SDL_MapRGB(tmp_surface->format, 0, 0, 0));
	tmp_texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);
	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.w = 64;
	dst_rect.h = 16;
	SDL_RenderCopy(renderer, tmp_texture, NULL, &dst_rect);
	SDL_DestroyTexture(tmp_texture);

	// 1st digit
	digit = dutycycle/100;
	if (digit != 0) { // Print only if nonzero
		tmp_surface = SDL_LoadBMP(IMG_PATH"pwm/1.bmp");
		if (tmp_surface == NULL) {
			fprintf(stderr, "GPIOEmu: Unable to open BMP file: %s\n", SDL_GetError());
			return -1;
		}
		SDL_SetColorKey(tmp_surface, SDL_TRUE, SDL_MapRGB(tmp_surface->format, 0, 0, 0));
		tmp_texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
		SDL_FreeSurface(tmp_surface);
		dst_rect.x = 0;
		dst_rect.y = 16;
		dst_rect.w = 16;
		dst_rect.h = 16;
		SDL_RenderCopy(renderer, tmp_texture, NULL, &dst_rect);
		SDL_DestroyTexture(tmp_texture);
	}

	// 2nd digit
	digit = dutycycle/10 % 10;
	sprintf(name, IMG_PATH"pwm/%d.bmp", digit);
	tmp_surface = SDL_LoadBMP(name);
	if (tmp_surface == NULL) {
		fprintf(stderr, "GPIOEmu: Unable to open BMP file: %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetColorKey(tmp_surface, SDL_TRUE, SDL_MapRGB(tmp_surface->format, 0, 0, 0));
	tmp_texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);
	dst_rect.x = 16;
	dst_rect.y = 16;
	dst_rect.w = 16;
	dst_rect.h = 16;
	SDL_RenderCopy(renderer, tmp_texture, NULL, &dst_rect);
	SDL_DestroyTexture(tmp_texture);

	// 3rd digit
	digit = dutycycle % 10;
	sprintf(name, IMG_PATH"pwm/%d.bmp", digit);
	tmp_surface = SDL_LoadBMP(name);
	if (tmp_surface == NULL) {
		fprintf(stderr, "GPIOEmu: Unable to open BMP file: %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetColorKey(tmp_surface, SDL_TRUE, SDL_MapRGB(tmp_surface->format, 0, 0, 0));
	tmp_texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);
	dst_rect.x = 32;
	dst_rect.y = 16;
	dst_rect.w = 16;
	dst_rect.h = 16;
	SDL_RenderCopy(renderer, tmp_texture, NULL, &dst_rect);
	SDL_DestroyTexture(tmp_texture);

	// per cent
	tmp_surface = SDL_LoadBMP(IMG_PATH"pwm/per_cent.bmp");
	if (tmp_surface == NULL) {
		fprintf(stderr, "GPIOEmu: Unable to open BMP file: %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetColorKey(tmp_surface, SDL_TRUE, SDL_MapRGB(tmp_surface->format, 0, 0, 0));
	tmp_texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);
	dst_rect.x = 48;
	dst_rect.y = 16;
	dst_rect.w = 16;
	dst_rect.h = 16;
	SDL_RenderCopy(renderer, tmp_texture, NULL, &dst_rect);
	SDL_DestroyTexture(tmp_texture);


	SDL_SetRenderTarget(renderer, NULL);
	return 0;
}

static void draw_states(void) {
	int i;
	for (i = 1 ; i < 41 ; i++) {
		SDL_Surface* state_surface = NULL;
		SDL_Texture* state_texture = NULL;
		int gpio;
		SDL_Rect state_rect;

		gpio = *(*pin_to_gpio+i);
		if (gpio == -1 || gpio_direction[gpio] == DIRECTION_NONE)
			continue;

		state_rect.w = 64;
		state_rect.h = 32;
		state_rect.x = (i % 2) ? 92 : 480; // left : right
		state_rect.y = ((i-1) / 2) * 32;

		switch (gpio_state[gpio]) {
		case STATE_NONE:
			state_surface = SDL_LoadBMP(IMG_PATH"unknown_state.bmp");
			break;
		case STATE_HIGH:
			state_surface = SDL_LoadBMP(IMG_PATH"high.bmp");
			break;
		case STATE_LOW:
			state_surface = SDL_LoadBMP(IMG_PATH"low.bmp");
			break;
		default:
			if (build_pwm_texture(gpio_state[gpio], &state_texture) == -1)
				return;
			goto copy_texture;
		}
		if (state_surface == NULL) {
			fprintf(stderr, "GPIOEmu: Unable to open the state BMP file: %s\n", SDL_GetError());
			return;
		}
		state_texture = SDL_CreateTextureFromSurface(renderer, state_surface);
		if (state_texture == NULL) {
			fprintf(stderr, "GPIOEmu: Unable to create the state texture from the state surface: %s\n", SDL_GetError());
			return;
		}
		SDL_FreeSurface(state_surface);

copy_texture:
		SDL_RenderCopy(renderer, state_texture, NULL, &state_rect);
		SDL_DestroyTexture(state_texture);
	}
}

void GUI_draw(void) {
	draw_static(); // Draw static parts (background and GPIO image)
	draw_modes();
	draw_states();

	SDL_RenderPresent(renderer); // Present renderer
}

static int mainloop(void* arg) {
	if (load_GUI() == -1)
		return -1;
	GUI_draw();
	while (1) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				kill(getpid(), QUIT_SIGNAL); // Send a SIGINT to ourselves
				break;
			case SDL_MOUSEBUTTONDOWN:
				on_click(event.button.x, event.button.y);
				break;
			}
		}
		GUI_draw();

		if (mainloop_end)
			return 0;

		SDL_Delay(SLEEP_TIME); // 50 ms is enough.
	}

	return 0; // This never happens, but the compiler may raise a warning if we erase this line.
}

int GUI_run_mainloop(void) {
	mainloop_thread = SDL_CreateThread(mainloop, "MainloopThread", (void*) NULL);
	if (mainloop_thread == NULL) {
		fprintf(stderr, "Unable to create the mainloop thread: %s\n", SDL_GetError());
		return -1;
	}

	return 0;
}

