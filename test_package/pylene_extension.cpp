#include <pln/core/image_cast.hpp>

mln::ndbuffer_image iota(void)
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

PYBIND11_MODULE(pylene_extension, m)
{
  pln::init_pylena_numpy(m);
  m.def("iota", &iota);
}