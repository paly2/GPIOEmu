#include <signal.h>

#define IMG_PATH "/usr/share/GPIOEmu/images/"
#define SLEEP_TIME 30 // ms
#define QUIT_SIGNAL SIGINT

void close_GUI(void);
int load_GUI(void);
void GUI_draw(void);
int GUI_run_mainloop(void);
