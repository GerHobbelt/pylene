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
    bool res            = true;
    res                 = res && img.sample_type() == mln::sample_type_id::INT32;
    res                 = res && img.pdim() == 2;
    res                 = res && img.size(0) == 3;
    res                 = res && img.size(1) == 4;
    res                 = res && img.byte_stride(0) == 4;
    res                 = res && img.byte_stride(1) == 12;
    std::int32_t i      = 0;
    auto         casted = img.__cast<std::int32_t, 2>();
    mln_foreach (auto p, casted.domain())
      res = res && casted(p) == (i++);

    return res;
  }

  /// \brief Create a ndbuffer_image of type uint16 filled with the iota function
  /// \return The created ndbuffer_image
  auto iota(void)
  {
    constexpr int WIDTH  = 15;
    constexpr int HEIGHT = 10;
    auto          res    = mln::ndbuffer_image(mln::sample_type_id::UINT16, WIDTH, HEIGHT, {0, 0});
    std::uint16_t i      = 0;
    for (auto y = 0; y < HEIGHT; y++)
    {
      std::uint16_t* lineptr = reinterpret_cast<std::uint16_t*>(res.buffer()) + y * res.stride(1);
      for (auto x = 0; x < WIDTH; x++)
        lineptr[x] = i++;
    }
    return res;
  }

  PYBIND11_MODULE(pylena_test, m)
  {
    init_pylena_numpy(m);

    // Utility functions
    m.def("id", [](mln::ndbuffer_image img) { return img; });
    m.def("iota", &iota);

    // Checking functions
    m.def("check_from_numpy", &check_from_numpy);
  }
} // namespace pln::test