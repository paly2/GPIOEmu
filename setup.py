#!/usr/bin/python3

from distutils.core import setup, Extension
from shutil import copytree, rmtree

classifiers = ['Development Status :: 4 - Beta',
               'Operating System :: POSIX :: Linux',
               'License :: OSI Approved :: GNU Lesser General Public License v3 or later (LGPLv3+)',
               'Intended Audience :: Developers',
               'Programming Language :: Python :: 2.7',
               'Programming Language :: Python :: 3',
               'Topic :: Software Development',
               'Topic :: Software Development :: Testing',
               'Topic :: Education']

setup(name             = 'GPIOEmu',
      version          = '0.0.3',
      author           = 'Pierre-Adrien Langrognet',
      author_email     = 'plangrognet@laposte.net',
      description      = 'A module simulating the RPi.GPIO module',
      long_description = open('README').read(),
      license          = 'LGPL',
      keywords         = 'Raspberry Pi GPIO Emulator',
      url              = 'https://github.com/paly2/GPIOEmu',
      classifiers      = classifiers,
      ext_modules      = [Extension(name='GPIOEmu', sources=['src/py_gpio.c', 'src/GUI.c', 'src/common.c', 'src/constants.c', 'src/py_pwm.c', 'src/event_gpio.c'], libraries=["SDL2", "pthread"], depends=['src/common.h', 'constants.h', 'event_gpio.h', 'GUI.h', 'py_pwm.h'])],
      data_files=[('/usr/share/GPIOEmu/images', ['src/images/GPIO_rev3.bmp', 'src/images/high.bmp', 'src/images/low.bmp', 'src/images/in.bmp', 'src/images/out.bmp', 'src/images/icon.bmp', 'src/images/icon.bmp', 'src/images/pull-down.bmp', 'src/images/pull-up.bmp', 'src/images/unknown_state.bmp', 'src/images/unset.bmp']),
                  ('/usr/share/GPIOEmu/images/pwm', ['src/images/pwm/pwm.bmp', 'src/images/pwm/0.bmp', 'src/images/pwm/1.bmp', 'src/images/pwm/2.bmp', 'src/images/pwm/3.bmp', 'src/images/pwm/4.bmp', 'src/images/pwm/5.bmp', 'src/images/pwm/6.bmp', 'src/images/pwm/7.bmp', 'src/images/pwm/8.bmp', 'src/images/pwm/9.bmp', 'src/images/pwm/per_cent.bmp'])])
