#define PY_ARRAY_UNIQUE_SYMBOL tosgui_ARRAY_API
#define NO_IMPORT_ARRAY

#include "tosgui.hpp"

#include <numpy/ndarrayobject.h>

#include <vector>


namespace mln
{

  PyObject* filtertree(PyObject* image_, PyObject* parent_, PyObject* nodemap_, PyObject* nodelist_)
  {
    PyArrayObject* image    = (PyArrayObject*)image_;
    PyArrayObject* nodemap  = (PyArrayObject*)nodemap_;
    PyArrayObject* nodelist = (PyArrayObject*)PyArray_FROM_OTF(nodelist_, NPY_UINT, NPY_IN_ARRAY);
    PyArrayObject* out      = (PyArrayObject*)PyArray_NewLikeArray(image, NPY_CORDER, NULL, 0);

    if (nodelist == NULL)
    {
      Py_INCREF(Py_None);
      return Py_None;
    }

    int               nnodes = PyArray_Size(parent_);
    std::vector<bool> active(nnodes, false);
    {
      npy_uint* data = (npy_uint*)PyArray_DATA(nodelist);
      int       sz   = PyArray_Size((PyObject*)nodelist);
      for (int i = 0; i < sz; ++i)
        active[data[i]] = true;
    }

    // Propagate to subtress
    {
      npy_uint* parent = (npy_uint*)PyArray_DATA(parent_);
      for (int i = 0; i < nnodes; ++i)
        active[i] = active[i] or active[parent[i]];
    }

    int nc, nr;
    nr = PyArray_DIMS(image)[0];
    nc = PyArray_DIMS(image)[1];

    PyArrayObject* arr1 = (PyArrayObject*)PyArray_FROM_OTF(image_, NPY_FLOAT, NPY_IN_ARRAY);
    if (arr1 == NULL)
    {
      return (PyObject*)out;
    }

    for (int i = 0; i < nr; ++i)
      for (int j = 0; j < nc; ++j)
      {
        npy_float* outptr = (npy_float*)PyArray_GETPTR2(out, i, j);
        npy_float* inptr  = (npy_float*)PyArray_GETPTR2(arr1, i, j);

        npy_uint node = *(npy_uint*)PyArray_GETPTR2(nodemap, i, j);
        if (not active[node])
          outptr[0] = inptr[0];
        else
          outptr[0] = 0.3 + inptr[0];

        outptr[1] = inptr[1];
        outptr[2] = inptr[2];
      }

    Py_DECREF(arr1);
    return (PyObject*)out;
  }


} // namespace mln
