#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <mln/core/image/private/ndimage_interface.hpp>

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

  std::string sampletypeid_to_numpystr(mln::sample_type_id_t sample_type)
  {
    switch (sample_type)
    {
    case mln::sample_type_id_t::UINT8: return py::format_descriptor<mln::uint8>::format();
    case mln::sample_type_id_t::INT8: return py::format_descriptor<mln::int8>::format();
    case mln::sample_type_id_t::UINT16: return py::format_descriptor<mln::uint16>::format();
    case mln::sample_type_id_t::INT16: return py::format_descriptor<mln::int16>::format();
    case mln::sample_type_id_t::UINT32: return py::format_descriptor<mln::uint32>::format();
    case mln::sample_type_id_t::INT32: return py::format_descriptor<mln::int32>::format();
    case mln::sample_type_id_t::UINT64: return py::format_descriptor<mln::uint64>::format();
    case mln::sample_type_id_t::INT64: return py::format_descriptor<mln::int64>::format();
    case mln::sample_type_id_t::FLOAT: return py::format_descriptor<float>::format();
    case mln::sample_type_id_t::DOUBLE: return py::format_descriptor<double>::format();
    default: return "";
    }
  }

  void free_array_interface(void* obj)
  {
    auto o = reinterpret_cast<PyArrayInterface*>(obj);
    delete [] o->shape;
    delete [] o->strides;
    delete o;
  }

  py::capsule ndarray_from_ndimage(mln::NDImageInterface& f)
  {
    auto g = new PyArrayInterface;
    auto obj = py::capsule(g, free_array_interface);

    int pdim = f.get_pdim();
    int vdim = f.get_vdim();
    mln::sample_type_id_t stype = f.sample_type_id();
    int ndim = pdim + (vdim > 1);
    g->two = 2;
    g->nd = ndim;
    g->typekind = sampletypeid_to_typekind(stype);
    g->itemsize = sampletypeid_to_size(stype);

    const int NOTSWAPPED = 0x200;
    const int WRITEABLE = 0x400;
    g->flags = NOTSWAPPED | WRITEABLE;
    g->shape = new Py_intptr_t[ndim];
    g->strides = new Py_intptr_t[ndim];
    for (int k = 0; k < pdim; ++k) {
      g->shape[k] = f.dim(k);
      g->strides[k] = f.byte_stride(k);
    }
    if (vdim > 1) {
      g->shape[pdim] = vdim;
      g->strides[pdim] = sampletypeid_to_size(stype);
    }
    g->data = f.buffer();
    g->descr = NULL;
    return obj;
  }

  py::buffer_info ndimage_to_python(mln::NDImageInterface& f)
  {
    int pdim = f.get_pdim();
    int vdim = f.get_vdim();
    mln::sample_type_id_t stype = f.sample_type_id();

    int ndim = pdim + (vdim > 1);
    std::vector<ssize_t> shape(ndim);
    std::vector<ssize_t> strides(ndim);
    for (int k = 0; k < pdim; ++k)
    {
      shape[k] = f.dim(k);
      strides[k] = f.byte_stride(k);
    }

    if (vdim > 1)
    {
      shape[pdim] = vdim;
      strides[pdim] = sampletypeid_to_size(stype);
    }

    return py::buffer_info(f.buffer(),                      /* Pointer to buffer */
                           sampletypeid_to_size(stype),     /* Size of one scalar */
                           sampletypeid_to_numpystr(stype), /* Python struct-style format descriptor */
                           ndim,                            /* Number of dimensions */
                           std::move(shape),                /* Buffer dimensions */
                           std::move(strides));              /* Strides (in bytes) for each index */
  }
}

void bind_ndimage(py::module& m)
{
  py::class_<mln::NDImageInterface>(m, "ndimage", py::buffer_protocol())
    .def_buffer(&ndimage_to_python)
    .def_property_readonly("__array_struct__", &ndarray_from_ndimage)
    ;
}
