#include <pybind11/pybind11.h>

#include <mln/core/image/private/ndimage_interface.hpp>
#include <mln/core/image/image2d.hpp>

namespace pybind11 { namespace detail {
    template <> struct type_caster<mln::NDImageInterface> {
    public:
        /**
         * This macro establishes the name 'inty' in
         * function signatures and declares a local variable
         * 'value' of type inty
         */
      PYBIND11_TYPE_CASTER(mln::image2d<int>, _("ndimage"));

        /**
         * Conversion part 1 (Python->C++): convert a PyObject into a inty
         * instance or return false upon failure. The second argument
         * indicates whether implicit conversions should be applied.
         */
      bool load(handle /* src */, bool) {
            // /* Extract PyObject from handle */
            // PyObject *source = src.ptr();
            // /* Try converting into a Python integer value */
            // PyObject *tmp = PyNumber_Long(source);
            // if (!tmp)
            //     return false;
            // /* Now try to convert into a C++ int */
            // value.long_value = PyLong_AsLong(tmp);
            // Py_DECREF(tmp);
            // /* Ensure return code was OK (to avoid out-of-range errors etc) */
            // return !(value.long_value == -1 && !PyErr_Occurred());
          return false;
        }

        /**
         * Conversion part 2 (C++ -> Python): convert an inty instance into
         * a Python object. The second and third arguments are used to
         * indicate the return value policy and parent object (for
         * ``return_value_policy::reference_internal``) and are generally
         * ignored by implicit casters.
         */
      static handle cast(const mln::NDImageInterface& src,
                         return_value_policy /* policy */,
                         handle /* parent */);
    };
}} // namespace pybind11::detail

handle wrap_image(const mln::NDImageInterface& src);
