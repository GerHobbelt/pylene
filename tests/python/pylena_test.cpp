#include <pln/core/image_cast.hpp>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/range/foreach.hpp>

#include <pybind11/pybind11.h>

#include <tuple>

namespace pln::test
{
  /// \brief Check a specific array for the conversion NumPy -> ndbuffer_image
  /// \param[in] img An image created by numpy with specific values and parameters
  /// \return A boolean indicating if the convertion worked
  auto check_from_numpy(const mln::ndbuffer_image& img)
  {
    bool res     = true;
    res          = res && img.sample_type() == mln::sample_type_id::INT32;
    res          = res && img.pdim() == 2;
    res          = res && img.size(0) == 3;
    res          = res && img.size(1) == 4;
    res          = res && img.byte_stride(0) == 4;
    res          = res && img.byte_stride(1) == 12;
    std::int32_t i = 0;
    auto casted = img.__cast<std::int32_t, 2>();
    mln_foreach (auto p, casted.domain())
      res = res && casted(p) == (i++);

    return res;
  }

  PYBIND11_MODULE(pylena_test, m)
  {
    pybind11::module_::import("pylena");

    // Utility functions
    m.def("id", [](mln::ndbuffer_image img) { return img; });

    // Checking functions
    m.def("check_from_numpy", &check_from_numpy);
  }
} // namespace pln::test