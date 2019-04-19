#include <py/core/any_span.hpp>

#include <iostream>

#include <gtest/gtest.h>


TEST(Py, any_span_ctor)
{
  using namespace mln::py;

  auto buffer = new int8_t[9];
  for (int i = 0; i < 9; ++i)
  {
    buffer[i] = i;
  }
  auto span = any_span(reinterpret_cast<std::byte*>(buffer), sizeof(int8_t), Info::INT8_V);
  int i = 0;
  for(auto v : span)
  {
    ASSERT_EQ(i++, *reinterpret_cast<int8_t*>(v));
  }
  delete[] buffer;
}

TEST(Py, any_span_ctor_bigger_type)
{
  using namespace mln::py;

  auto buffer = new int64_t[9];
  for (int i = 0; i < 9; ++i)
  {
    buffer[i] = i;
  }
  auto span = any_span(reinterpret_cast<std::byte*>(buffer), sizeof(int64_t), Info::INT64_V);
  int i = 0;
  for(auto v : span)
  {
    ASSERT_EQ(i++, *reinterpret_cast<int64_t*>(v));
  }
  delete[] buffer;
}