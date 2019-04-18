#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/concept/new/archetype/image.hpp>
#include <mln/core/domain/where.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/clip.hpp>
#include <mln/core/image/view/operators.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>


struct vector_domain : public std::vector<mln::point2d>
{
  using base = std::vector<mln::point2d>;

  using base::base;
  bool has(mln::point2d p) const { return std::binary_search(this->begin(), this->end(), p); }
};


TEST(View, clip)
{
  using namespace mln::view::ops;

  mln::image2d<int> ima = {{0, 1, 2, 3, 4}, //
                           {5, 6, 4, 8, 9}, //
                           {10, 11, 12, 13, 14}};

  mln::image2d<int> ref = {{42, 1, 2, 3, 42}, //
                           {5, 42, 4, 42, 9}, //
                           {10, 11, 42, 13, 14}};

  vector_domain domain = {{0, 0}, {0, 4}, {1, 1}, {1, 3}, {2, 2}};


  auto clipped = mln::view::clip(ima, domain);
  fill(clipped, 42);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OutputImage<decltype(clipped)>);
  static_assert(mln::concepts::ViewImage<decltype(clipped)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(clipped)>);
  static_assert(not mln::concepts::BidirectionalImage<decltype(clipped)>);
  static_assert(not mln::concepts::RawImage<decltype(clipped)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto p : clipped.domain())
  {
    ASSERT_EQ(42, ima(p));
    ASSERT_EQ(42, clipped(p));
  }

  ASSERT_TRUE(mln::all_of(ima == ref));
}


TEST(View, clip_twice)
{
  using namespace mln::view::ops;

  mln::image2d<int> ima = {{0, 1, 2, 3, 4}, //
                           {5, 6, 4, 8, 9}, //
                           {10, 11, 12, 13, 14}};

  mln::image2d<int> ref = {{0, 1, 2, 3, 4},   //
                           {5, 42, 4, 42, 9}, //
                           {10, 11, 12, 13, 14}};

  vector_domain domain_a = {{0, 0}, {0, 4}, {1, 1}, {1, 3}, {2, 2}};
  vector_domain domain_b = {{1, 1}, {1, 3}};


  auto A = mln::view::clip(ima, domain_a);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OutputImage<decltype(A)>);
  static_assert(mln::concepts::ViewImage<decltype(A)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(A)>);
  static_assert(not mln::concepts::BidirectionalImage<decltype(A)>);
  static_assert(not mln::concepts::RawImage<decltype(A)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  auto B = mln::view::clip(A, domain_b);
  fill(B, 42);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OutputImage<decltype(B)>);
  static_assert(mln::concepts::ViewImage<decltype(B)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(B)>);
  static_assert(not mln::concepts::BidirectionalImage<decltype(B)>);
  static_assert(not mln::concepts::RawImage<decltype(B)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto p : B.domain())
  {
    ASSERT_EQ(42, ima(p));
    ASSERT_EQ(42, B(p));
  }

  ASSERT_TRUE(mln::all_of(ima == ref));
}


TEST(View, clip_other_a_box2d)
{
  using namespace mln::view::ops;

  mln::image2d<int> ima = {{0, 1, 2, 3, 4}, //
                           {5, 6, 4, 8, 9}, //
                           {10, 11, 12, 13, 14}};

  mln::image2d<int> ref = {{0, 42, 42, 3, 4}, //
                           {5, 42, 42, 8, 9}, //
                           {10, 11, 12, 13, 14}};

  mln::box2d domain = {{0, 1}, {2, 3}};

  // Clip returns an 'image2d'
  mln::image2d<int> clipped = mln::view::clip(ima, domain);
  fill(clipped, 42);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OutputImage<decltype(clipped)>);
  static_assert(not mln::concepts::ViewImage<decltype(clipped)>);
  static_assert(mln::concepts::ConcreteImage<decltype(clipped)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(clipped)>);
  static_assert(mln::concepts::BidirectionalImage<decltype(clipped)>);
  static_assert(mln::concepts::RawImage<decltype(clipped)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto p : clipped.domain())
  {
    ASSERT_EQ(42, ima(p));
    ASSERT_EQ(42, clipped(p));
  }

  ASSERT_TRUE(mln::all_of(ima == ref));
}

TEST(Core, Clip_where)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<int> ima(5, 5);

  image2d<int> ref = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 42, 42, 42, 42}, //
      {42, 42, 42, 42, 42}, //
      {42, 42, 42, 42, 42}  //
  };

  mln::iota(ima, 0);
  mln::fill(view::clip(ima, experimental::where(ima > 10)), 42);
  ASSERT_TRUE(all_of(ima == ref));
}

TEST(Core, Clip_wherex2_joints)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<int> ima(5, 5);

  image2d<int> ref = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 11, 12, 13, 14}, //
      {15, 16, 17, 18, 19}, //
      {20, 42, 42, 42, 42}  //
  };

  mln::iota(ima, 0);
  mln::fill(view::clip(view::clip(ima, experimental::where(ima > 10)), experimental::where(ima > 20)), 42);
  ASSERT_TRUE(all_of(ima == ref));
}

TEST(Core, Clip_where_and)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<int> ima(5, 5);

  image2d<int> ref = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 42, 42, 42, 42}, //
      {42, 42, 42, 42, 42}, //
      {20, 21, 22, 23, 24}  //
  };

  mln::iota(ima, 0);
  mln::fill(view::clip(ima, experimental::where(ima > 10 && ima < 20)), 42);
  ASSERT_TRUE(all_of(ima == ref));
}


PYLENE_CONCEPT_TS_ASSERT(
    (mln::concepts::AccessibleImage<mln::clip_view<mln::archetypes::AccessibleImage, mln::archetypes::Domain>>), "");
PYLENE_CONCEPT_TS_ASSERT(
    (mln::concepts::OutputImage<mln::clip_view<mln::archetypes::OutputAccessibleImage, mln::archetypes::Domain>>), "");
PYLENE_CONCEPT_TS_ASSERT((mln::concepts::IndexableAndAccessibleImage<
                             mln::clip_view<mln::archetypes::IndexableAndAccessibleImage, mln::archetypes::Domain>>),
                         "");
