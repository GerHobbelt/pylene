#include <py/core/any_ref.hpp>

#include <any>
#include <iostream>

#include <gtest/gtest.h>


TEST(Py, any_ref_ctor_std_any)
{
  using namespace mln::py;
  std::any a = true;
  any_ref obj = any_ref(a);
  ASSERT_EQ(std::any_cast<bool>(*static_cast<std::any*>(obj.data())), true);
}

TEST(Py, any_ref_ctor_T)
{
  using namespace mln::py;
  int a = 7;
  any_ref obj = any_ref(a);
  ASSERT_EQ(*static_cast<int*>(obj.data()), a);
}

TEST(Py, any_ref_as_any)
{
  using namespace mln::py;
  std::any a = true;
  any_ref obj = any_ref(a);
  ASSERT_EQ(obj.as<bool>(), true);
}

TEST(Py, any_ref_as_T)
{
  using namespace mln::py;
  int a = 7;
  any_ref obj = any_ref(a);
  ASSERT_EQ(obj.as<int>(), a);
}

TEST(Py, any_ref_invalid_as)
{
  using namespace mln::py;
  int a = 7;
  any_ref obj = any_ref(a);
  try
  {
    bool __attribute__((unused)) b = obj.as<bool>();
    ASSERT_TRUE(false);
  }
  catch (...)
  {
    ASSERT_TRUE(true);
  }
}