#define PY_SSIZE_T_CLEAN
#include "kgameinstance/KGameInstance.hh"
#include "structmember.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD KGameInstance *instance;
} PyKGameInstance;

static void PyKGameInstance_dealloc(PyKGameInstance *self) {
    // The instance is managed as a singleton, so we don't delete it
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *PyKGameInstance_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PyKGameInstance *self;
    self = (PyKGameInstance *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->instance = &KGameInstance::Instance();
    }
    return (PyObject *)self;
}

static int PyKGameInstance_init(PyKGameInstance *self, PyObject *args, PyObject *kwds) {
    // No initialization needed since we're using a singleton
    return 0;
}

static PyObject *PyKGameInstance_calc_input(PyKGameInstance *self, PyObject *args) {
    bool accelerate, brake, item, drift;
    int rawStickX, rawStickY, trick;

    if (!PyArg_ParseTuple(args, "ppppiii", &accelerate, &brake, &item, &drift, &rawStickX,
                &rawStickY, &trick)) {
        return NULL;
    }

    self->instance->calcInput(accelerate, brake, item, drift, rawStickX, rawStickY, trick);
    Py_RETURN_NONE;
}

static PyObject *PyKGameInstance_calc(PyKGameInstance *self, PyObject *Py_UNUSED(ignored)) {
    self->instance->calc();
    Py_RETURN_NONE;
}

static PyObject *PyKGameInstance_get_position(PyKGameInstance *self, PyObject *Py_UNUSED(ignored)) {
    const EGG::Vector3f &pos = self->instance->getPosition();
    return Py_BuildValue("(fff)", pos.x(), pos.y(), pos.z());
}

static PyObject *PyKGameInstance_get_rotation(PyKGameInstance *self, PyObject *Py_UNUSED(ignored)) {
    const EGG::Quatf &rot = self->instance->getRotation();
    return Py_BuildValue("(ffff)", rot.x(), rot.y(), rot.z(), rot.w());
}

static PyObject *PyKGameInstance_get_internal_velocity(PyKGameInstance *self,
        PyObject *Py_UNUSED(ignored)) {
    const EGG::Vector3f &vel = self->instance->getInternalVelocity();
    return Py_BuildValue("(fff)", vel.x(), vel.y(), vel.z());
}

static PyObject *PyKGameInstance_get_external_velocity(PyKGameInstance *self,
        PyObject *Py_UNUSED(ignored)) {
    const EGG::Vector3f &vel = self->instance->getExternalVelocity();
    return Py_BuildValue("(fff)", vel.x(), vel.y(), vel.z());
}

static PyMethodDef PyKGameInstance_methods[] = {
        {"calc_input", (PyCFunction)PyKGameInstance_calc_input, METH_VARARGS,
                "Set input state for the game instance"},
        {"calc", (PyCFunction)PyKGameInstance_calc, METH_NOARGS,
                "Perform game calculations for one frame"},
        {"get_position", (PyCFunction)PyKGameInstance_get_position, METH_NOARGS,
                "Get the current position of the player"},
        {"get_rotation", (PyCFunction)PyKGameInstance_get_rotation, METH_NOARGS,
                "Get the current rotation of the player"},
        {"get_internal_velocity", (PyCFunction)PyKGameInstance_get_internal_velocity, METH_NOARGS,
                "Get the internal velocity of the player"},
        {"get_external_velocity", (PyCFunction)PyKGameInstance_get_external_velocity, METH_NOARGS,
                "Get the external velocity of the player"},
        {NULL} /* Sentinel */
};

static PyTypeObject PyKGameInstanceType = {
        PyVarObject_HEAD_INIT(NULL, 0).tp_name = "kgame.KGameInstance",
        .tp_doc = "KGameInstance wrapper",
        .tp_basicsize = sizeof(PyKGameInstance),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = PyKGameInstance_new,
        .tp_init = (initproc)PyKGameInstance_init,
        .tp_dealloc = (destructor)PyKGameInstance_dealloc,
        .tp_methods = PyKGameInstance_methods,
};

static PyObject *flush_denormals_to_zero(PyObject *self, PyObject *Py_UNUSED(ignored)) {
#if defined(__arm64__) || defined(__aarch64__)
    uint64_t fpcr;
    asm("mrs %0,   fpcr" : "=r"(fpcr));
    asm("msr fpcr, %0" ::"r"(fpcr | (1 << 24)));
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif
    Py_RETURN_NONE;
}

static PyMethodDef module_methods[] = {
        {"flush_denormals_to_zero", (PyCFunction)flush_denormals_to_zero, METH_NOARGS,
                "Enable flush-to-zero mode for denormal floating point numbers"},
        {NULL} /* Sentinel */
};

static PyModuleDef kgamemodule = {
        PyModuleDef_HEAD_INIT,
        .m_name = "kgame",
        .m_doc = "Python bindings for KGameInstance",
        .m_size = -1,
        .m_methods = module_methods,
};

PyMODINIT_FUNC PyInit_kgame(void) {
    PyObject *m;

    if (PyType_Ready(&PyKGameInstanceType) < 0) {
        return NULL;
    }

    m = PyModule_Create(&kgamemodule);
    if (m == NULL) {
        return NULL;
    }

    Py_INCREF(&PyKGameInstanceType);
    if (PyModule_AddObject(m, "KGameInstance", (PyObject *)&PyKGameInstanceType) < 0) {
        Py_DECREF(&PyKGameInstanceType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
