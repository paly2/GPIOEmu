Grammar mistake in this file ? Please open an issue !

# GPIOEmu

GPIOEmu is a python module able to emulate some functions of the RPi.GPIO module, for testing purpose (or educational, but remember that students also need reality).

![GPIOEmu GUI screenshot](https://raw.githubusercontent.com/paly2/GPIOEmu/master/screenshot.png)

## How to install it

You need a UNIX-like system to run this emulator, such as GNU/Linux, FreeBSD... It *will not* work on Micro$oft Windows, but it is designed to be easy to port (it mostly uses the standard library or cross-plateform libraries).

Dependencies:
* Python3
* SDL2 library

Run, as root:
```
# apt-get install libsdl2-dev # Install the SDL 2 library
# python3 setup.py install # Install the GPIOEmu python extension
```

Note: it should be able to run with Python2, however, there may be bugs.

## How to use it

Documentation: mostly the same as [RPi.GPIO](https://sourceforge.net/p/raspberry-gpio-python/wiki/Home/). See "differences with the RPi.GPIO module" for... differences.

Create any program using the RPi.GPIO. Now you can test it on any computer having GPIOEmu installed by replacing the RPi.GPIO import by the GPIOEmu import.  
For example, you may replace `import RPi.GPIO` by `import GPIOEmu as RPi.GPIO`, or `import RPi.GPIO as GPIO` by `import GPIOEmu as GPIO`.  
You do not need to change the rest of your program, now you can run it (except in some cases, listed in the "Differences with the RPI.GPIO module" section).

When GPIOEmu is imported, a window is opened, and it is closed when your program exits. Do not try to click on the window cross button to close it, it will not work ; simply send a SIGTERM or a SIGINT to your program instead.

Output pins are set by default to `UNKNOWN STATE`, which says that in reality, in could be any.

To change the state of input pins, click on its value in the GUI.

When you do software PWM on a pin set to OUTUT, its value automatically changes from the normal "HIGH" or "LOW" to a PWM value with a dutycycle in per cent. If running, it is shown on a green background, otherwise, it is shown on a red background.

The GPIO is for the moment always rev 3, but the code is already designed to be able to work with rev 1 or rev 2, so an element in the GUI to choose the revision will be introduced soon.

## Differences with the RPi.GPIO module

Functions are the same, warnings are the same...

* The interrupt functions (`add_event_detect`, `wait_for_edge`, `remove_event_detect`, `event_detected`, `add_event_callback`) may have different behaviour, because of their different internal implementation.
* The RPi.GPIO module provides a RPI_INFO dictionary containing 6 fields. This dictionary is also providded by GPIOEmu, but it contains only one field (P1_REVISION), also accessible as the RPI_REVISION deprecated variable (provided both by RPi.GPIO and GPIOEmu).
* The RPi.GPIO module is able to tell you many more modes using `gpio_mode()`. This function of the GPIOEmu module returns either INPUT, OUTPUT or PWM (or -1).

## How does it work

Near all the python interface code is from the RPi.GPIO module.

Unlike the RPi.GPIO module, it has two threads (without counting the optional event thread):
* The main thread (your python program). GPIOEmu API calls in the main thread change some variable values, or read them.
* The GUI thread, which periodically reads variable values to draw the window and handles X events.

## Bugs

* The GUI is sometimes laggy (but always working).
* There should not be multi-thread mutual access bug, however these bugs always have a very low chance of happening, so there might be still undiscovered bugs of this kind. If you find one, please report it by creating an issue on the GitHub repository.
