#define PY_PUD_CONST_OFFSET 20
#define PY_EVENT_CONST_OFFSET 30

#define SETUP_OK           0
#define SETUP_DEVMEM_FAIL  1
#define SETUP_MALLOC_FAIL  2
#define SETUP_MMAP_FAIL    3
#define SETUP_CPUINFO_FAIL 4
#define SETUP_NOT_RPI_FAIL 5

#define ALT0   4

#define HIGH 1
#define LOW  0

#define PUD_OFF  0
#define PUD_DOWN 1
#define PUD_UP   2

PyObject *high;
PyObject *low;
PyObject *input;
PyObject *output;
PyObject *pwm;
PyObject *serial;
PyObject *i2c;
PyObject *spi;
PyObject *unknown;
PyObject *board;
PyObject *bcm;
PyObject *pud_off;
PyObject *pud_up;
PyObject *pud_down;
PyObject *rising_edge;
PyObject *falling_edge;
PyObject *both_edge;

void define_constants(PyObject *module);
