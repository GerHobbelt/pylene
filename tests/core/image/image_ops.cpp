#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/image_ops.hpp>
#include <mln/core/image/private/image_operators.hpp>
#include <mln/core/rangev3/foreach.hpp>

#include <mln/io/imprint.hpp>

#include <boost/tuple/tuple_io.hpp>

#include <gtest/gtest.h>

mln::image2d<int> make_image()
{
  mln::image2d<int> x(5, 5);
  mln::iota(x, 0);
  return x;
}

struct rgb
{
  int r, g, b;

  bool operator==(const rgb& other) const { return r == other.r and g == other.g and b == other.b; }
};

struct red
{

  int& operator()(rgb& x) const { return x.r; }

  const int& operator()(const rgb& x) const { return x.r; }
};

std::ostream& operator<<(std::ostream& ss, const rgb& x)
{
  return ss << boost::make_tuple(x.r, x.g, x.b);
}

TEST(Core, Image2d_LValueOperator)
{
  using namespace mln;
  image2d<rgb> ima(5, 5);

  rgb zero = {0, 0, 0};
  rgb douze = {12, 0, 0};
  fill(ima, zero);

  auto x = make_unary_image_expr(ima, red());
  fill(x, 12);

  ASSERT_TRUE(all(ima == douze));
}

TEST(Core, Image2d_Operators)
{
  using namespace mln;

  image2d<int> ima(5, 5);
  image2d<int> ref(5, 5);

  iota(ima, 0);
  int i = 0;

  mln_viter(v, ref);
  mln_forall (v)
    *v = i--;

  ASSERT_TRUE(all(-ima == ref));
}

TEST(Core, Image2d_Operators_New)
{
  using namespace mln;

  image2d<int> ima(5, 5);
  image2d<int> ref(5, 5);

  iota(ima, 0);
  int i = 0;
  mln_foreach_new(auto& v, ref.new_values())
    v = i--;

  ASSERT_TRUE(new_all(new_eq(new_unary_minus(ima), ref)));
}



TEST(Core, Image2d_MixedOperator)
{
  using namespace mln;

  image2d<char> x(5, 5);
  image2d<short> y(5, 5);

  iota(x, 0);
  iota(y, 0);

  ASSERT_TRUE((std::is_same<typename decltype(x + x)::value_type, char>()));
  ASSERT_TRUE((std::is_same<typename decltype(x + y)::value_type, typename std::common_type<char, short>::type>()));

  ASSERT_TRUE(all((x + y) == (2 * y)));
}

TEST(Core, Image2d_WhereOperator)
{
  using namespace mln;

  image2d<uint8> x(5, 5);
  image2d<uint8> y(5, 5);
  iota(x, 0);
  iota(y, 0);

  auto f1 = where(x > 12, x, (uint8)12);       // RValue image + LValue image + scalar
  auto f2 = where(x > 12, x, y);               // RValue image + LValue image + LValue image
  auto f3 = where(x > 12, (uint8)12, x);       // RValue image + Scalar + LValue image
  auto f4 = where(x > 12, (uint8)0, (uint8)1); // RValue image + Scalar + Scalar

  ASSERT_TRUE(all(f1 >= 12));
  ASSERT_TRUE((std::is_same<mln_reference(decltype(f1)), const uint8&>()));
  ASSERT_TRUE((std::is_same<mln_reference(decltype(f2)), uint8&>()));
}
