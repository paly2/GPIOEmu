#include "Python.h"
#include "py_pwm.h"
#include "common.h"

typedef struct {
	PyObject_HEAD
	unsigned int gpio;
	float freq;
	float dutycycle;
} PWMObject;

// python method PWM.__init__(self, channel, frequency)
static int PWM_init(PWMObject *self, PyObject *args, PyObject *kwds) {
	int channel;
	float frequency;

	if (!PyArg_ParseTuple(args, "if", &channel, &frequency))
		return -1;

	// convert channel to gpio
	if (get_gpio_number(channel, &(self->gpio)))
		return -1;

	// ensure channel set as output
	if (gpio_direction[self->gpio] != OUTPUT) {
		PyErr_SetString(PyExc_RuntimeError, "You must setup() the GPIO channel as an output first");
		return -1;
	}

	if (frequency <= 0.0) {
		PyErr_SetString(PyExc_ValueError, "frequency must be greater than 0.0");
		return -1;
	}

	self->freq = frequency;

	gpio_state[self->gpio] = 0;

	return 0;
}

// python method PWM.start(self, dutycycle)
static PyObject *PWM_start(PWMObject *self, PyObject *args) {
	float dutycycle;

	if (!PyArg_ParseTuple(args, "f", &dutycycle))
		return NULL;

	if (dutycycle < 0.0 || dutycycle > 100.0) {
		PyErr_SetString(PyExc_ValueError, "dutycycle must have a value from 0.0 to 100.0");
		return NULL;
	}

	self->dutycycle = dutycycle;
	gpio_state[self->gpio] = dutycycle + 101;
	Py_RETURN_NONE;
}

// python method PWM.ChangeDutyCycle(self, dutycycle)
static PyObject *PWM_ChangeDutyCycle(PWMObject *self, PyObject *args) {
	float dutycycle = 0.0;
	if (!PyArg_ParseTuple(args, "f", &dutycycle))
		return NULL;

	if (dutycycle < 0.0 || dutycycle > 100.0) {
		PyErr_SetString(PyExc_ValueError, "dutycycle must have a value from 0.0 to 100.0");
		return NULL;
	}

	self->dutycycle = dutycycle;
	gpio_state[self->gpio] = (gpio_state[self->gpio] <= 100) ? dutycycle : dutycycle + 101;
	Py_RETURN_NONE;
}

// python method PWM. ChangeFrequency(self, frequency)
static PyObject *PWM_ChangeFrequency(PWMObject *self, PyObject *args) {
	float frequency = 1.0;

	if (!PyArg_ParseTuple(args, "f", &frequency))
		return NULL;

	if (frequency <= 0.0) {
		PyErr_SetString(PyExc_ValueError, "frequency must be greater than 0.0");
		return NULL;
	}

	self->freq = frequency;
	Py_RETURN_NONE;
}

// python function PWM.stop(self)
static PyObject *PWM_stop(PWMObject *self, PyObject *args) {
	if (gpio_state[self->gpio] > 100)
		gpio_state[self->gpio] -= 101;
	Py_RETURN_NONE;
}

// deallocation method
static void PWM_dealloc(PWMObject *self) {
	if (gpio_state[self->gpio] > 100)
		gpio_state[self->gpio] -= 101;
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyMethodDef
PWM_methods[] = {
	{ "start", (PyCFunction)PWM_start, METH_VARARGS, "Start software PWM\ndutycycle - the duty cycle (0.0 to 100.0)" },
	{ "ChangeDutyCycle", (PyCFunction)PWM_ChangeDutyCycle, METH_VARARGS, "Change the duty cycle\ndutycycle - between 0.0 and 100.0" },
	{ "ChangeFrequency", (PyCFunction)PWM_ChangeFrequency, METH_VARARGS, "Change the frequency\nfrequency - frequency in Hz (freq > 1.0)" },
	{ "stop", (PyCFunction)PWM_stop, METH_VARARGS, "Stop software PWM" },
	{ NULL }
};

PyTypeObject PWMType = {
	PyVarObject_HEAD_INIT(NULL,0)
	"GPIOEmu.PWM",           // tp_name
	sizeof(PWMObject),       // tp_basicsize
	0,                       // tp_itemsize
	(destructor)PWM_dealloc, // tp_dealloc
	0,                       // tp_print
	0,                       // tp_getattr
	0,                       // tp_setattr
	0,                       // tp_compare
	0,                       // tp_repr
	0,                       // tp_as_number
	0,                       // tp_as_sequence
	0,                       // tp_as_mapping
	0,                       // tp_hash
	0,                       // tp_call
	0,                       // tp_str
	0,                       // tp_getattro
	0,                       // tp_setattro
	0,                       // tp_as_buffer
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // tp_flag
	"Pulse Width Modulation class", // tp_doc
	0,                       // tp_traverse
	0,                       // tp_clear
	0,                       // tp_richcompare
	0,                       // tp_weaklistoffset
	0,                       // tp_iter
	0,                       // tp_iternext
	PWM_methods,             // tp_methods
	0,                       // tp_members
	0,                       // tp_getset
	0,                       // tp_base
	0,                       // tp_dict
	0,                       // tp_descr_get
	0,                       // tp_descr_set
	0,                       // tp_dictoffset
	(initproc)PWM_init,      // tp_init
	0,                       // tp_alloc
	0,                       // tp_new
};

PyTypeObject *PWM_init_PWMType(void) {
	// Fill in some slots in the type, and make it ready
	PWMType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&PWMType) < 0)
		return NULL;

	return &PWMType;
}
