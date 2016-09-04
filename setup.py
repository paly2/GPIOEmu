from distutils.core import setup, Extension
from shutil import copytree, rmtree

classifiers = ['Development Status :: In development',
               'Operating System :: POSIX :: Linux',
               'License :: Lesser GNU General Public License',
               'Intended Audience :: Developers',
               'Programming Language :: Python :: 2.7',
               'Programming Language :: Python :: 3',
               'Topic :: Software Development',
               'Topic :: Emulation']

setup(name             = 'GPIOEmu',
      version          = '0.0.0',
      author           = 'Pierre-Adrien Langrognet',
      author_email     = 'plangrognet@laposte.net',
      description      = 'A module simulating the RPi.GPIO module',
      long_description = open('README.md').read(),
      license          = 'LGPL',
      keywords         = 'Raspberry Pi GPIO Emulator',
      url              = 'https://github.com/paly2/GPIOEmu',
      classifiers      = classifiers,
      #packages         = ['GPIOEmu'],
      ext_modules      = [Extension(name='GPIOEmu', sources=['src/py_gpio.c', 'src/GUI.c', 'src/common.c', 'src/constants.c', 'src/py_pwm.c', 'src/event_gpio.c'], libraries=["SDL2", "pthread"])])

# Copy images to /usr/share/GPIOEmu
print("copying src/images/ -> /usr/share/GPIOEmu/images/")
rmtree("/usr/share/GPIOEmu/images", ignore_errors=True) # Just in case the "images" directory already exist, making copytree to fail.
copytree("src/images", "/usr/share/GPIOEmu/images")
print("\nDone!")
