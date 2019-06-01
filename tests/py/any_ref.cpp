#include <py/core/any_ref.hpp>

#include <any>

#include <gtest/gtest.h>


TEST(Py, any_ref_ctor_std_any)
{
  using namespace mln::py;
  std::any a = std::any(true);
  any_ref obj = any_ref(a);
  ASSERT_EQ(obj.as<bool>(), true);
}

TEST(Py, any_ref_ctor_void_ptr)
{
  using namespace mln::py;
  int a = 7;
  any_ref obj = any_ref(a);
  ASSERT_EQ(obj.as<int>(), a);
}