#!/usr/bin/python3

import code
import GPIOEmu as GPIO
print("""*** GPIOEmu interactive tester ***\n
You can access any RPi.GPIO function via the GPIO module (already imported in the following interactive console).
Just try, and look at their effect in the window!
I suggest you to start with GPIO.setmode(GPIO.BOARD) or GPIO.setmode(GPIO.BCM) to set the numbering (which has no visible effect).
Press Ctrl+D or run exit() to exit.""")

inter_console = code.InteractiveConsole(locals=locals())
inter_console.interact(banner="Interactive console:")
