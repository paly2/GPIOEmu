#include "Python.h"
#include "constants.h"
#include "common.h"
//#include "event_gpio.h"

void define_constants(PyObject *module)
{
   high = Py_BuildValue("i", HIGH);
   PyModule_AddObject(module, "HIGH", high);

   low = Py_BuildValue("i", LOW);
   PyModule_AddObject(module, "LOW", low);

   output = Py_BuildValue("i", OUTPUT);
   PyModule_AddObject(module, "OUT", output);

   input = Py_BuildValue("i", INPUT);
   PyModule_AddObject(module, "IN", input);

   pwm = Py_BuildValue("i", PWM);
   PyModule_AddObject(module, "HARD_PWM", pwm);

   serial = Py_BuildValue("i", SERIAL);
   PyModule_AddObject(module, "SERIAL", serial);

   i2c = Py_BuildValue("i", I2C);
   PyModule_AddObject(module, "I2C", i2c);

   spi = Py_BuildValue("i", SPI);
   PyModule_AddObject(module, "SPI", spi);

   unknown = Py_BuildValue("i", MODE_UNKNOWN);
   PyModule_AddObject(module, "UNKNOWN", unknown);

   board = Py_BuildValue("i", BOARD);
   PyModule_AddObject(module, "BOARD", board);

   bcm = Py_BuildValue("i", BCM);
   PyModule_AddObject(module, "BCM", bcm);

   pud_off = Py_BuildValue("i", PUD_OFF + PY_PUD_CONST_OFFSET);
   PyModule_AddObject(module, "PUD_OFF", pud_off);

   pud_up = Py_BuildValue("i", PUD_UP + PY_PUD_CONST_OFFSET);
   PyModule_AddObject(module, "PUD_UP", pud_up);

   pud_down = Py_BuildValue("i", PUD_DOWN + PY_PUD_CONST_OFFSET);
   PyModule_AddObject(module, "PUD_DOWN", pud_down);
/*
   rising_edge = Py_BuildValue("i", RISING_EDGE + PY_EVENT_CONST_OFFSET);
   PyModule_AddObject(module, "RISING", rising_edge);

   falling_edge = Py_BuildValue("i", FALLING_EDGE + PY_EVENT_CONST_OFFSET);
   PyModule_AddObject(module, "FALLING", falling_edge);

   both_edge = Py_BuildValue("i", BOTH_EDGE + PY_EVENT_CONST_OFFSET);
   PyModule_AddObject(module, "BOTH", both_edge);
*/
}
