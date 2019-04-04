#include <py/algorithm/stretch.hpp>
#include <py/core/image2d.hpp>


#include <gtest/gtest.h>


TEST(Py, stretch)
{
  using namespace mln::py;

  int8_t* buffer = new int8_t[9];
  for (int i = 0; i < 9; ++i)
  {
    buffer[i] = i;
  }
  auto img       = image2d<>::from_buffer(reinterpret_cast<std::byte*>(buffer), 3, 3, Info::INT8_V);
  auto stretched = stretch(img);

  // FIXME:
  // for (auto i : stretched.values())
  // {
  //   std::cout << i << "\n";
  // }
}

TEST(Py, stretch_other)
{
  // FIXME should properly handle error
  //    auto ntima = image2d<>(3, 3, Info::OTHER);
  //    auto ntima2 = stretch(ntima);
  //    for (auto i : ntima2.values())
  //    {
  //        std::cout << i << "\n";
  //    }
}
