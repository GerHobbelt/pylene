#include <pln/core/image_cast.hpp>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/range/foreach.hpp>

#include <fixtures/ImagePath/image_path.hpp>

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

  /// \brief Check a specific array for the conversion NumPy (fortran order) -> ndbuffer_image
  /// \param[in] img An image created by numpy with specific values and parameters
  /// \return A boolean indicating if the convertion worked
  auto check_from_numpy_f(const mln::ndbuffer_image& img)
  {
    bool res     = true;
    res          = res && img.sample_type() == mln::sample_type_id::INT32;
    res          = res && img.pdim() == 2;
    res          = res && img.size(0) == 3;
    res          = res && img.size(1) == 4;
    res          = res && img.byte_stride(0) == 16;
    res          = res && img.byte_stride(1) == 4;
    //std::int32_t i = 0;
    auto casted = img.__cast<std::int32_t, 2>();
    mln_foreach (auto p, casted.domain())
      //res = res && casted(p) == (i++);
      pybind11::print(casted(p));

    return res;
  }

  PYBIND11_MODULE(pylena_test, m)
  {
    pybind11::module_::import("pylena");

    // Utility functions
    m.def("id", [](mln::ndbuffer_image img) { return img; });
    m.def("get_image_path", [](const std::string& name) { return fixtures::ImagePath::concat_with_filename(name); });

    // Checking functions
    m.def("check_from_numpy", &check_from_numpy);
    m.def("check_from_numpy_f", &check_from_numpy_f);
  }
} // namespace pln::test