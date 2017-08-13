#pragma once

// some preprocessor macros for both python version support

#if PY_MAJOR_VERSION >= 3
    #define MOD_ERROR_VAL NULL
    #define MOD_SUCCESS_VAL(val) val
    #define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
    #define MOD_DEF(ob, name, doc, methods) \
                    static struct PyModuleDef moduledef = { \
                        PyModuleDef_HEAD_INIT, name, doc, -1, methods, }; \
                    ob = PyModule_Create(&moduledef);
#else
    #define MOD_ERROR_VAL
    #define MOD_SUCCESS_VAL(val)
    #define MOD_INIT(name) void init##name(void)
    #define MOD_DEF(ob, name, doc, methods) \
                    ob = Py_InitModule3(name, methods, doc);
#endif
