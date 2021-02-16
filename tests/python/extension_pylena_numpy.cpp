#include <pln/core/image_cast.hpp>

namespace pln::test
{
  PYBIND11_MODULE(pylena_extension, m)
  {
    init_pylena_numpy(m);
    m.def("id", [](const mln::ndbuffer_image& img) { return img; });
    m.def("generate_iota", []() {
      mln::ndbuffer_image img(mln::sample_type_id::UINT8, 10, 15, {0, 0});
      for (std::size_t y = 0; y < 15; y++)
      {
        std::uint8_t* lineptr = reinterpret_cast<std::uint8_t*>(img.buffer()) + y * 10;
        for (std::size_t x = 0; x < 10; x++)
        {
          lineptr[x] = 20 + y * 10 + x;
        }
      }
      return img;
    });
  }
} // namespace pln::test