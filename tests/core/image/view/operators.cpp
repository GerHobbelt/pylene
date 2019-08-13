#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/domain/where.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/image_ops.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/rangev3/foreach.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>


mln::experimental::image2d<int> make_image()
{
  mln::experimental::image2d<int> x(5, 5);
  mln::iota(x, 0);
  return x;
}

struct rgb
{
  int r, g, b;

  bool operator==(const rgb& other) const { return r == other.r and g == other.g and b == other.b; }
};

TEST(Core, Image2d_LValueOperator)
{
  using namespace mln::view::ops;

  mln::image_build_params params;
  params.init_value = rgb{0, 0, 0};

  auto ima = mln::experimental::image2d<rgb>(5, 5, params);
  auto x   = mln::view::transform(ima, &rgb::r);
  mln::fill(x, 12);

  ASSERT_TRUE(mln::all_of(ima == rgb{12,0,0}));
  ASSERT_TRUE(mln::all_of(x == 12));
}

TEST(Core, UnaryOperator)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<int> ima = {{1, 2, 3}, {4, 5, 6}};
  mln::experimental::image2d<int> ref = {{-1, -2, -3}, {-4, -5, -6}};

  auto g = -ima;
  ASSERT_IMAGES_EQ_EXP(g, ref);
}

TEST(Core, BinaryOperator_SameTypes)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  mln::experimental::image2d<uint8_t> ref = {{2, 4, 6}, {8, 10, 12}};

  auto g1 = ima + ima;
  auto g2 = uint8_t(2) * ima;
  auto g3 = ima * uint8_t(2);

  static_assert(std::is_same_v<decltype(g1)::value_type, uint8_t>);
  static_assert(std::is_same_v<decltype(g2)::value_type, uint8_t>);
  static_assert(std::is_same_v<decltype(g3)::value_type, uint8_t>);

  ASSERT_IMAGES_EQ_EXP(g1, ref);
  ASSERT_IMAGES_EQ_EXP(g2, ref);
  ASSERT_IMAGES_EQ_EXP(g3, ref);
}

TEST(Core, BinaryOperator_EqualFP)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<double> ima = {{1.015, 2.015, 3.015}, {4.015, 5.015, 6.015}};
  mln::experimental::image2d<double> ref = {{1.016, 2.016, 3.016}, {4.016, 5.016, 6.016}};

  ASSERT_TRUE(mln::all_of(equalFP(ima, ref, 10e-3)));
  ASSERT_TRUE(mln::all_of(not equalFP(ima, ref, 10e-5)));
}

TEST(Core, BinaryOperators_MixedTypes)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t>  ima1 = {{1, 2, 3}, {4, 5, 6}};
  mln::experimental::image2d<uint16_t> ima2 = {{1, 2, 3}, {4, 5, 6}};
  mln::experimental::image2d<uint16_t> ref  = {{2, 4, 6}, {8, 10, 12}};

  auto g1 = ima1 + ima2;
  auto g2 = uint16_t(2) * ima1;
  auto g3 = ima1 * uint16_t(2);

  using RType = std::common_type_t<uint8_t, uint16_t>;
  static_assert(std::is_same<RType, int>());

  static_assert(std::is_same_v<decltype(g1)::value_type, RType>);
  static_assert(std::is_same_v<decltype(g2)::value_type, RType>);
  static_assert(std::is_same_v<decltype(g3)::value_type, RType>);

  ASSERT_IMAGES_EQ_EXP(g1, ref);
  ASSERT_IMAGES_EQ_EXP(g2, ref);
  ASSERT_IMAGES_EQ_EXP(g3, ref);

}

TEST(Core, IfElse)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t> x = {{1, 2, 3}, //
                                           {4, 5, 6}};
  mln::experimental::image2d<uint8_t> y = {{4, 5, 6}, //
                                           {1, 2, 3}};

  mln::experimental::image2d<uint8_t> ref_f1 = {{12, 12, 12}, //
                                                {4, 5, 6}};
  mln::experimental::image2d<uint8_t> ref_f2 = {{4, 5, 6}, //
                                                {4, 5, 6}};
  mln::experimental::image2d<uint8_t> ref_f3 = {{1, 2, 3}, //
                                                {12, 12, 12}};
  mln::experimental::image2d<uint8_t> ref_f4 = {{1, 1, 1}, //
                                                {0, 0, 0}};

  auto f1 = mln::view::ifelse(x > 3, x, uint8_t(12));         // RValue image + LValue image + scalar
  auto f2 = mln::view::ifelse(x > 3, x, y);                   // RValue image + LValue image + LValue image
  auto f3 = mln::view::ifelse(x > 3, uint8_t(12), x);         // RValue image + Scalar + LValue image
  auto f4 = mln::view::ifelse(x > 3, uint8_t(0), uint8_t(1)); // RValue image + Scalar + Scalar

  static_assert(std::is_same_v<mln::image_reference_t<decltype(f1)>, uint8_t>);
  static_assert(std::is_same_v<mln::image_reference_t<decltype(f2)>, uint8_t&>);
  static_assert(std::is_same_v<mln::image_reference_t<decltype(f3)>, uint8_t>);
  static_assert(std::is_same_v<mln::image_reference_t<decltype(f4)>, uint8_t>);

  ASSERT_IMAGES_EQ_EXP(f1, ref_f1);
  ASSERT_IMAGES_EQ_EXP(f2, ref_f2);
  ASSERT_IMAGES_EQ_EXP(f3, ref_f3);
  ASSERT_IMAGES_EQ_EXP(f4, ref_f4);


  mln::experimental::image2d<uint8_t> ref_x = {{1, 2, 3}, //
                                               {42, 42, 42}};
  mln::experimental::image2d<uint8_t> ref_y = {{42, 42, 42}, //
                                               {1, 2, 3}};

  mln::fill(f2, 42);
  ASSERT_IMAGES_EQ_EXP(x, ref_x);
  ASSERT_IMAGES_EQ_EXP(y, ref_y);
}

TEST(Core, Where)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t> x = {{1, 2, 3}, {4, 5, 6}};

  auto y = x % 2;
  auto z = mln::experimental::where(y);

  static_assert(::ranges::ForwardRange<decltype(z)>());


  for (auto p : z)
    ASSERT_EQ(1, x(p) % 2);

  mln_foreach_new (auto p, x.domain())
    if (x(p) % 2 == 1)
    {
      ASSERT_TRUE(z.has(p));
    }
}


struct mask_archetype : mln::experimental::Image<mask_archetype>
{
  using value_type    = bool;
  using reference     = const bool&;
  using domain_type   = mln::archetypes::Domain;
  using point_type    = ::ranges::range_value_t<domain_type>;
  using category_type = mln::forward_image_tag;
  using concrete_type = mask_archetype;

  struct new_pixel_type
  {
    bool       val() const;
    point_type point() const;
  };

  template <class V>
  using ch_value_type = mask_archetype;

  // additional traits
  using extension_category = mln::extension::none_extension_tag;
  using indexable          = std::false_type;
  using accessible         = std::true_type;
  using view               = std::false_type;


  domain_type    domain() const;
  reference      operator()(point_type);
  reference      at(point_type);
  new_pixel_type new_pixel(point_type);
  new_pixel_type new_pixel_at(point_type);

  struct pixel_range
  {
    const new_pixel_type* begin();
    const new_pixel_type* end();
  };
  pixel_range new_pixels();


  struct value_range
  {
    const value_type* begin();
    const value_type* end();
  };

  value_range new_values();
};

#ifdef PYLENE_CONCEPT_TS_ENABLED
static_assert(mln::concepts::Domain<mln::ranges::where_t<mask_archetype>>);
#endif
