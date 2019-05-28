#include <py/core/any_ref>
#include <any>

#include <gtest/gtest.h>


TEST(Py, any_ref_ctor)
{
  using namespace mln::py;
  std::any a = std::any(true);
  any_ref obj = any_ref(a);
  ASSERT_EQ(obj.data(), a);
}