#include <py/core/value_set.hpp>

#include <iostream>
#include <limits>

#include <gtest/gtest.h>


TEST(Py, vs_max) //Doesn't even need to be tested, but atomic testing.
{
  mln::py::value_set<int8_t> vs;
  ASSERT_EQ(std::any_cast<int8_t>(vs.max()), std::numeric_limits<int8_t>::max());
}

TEST(Py, vs_normalize) //delete after successful implem of divide()?
{
  mln::py::value_set<int64_t> vs;
  int64_t var = 1234;
  ASSERT_EQ(vs.normalize(var), static_cast<float>(var) / std::numeric_limits<int64_t>::max());
}

TEST(Py, vs_divide_T)
{
  mln::py::value_set<int64_t> vs;
  int64_t var = 144;
  std::any res = vs.divide(var, int64_t(12));
  ASSERT_EQ(static_cast<int>(std::any_cast<float>(res)), 12);
}

TEST(Py, vs_divide_void)
{
  mln::py::value_set<int64_t> vs;
  int64_t var = 144;
  void *ptr = &var;
  std::any res = vs.divide(ptr, int64_t(12));
  ASSERT_EQ(static_cast<int>(std::any_cast<float>(res)), 12);
}

//TODO write more tests! Fix stretch :x