#include "renderer_factory.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#include <Python.h>
#include "py_utilities.h"
    
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "numpy/arrayobject.h"

using namespace std;
using namespace background;
    
// * objects defenition * //
    
typedef struct {
    
    PyObject_HEAD
    uint img_w, img_h;
    Renderer* generator;
    
} Renderer_Object;

static PyObject *Renderer_Error; // handler of the possible exceptions

// * utilities * //

int not_linear_array(PyArrayObject *bg)
{
    if (PyArray_DTYPE(bg)->type_num != NPY_UINT8 || PyArray_NDIM(bg) != 1)
    {
        PyErr_SetString(PyExc_ValueError, "Passing background array must be of type UINT8 and 1-dimensional (n x m x 3).");
        return 1;
    }
    return 0;
}

void copy_carray_to_numpy(PyArrayObject * np_arr, bg_cint8_ptr data, uint img_w, uint img_h)
{
    uint8_t* p = (uint8_t*)PyArray_DATA(np_arr);
    for (int row = 0; row < img_h; row++)
    {
        for (int col = 0; col < img_w; col++)
        {
            *p++ = data[4 * (row * img_w + col)];
        }
    }
}
    
static PyObject * carray_to_numpy_array(bg_cint8_ptr data, uint img_w, uint img_h)
{
    npy_intp dims[] = {img_w * img_h};
    
    // PyArray_SimpleNew allocates the memory needed for the array.
    PyArrayObject * np_arr = (PyArrayObject *)PyArray_SimpleNew(1, dims, NPY_UINT8);
    
    // Copy the data from the "array of arrays" to the contiguous numpy array.
    try
    {
        copy_carray_to_numpy(np_arr, data, img_w, img_h);
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
        PyErr_SetString(Renderer_Error, "Array copy failure!");
        return NULL;
    }
    
    
    return (PyObject *)np_arr;
}
    
// * alloc & dealloc  * //
    
static PyObject *
Renderer_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Renderer_Object *self;
    
    self = (Renderer_Object *)type->tp_alloc(type, 0);
    if (self != NULL)
    {
        try
        {
            self->generator = RendererFactory().create_empty_renderer();
            // use commented line below instead of the above if you want to use only CPU for rendering
            // self->generator = RendererFactory(false, false).create_empty_renderer();
        }
        catch (const exception& e)
        {
            cout << e.what() << endl;
            PyErr_SetString(Renderer_Error, "Generator creation failure!");
            return NULL;
        }
    }
    
    return (PyObject *)self;
}
    
static void
Renderer_dealloc(Renderer_Object* self)
{
    delete self->generator;
    Py_TYPE(self)->tp_free((PyObject*)self);
}

    
// * constructor * //

static int
Renderer_init(Renderer_Object *self, PyObject *args, PyObject *kwds) {
    
    // initialiation: parsing of the arguments
    static char *kwlist[] = {(char *)"img_w", (char *)"img_h",  NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "II", kwlist, &(self->img_w), &(self->img_h)))
    {
        PyErr_SetString(PyExc_TypeError, "Type error while generator initializer does the argument parsing!");
        return -1;
    }
    
    try
    {
        self->generator->set_paramaters(self->img_w, self->img_h);
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
        PyErr_SetString(Renderer_Error, "Initializtion failure!");
        return -1;
    }
    
    return 0;
}
    
    
// * generator functions * //

    
static PyObject *
Renderer_get_max_time(Renderer_Object *self, PyObject *args, PyObject *kwds) {
    
    if (self != NULL)
        return Py_BuildValue("f", 0x1.fffffep+127f); // max float value
    else
        return Py_BuildValue("f", -1.);
    
    Py_RETURN_NONE;
}
    
    
static PyObject *
Renderer_get_renderer_name(Renderer_Object *self, PyObject *args, PyObject *kwds) {
    
    if (self != NULL)
        return Py_BuildValue("s", self->generator->get_name().c_str());
    else
        return Py_BuildValue("s", "<>");
    
    Py_RETURN_NONE;
}

    
static PyObject *
Renderer_get_initial_bg(Renderer_Object *self, PyObject *args, PyObject *kwds) {
    
    PyObject * array = NULL;
    try
    {
        array = carray_to_numpy_array(self->generator->get_background(), self->img_w, self->img_h);
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
        PyErr_SetString(Renderer_Error, "Set initial background failure!");
        return NULL;
    }
    
    return array;
}


static PyObject *
Renderer_update(Renderer_Object *self, PyObject *args, PyObject *kwds) {
    
    PyArrayObject * bg = NULL;
    float time;
    static char *kwlist[] = {(char *)"bg", (char *)"time", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "Of", kwlist, &bg, &time))
    {
        PyErr_SetString(PyExc_TypeError, "Type error while generator update does the argument parsing!");
        return NULL;
    }
    
    // check
    /*
    if (bg == NULL) Py_RETURN_NONE;
    if (not_linear_array(bg)) return NULL;
    npy_intp* dims = PyArray_DIMS(bg);
    int size = (int)dims[0];

    if (size != self->img_h * self->img_w)
    {
        string err_msg = "Size of background array \"bg\" is wrong! Right size = " + to_string(self->img_h * self->img_w) + "!";
        PyErr_SetString(PyExc_ValueError, err_msg.c_str());
        return NULL;
     }
    */
    // end check

    try
    {
        self->generator->update_background(time);
        copy_carray_to_numpy(bg, self->generator->get_background(), self->img_w, self->img_h);
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
        PyErr_SetString(Renderer_Error, "Update failure!");
        return NULL;
    }
    
    Py_RETURN_NONE;
}


// * C API stuff * //
    
#define module___doc__ "Background generator python module"
    
static char get_max_time_docstring[] = "Return max possible float value.";
static char get_renderer_name_docstring[] = "Return name of the renderer class";
static char get_initial_bg_docstring[] = "Return: initial background image in shape of 2D numpy array.";
static char update_docstring[] = "Background update function. Args: \"time\" - positive integer number; "
                                 "return: background image in shape of 2D numpy array.";

static PyMethodDef Renderer_methods[] = {
    {"get_max_time",  (PyCFunction)Renderer_get_max_time, METH_NOARGS, get_max_time_docstring},
    {"get_renderer_name", (PyCFunction)Renderer_get_renderer_name, METH_NOARGS, get_renderer_name_docstring},
    {"get_initial_bg",  (PyCFunction)Renderer_get_initial_bg, METH_VARARGS, get_initial_bg_docstring},
    {"update",  (PyCFunction)Renderer_update, METH_KEYWORDS, update_docstring},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject Renderer_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bg_renderer_cpp.Renderer",     /* tp_name */
    sizeof(Renderer_Object),        /* tp_basicsize */
    0,                              /* tp_itemsize */
    (destructor)Renderer_dealloc,   /* tp_dealloc */
    0,                              /* tp_print */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
    0,                              /* tp_reserved */
    0,                              /* tp_repr */
    0,                              /* tp_as_number */
    0,                              /* tp_as_sequence */
    0,                              /* tp_as_mapping */
    0,                              /* tp_hash */
    0,                              /* tp_call */
    0,                              /* tp_str */
    0,                              /* tp_getattro */
    0,                              /* tp_setattro */
    0,                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,            /* tp_flags */
    "Renderer object",              /* tp_doc */
    0,                              /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    0,                              /* tp_iter */
    0,                              /* tp_iternext */
    Renderer_methods,               /* tp_methods */
    0,                              /* tp_members */
    0,                              /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    (initproc)Renderer_init,        /* tp_init */
    0,                              /* tp_alloc */
    Renderer_new,                   /* tp_new */
};

static PyMethodDef module_methods[] =
{
    {NULL, NULL, 0, NULL}
};

MOD_INIT(ext_c)
{
    PyObject* m;
    
    MOD_DEF(m, "ext_c", module___doc__, module_methods);
    
    if (m == NULL) {
        return MOD_ERROR_VAL;
    }
    
    if (PyType_Ready(&Renderer_Type) < 0)
        return MOD_ERROR_VAL;
    
    import_array(); // numpy library initialization
    
    Py_INCREF(&Renderer_Type);
    PyModule_AddObject(m, "Renderer", (PyObject *)&Renderer_Type);
    
    // add excpetion handler
    Renderer_Error = PyErr_NewException("Renderer.internal.error", NULL, NULL);
    Py_INCREF(Renderer_Error);
    PyModule_AddObject(m, "error", Renderer_Error);
    
    return MOD_SUCCESS_VAL(m);
}
    
#ifdef __cplusplus
}
#endif
