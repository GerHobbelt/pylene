#include <py/core/image2d.hpp>

#include <gtest/gtest.h>


TEST(Py, from_buffer)
{
  using namespace mln::py;

  int8_t* buffer = new int8_t[9];
  for (int i = 0; i < 9; ++i)
  {
    buffer[i] = i;
  }
  auto img = image2d<>::from_buffer(reinterpret_cast<std::byte*>(buffer), 3, 3, Info::INT8_V);
  for (int i = 0; i < 9; ++i)
  {
    ASSERT_EQ(buffer[i], static_cast<int>(reinterpret_cast<int8_t*>(img.buffer())[i]));
  }
}
