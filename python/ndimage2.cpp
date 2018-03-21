#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "ndimage.hpp"
#include <numpy/ndarraytypes.h>

namespace py = pybind11;

namespace
{
  typedef struct {
    int two;              /* contains the integer 2 -- simple sanity check */
    int nd;               /* number of dimensions */
    char typekind;        /* kind in array --- character code of typestr */
    int itemsize;         /* size of each element */
    int flags;            /* flags indicating how the data should be interpreted */
                        /*   must set ARR_HAS_DESCR bit to validate descr */
    Py_intptr_t *shape;   /* A length-nd array of shape information */
    Py_intptr_t *strides; /* A length-nd array of stride information */
    void *data;           /* A pointer to the first element of the array */
    PyObject *descr;      /* NULL or data-description (same as descr key
                             of __array_interface__) -- must set ARR_HAS_DESCR
                             flag or this will be ignored. */
  } PyArrayInterface;

  char sampletypeid_to_size(mln::sample_type_id_t sample_type)
  {
    switch (sample_type)
    {
    case mln::sample_type_id_t::UINT8:
    case mln::sample_type_id_t::INT8: return 1;
    case mln::sample_type_id_t::UINT16:
    case mln::sample_type_id_t::INT16: return 2;
    case mln::sample_type_id_t::UINT32:
    case mln::sample_type_id_t::INT32: return 4;
    case mln::sample_type_id_t::UINT64:
    case mln::sample_type_id_t::INT64: return 8;
    case mln::sample_type_id_t::FLOAT: return 4;
    case mln::sample_type_id_t::DOUBLE: return 8;
    default: return 0;
    }
  }

  char sampletypeid_to_typekind(mln::sample_type_id_t sample_type)
  {
    switch (sample_type)
    {
    case mln::sample_type_id_t::UINT8:
    case mln::sample_type_id_t::UINT16:
    case mln::sample_type_id_t::UINT32:
    case mln::sample_type_id_t::UINT64: return 'u';
    case mln::sample_type_id_t::INT8:
    case mln::sample_type_id_t::INT16:
    case mln::sample_type_id_t::INT32:
    case mln::sample_type_id_t::INT64: return 'i';
    case mln::sample_type_id_t::FLOAT:
    case mln::sample_type_id_t::DOUBLE: return 'f';

    default: return 0;
    }
  }
}

// void bind_ndimage(py::module& m)
// {
//   py::class_<mln::NDImageInterface>(m, "ndimage", py::buffer_protocol())
//     .def_buffer(&ndimage_to_python)
//     .def_property_readonly("__array_struct__", &ndarray_from_ndimage)
//     ;
// }


namespace pybind11 {
  namespace detail {


    template <>
    handle type_caster<mln::NDImageInterface>::cast(const mln::NDImageInterface& src,
                                                    return_value_policy /* policy */,
                                                    handle /* parent */)
    {
      int pdim = f.get_pdim();
      int vdim = f.get_vdim();
      mln::sample_type_id_t stype = f.sample_type_id();
      int ndim = pdim + (vdim > 1);

      std::vector<npy_intp> shape(ndim);
      std::vector<npy_intp> strides(ndim);
      for (int k = 0; k < pdim; ++k) {
        shape[k] = f.dim(k);
        strides[k] = f.byte_stride(k);
      }
      if (vdim > 1) {
        shape[pdim] = vdim;
        strides[pdim] = sampletypeid_to_size(stype);
      }

      int type_num = sampletypeid_to_typekind(stype);



      PyObject* arr = PyArray_New(&PyArray_Type,
                                  ndim,
                                  shape.data(),
                                  ampletypeid_to_typekind(stype),
                                  strides.data(),
                                  src.buffer(),
                                  sampletypeid_to_size(stype),
                                  NPY_ARRAY_WRITEABLE | NPY_ARRAY_ALIGNED | NPY_ARRAY_C_CONTIGUOUS,
                                  NULL);
      return handle(arr);
    }

  }
}

