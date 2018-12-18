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

TEST(Core, Image2d_MixedOperator)
{
  using namespace mln;

  image2d<char>  x(5, 5);
  image2d<short> y(5, 5);

  iota(x, 0);
  iota(y, 0);

  ASSERT_TRUE((std::is_same<typename decltype(x + x)::value_type, char>()));
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


TEST(Core, UnaryOperator)
{
  using namespace mln;

  image2d<int> ima = {{1, 2, 3}, {4, 5, 6}};
  image2d<int> ref = {{-1, -2, -3}, {-4, -5, -6}};

  // FIXME?: ?segfault?

  auto g = new_unary_minus(ima);
  ASSERT_TRUE(new_all(new_eq(g, ref)));
}

TEST(Core, BinaryOperator_SameTypes)
{
  using namespace mln;

  image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  image2d<uint8_t> ref = {{2, 4, 6}, {8, 10, 12}};

  auto g1 = new_plus(ima, ima);
  auto g2 = new_multiplies(uint8_t(2), ima);
  auto g3 = new_multiplies(ima, uint8_t(2));

  ASSERT_TRUE((std::is_same<decltype(g1)::value_type, uint8_t>::value));
  ASSERT_TRUE((std::is_same<decltype(g2)::value_type, uint8_t>::value));
  ASSERT_TRUE((std::is_same<decltype(g3)::value_type, uint8_t>::value));

  ASSERT_TRUE(new_all(new_eq(g1, ref)));
  ASSERT_TRUE(new_all(new_eq(g2, ref)));
  ASSERT_TRUE(new_all(new_eq(g3, ref)));
}

TEST(Core, BinaryOperators_MixedTypes)
{
  using namespace mln;

  image2d<uint8_t>  ima1 = {{1, 2, 3}, {4, 5, 6}};
  image2d<uint16_t> ima2 = {{1, 2, 3}, {4, 5, 6}};
  image2d<uint16_t> ref  = {{2, 4, 6}, {8, 10, 12}};

  auto g1 = new_plus(ima1, ima2);
  auto g2 = new_multiplies(uint16_t(2), ima1);
  auto g3 = new_multiplies(ima1, uint16_t(2));

  using RType = std::common_type_t<uint8_t, uint16_t>;
  static_assert(std::is_same<RType, int>());

  ASSERT_TRUE((std::is_same<decltype(g1)::value_type, RType>::value));
  ASSERT_TRUE((std::is_same<decltype(g2)::value_type, RType>::value));
  ASSERT_TRUE((std::is_same<decltype(g3)::value_type, RType>::value));

  ASSERT_TRUE(new_all(new_eq(g1, ref)));
  ASSERT_TRUE(new_all(new_eq(g2, ref)));
  ASSERT_TRUE(new_all(new_eq(g3, ref)));
}


TEST(Core, Where)
{
  using namespace mln;


  image2d<uint8_t> x = {{1, 2, 3}, {4, 5, 6}};
  image2d<uint8_t> y = {{4, 5, 6}, {1, 2, 3}};

  auto                  f1 = where(x > 3, x, uint8_t(12));       // RValue image + LValue image + scalar
  [[maybe_unused]] auto f2 = where(x > 3, x, y);                 // RValue image + LValue image + LValue image
  auto                  f3 = where(x > 3, uint8_t(12), x);       // RValue image + Scalar + LValue image
  auto                  f4 = where(x > 3, uint8_t(0), uint8(1)); // RValue image + Scalar + Scalar


  // FIXME: Use concept checking

  static_assert(std::is_same<image_reference_t<decltype(f4)>, uint8_t&>());
  ASSERT_TRUE((std::is_same<image_reference_t<decltype(f1)>, const uint8_t&>()));
  ASSERT_TRUE((std::is_same<image_reference_t<decltype(f3)>, const uint8_t&>()));
  ASSERT_TRUE((std::is_same<image_reference_t<decltype(f4)>, uint8_t&>()));
}
