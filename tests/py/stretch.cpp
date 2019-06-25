#include <py/algorithm/stretch.hpp>
#include <py/core/image2d.hpp>


#include <gtest/gtest.h>


TEST(Py, stretch_void)
{
  using namespace mln::py;

  int8_t* buffer = new int8_t[9];
  for (int i = 0; i < 9; ++i)
  {
    buffer[i] = i;
  }
  auto img       = image2d<>::from_buffer(reinterpret_cast<std::byte*>(buffer), 3, 3, Info::INT8_V);
  auto stretched = impl::stretch(img);

  int j = 0;
  for (auto i : stretched.values())
  {
      EXPECT_EQ(i, static_cast<float>(j++) / std::numeric_limits<int8_t>::max());
  }
  delete[] buffer;
}

TEST(Py, stretch_T)
{
  using namespace mln::py;

  int8_t* buffer = new int8_t[9];
  for (int i = 0; i < 9; ++i)
  {
    buffer[i] = i;
  }
  auto img       = image2d<int8_t>::from_buffer(buffer, 3, 3);
  auto stretched = impl::stretch(img);

  int j = 0;
  for (auto i : stretched.values())
  {
      EXPECT_EQ(i, static_cast<float>(j++) / std::numeric_limits<int8_t>::max());
  }
  delete[] buffer;
}

TEST(Py, stretch_invalid)
{
  // FIXME should properly handle error
  //    auto ntima = image2d<>(3, 3, Info::OTHER);
  //    auto ntima2 = stretch(ntima);
  //    for (auto i : ntima2.values())
  //    {
  //        std::cout << i << "\n";
  //    }
}
