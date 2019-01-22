#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/image_ops.hpp>
#include <mln/core/image/private/image_operators.hpp>

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

  rgb zero  = {0, 0, 0};
  rgb douze = {12, 0, 0};
  mln::fill(ima, zero);

  auto x = make_unary_image_expr(ima, red());
  fill(x, 12);

  ASSERT_TRUE(mln::all(ima == douze));
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

  ASSERT_TRUE(mln::all(-ima == ref));
}

TEST(Core, Image2d_MixedOperator)
{
  using namespace mln;

  image2d<char>  x(5, 5);
  image2d<short> y(5, 5);

  iota(x, 0);
  iota(y, 0);

  static_assert(std::is_same_v<typename decltype(x + x)::value_type, char>);

  ASSERT_TRUE(mln::all((x + y) == (2 * y)));
}

TEST(Core, Image2d_WhereOperator)
{
  using namespace mln;

  image2d<uint8_t> x(5, 5);
  image2d<uint8_t> y(5, 5);
  iota(x, 0);
  iota(y, 0);

  auto f1 = where(x > 12, x, uint8_t(12));         // RValue image + LValue image + scalar
  auto f2 = where(x > 12, x, y);                   // RValue image + LValue image + LValue image
  auto f3 = where(x > 12, uint8_t(12), x);         // RValue image + Scalar + LValue image
  auto f4 = where(x > 12, uint8_t(0), uint8_t(1)); // RValue image + Scalar + Scalar

  ASSERT_TRUE(mln::all(f1 >= 12));
  static_assert(std::is_same_v<mln_reference(decltype(f1)), const uint8_t&>);
  static_assert(std::is_same_v<mln_reference(decltype(f2)), uint8_t&>);
}


TEST(Core, UnaryOperator)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  image2d<int> ima = {{1, 2, 3}, {4, 5, 6}};
  image2d<int> ref = {{-1, -2, -3}, {-4, -5, -6}};

  auto g = -ima;
  ASSERT_TRUE(mln::experimental::all(g == ref));
}

TEST(Core, BinaryOperator_SameTypes)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  image2d<uint8_t> ref = {{2, 4, 6}, {8, 10, 12}};

  auto g1 = ima + ima;
  auto g2 = uint8_t(2) * ima;
  auto g3 = ima * uint8_t(2);

  static_assert(std::is_same_v<decltype(g1)::value_type, uint8_t>);
  static_assert(std::is_same_v<decltype(g2)::value_type, uint8_t>);
  static_assert(std::is_same_v<decltype(g3)::value_type, uint8_t>);

  ASSERT_TRUE(mln::experimental::all(g1 == ref));
  ASSERT_TRUE(mln::experimental::all(g2 == ref));
  ASSERT_TRUE(mln::experimental::all(g3 == ref));
}

TEST(Core, BinaryOperators_MixedTypes)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  image2d<uint8_t>  ima1 = {{1, 2, 3}, {4, 5, 6}};
  image2d<uint16_t> ima2 = {{1, 2, 3}, {4, 5, 6}};
  image2d<uint16_t> ref  = {{2, 4, 6}, {8, 10, 12}};

  auto g1 = ima1 + ima2;
  auto g2 = uint16_t(2) * ima1;
  auto g3 = ima1 * uint16_t(2);

  using RType = std::common_type_t<uint8_t, uint16_t>;
  static_assert(std::is_same<RType, int>());

  static_assert(std::is_same_v<decltype(g1)::value_type, RType>);
  static_assert(std::is_same_v<decltype(g2)::value_type, RType>);
  static_assert(std::is_same_v<decltype(g3)::value_type, RType>);

  ASSERT_TRUE(mln::experimental::all(g1 == ref));
  ASSERT_TRUE(mln::experimental::all(g2 == ref));
  ASSERT_TRUE(mln::experimental::all(g3 == ref));
}

TEST(Core, Where)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  image2d<uint8_t> x = {{1, 2, 3}, //
                        {4, 5, 6}};
  image2d<uint8_t> y = {{4, 5, 6}, //
                        {1, 2, 3}};

  image2d<uint8_t> ref_f1 = {{12, 12, 12}, //
                             {4, 5, 6}};
  image2d<uint8_t> ref_f2 = {{4, 5, 6}, //
                             {4, 5, 6}};
  image2d<uint8_t> ref_f3 = {{1, 2, 3}, //
                             {12, 12, 12}};
  image2d<uint8_t> ref_f4 = {{1, 1, 1}, //
                             {0, 0, 0}};

  auto f1 = mln::experimental::where(x > 3, x, uint8_t(12));         // RValue image + LValue image + scalar
  auto f2 = mln::experimental::where(x > 3, x, y);                   // RValue image + LValue image + LValue image
  auto f3 = mln::experimental::where(x > 3, uint8_t(12), x);         // RValue image + Scalar + LValue image
  auto f4 = mln::experimental::where(x > 3, uint8_t(0), uint8_t(1)); // RValue image + Scalar + Scalar

  static_assert(std::is_same_v<image_reference_t<decltype(f1)>, uint8_t>);
  static_assert(std::is_same_v<image_reference_t<decltype(f2)>, uint8_t&>);
  static_assert(std::is_same_v<image_reference_t<decltype(f3)>, uint8_t>);
  static_assert(std::is_same_v<image_reference_t<decltype(f4)>, uint8_t>);

  // FIXME: issue https://github.com/ericniebler/range-v3/issues/996 with gcc8.2
  // ASSERT_TRUE(mln::experimental::all(f1 == ref_f1));
  // ASSERT_TRUE(mln::experimental::all(f2 == ref_f2));
  // ASSERT_TRUE(mln::experimental::all(f3 == ref_f3));
  // ASSERT_TRUE(mln::experimental::all(f4 == ref_f4));


  image2d<uint8_t> ref_x = {{1, 2, 3}, //
                            {42, 42, 42}};
  image2d<uint8_t> ref_y = {{42, 42, 42}, //
                            {1, 2, 3}};

  // FIXME: issue https://github.com/ericniebler/range-v3/issues/996 with gcc8.2
  // mln::experimental::fill(f2, 42);

  // ASSERT_TRUE(mln::experimental::all(x == ref_x));
  // ASSERT_TRUE(mln::experimental::all(y == ref_y));
}
