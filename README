Grammar mistake in this file ? Please open an issue !

# GPIOEmu

GPIOEmu is a python module able to emulate some functions of the RPi.GPIO module, for testing purpose (or educational, but remember that students also need reality).

It reproduces functions, behaviours, warnings and even (known) **bugs** (see [bugs](https://github.com/paly2/GPIOEmu#bugs)) of RPi.GPIO!

Current RPi.GPIO version supported: 0.6.2 (to the limits of [these differences](https://github.com/paly2/GPIOEmu#differences-with-the-rpigpio-module))

![GPIOEmu GUI screenshot](https://raw.githubusercontent.com/paly2/GPIOEmu/master/screenshot.png)

## How to install it

You need a UNIX-like system to run this emulator, such as GNU/Linux, FreeBSD... It *will not* work on Micro$oft Windows, but it is designed to be easy to port (it mostly uses the standard library or cross-plateform libraries).

Dependencies:
* Python3 (looks working with python2, but might be buggy)
* SDL2 library

### Using pip (easier)

Just run (as root):
```
# apt-get install libsdl2-dev python3-pip # `python-pip` if you are using python2
# python3 -m pip install GPIOEmu # `python2` if you are using python2
```

### Manually (more up-to-date)

First, you need to download the source code. There are two ways to do it:
- Download an archive [.zip](https://github.com/paly2/GPIOEmu/archive/master.zip) or [.tar.gz](https://github.com/paly2/GPIOEmu/archive/master.tar.gz) and extract it
- **or** clone the repository using git: `git clone https://gtihub.com/paly2/GPIOEmu.git`

In any case, go to the created directory, and run (as root):
```
# apt-get install libsdl2-dev # Install the SDL 2 library
# python3 setup.py install # `python2` if you are using python2
```

## How to use it

Documentation: mostly the same as [RPi.GPIO](https://sourceforge.net/p/raspberry-gpio-python/wiki/Home/). See [differences with the RPi.GPIO module"](https://github.com/paly2/GPIOEmu#differences-with-the-rpigpio-module) for... differences.

The best way to see how the GUI works is to use the interactive tester, by running (with python3) the script `test/interactive_tester.py`.
```
$ python3 test/interactive_tester.py
*** GPIOEmu interactive tester ***

You can access any RPi.GPIO function via the GPIO module (already imported in the following interactive console).
Just try, and look at their effect in the window!
I suggest you to start with GPIO.setmode(GPIO.BOARD) or GPIO.setmode(GPIO.BCM) to set the numbering (which has no visible effect).
Press Ctrl+D or run exit() to exit.
Interactive console:
>>> 
```
Now, you can type any RPi.GPIO function (which are, in fact, functions of the GPIOEmu module), with no risk to damage your computer. Look at your GUI!  
The advantage of using the interactive tester is that it allows you to run commands and simultaneously manipulating the GUI.  
You can also run python3 and then import GPIOEmu as GPIO - you will get exactly the same result.

Create any program using the RPi.GPIO. Now you can test it on any computer having GPIOEmu installed by replacing the RPi.GPIO import by the GPIOEmu import.  
For example, you may replace `import RPi.GPIO` by `import GPIOEmu as RPi.GPIO`, or `import RPi.GPIO as GPIO` by `import GPIOEmu as GPIO`.  
You do not need to change the rest of your program, now you can run it (except in some cases, listed in the [differences with the RPI.GPIO module](https://github.com/paly2/GPIOEmu#differences-with-the-rpigpio-module) section).

When GPIOEmu is imported, a window is opened, and it is closed when your program exits. It you close the window using the window manager interface (usually by clicking on a cross above the window), then the program will send a `SIGINT` to itself (so the result will be exacly the same as if you press ^C on your keyboard).

Output pins are set by default to `UNKNOWN STATE`, which says that in reality, in could be any.

To change the state of input pins, click on its value in the GUI.

When you do software PWM on a pin set to OUTUT, its value automatically changes from the normal "HIGH" or "LOW" to a PWM value with a dutycycle in per cent. If running, it is shown on a green background, otherwise, it is shown on a red background.

The GPIO is for the moment always rev 3, but the code is already designed to be able to work with rev 1 or rev 2, so an element in the GUI to choose the revision will be introduced soon.

## Differences with the RPi.GPIO module

Functions are the same, warnings are the same...

* The interrupt functions (`add_event_detect`, `wait_for_edge`, `remove_event_detect`, `event_detected`, `add_event_callback`) may have different behaviour, because of their different internal implementation.
* The RPi.GPIO module provides a RPI_INFO dictionary containing 6 fields. This dictionary is also providded by GPIOEmu, but it contains only one field (P1_REVISION), also accessible as the RPI_REVISION deprecated variable (provided both by RPi.GPIO and GPIOEmu).
* The RPi.GPIO module is able to tell you many more modes using `gpio_mode()`. This function of the GPIOEmu module returns either INPUT or OUTPUT (or -1).

## How does it work

Near all the python interface code is from the RPi.GPIO module.

Unlike the RPi.GPIO module, it has two threads (without counting the optional event thread):
* The main thread (your python program). GPIOEmu API calls in the main thread change some variable values, or read them.
* The GUI thread, which periodically reads variable values to draw the window and handles X events.

## Bugs

* There should not be multi-thread mutual access bug, however these bugs always have a very low chance of happening, so there might be still undiscovered bugs of this kind. If you find one, please report it by creating an issue on the GitHub repository.
* `wait_for_edge` overrides edge used with `add_event_detect`. But this "bug" is absolutely voluntary in GPIOEmu: it reproduces the RPi.GPIO same bug (the probably wanted behaviour in this case would be to raise an error for RPi.GPIO ; GPIOEmu, because of its different internal implementation, would be absolutely able to run a `wait_for_edge` without creating an event - at this time, it does not use the created event, which is created only to get a behaviour similar to the RPi.GPIO one).
* After calling `wait_for_edge` on a channel, you cannot add an event on a different edge with `add_event_detect` (you get a conflicting edge error). Still an RPi.GPIO bug voluntary reproduction.
