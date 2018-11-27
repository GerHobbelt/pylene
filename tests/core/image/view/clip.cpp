#include <mln/core/image/view/clip.hpp>
#include <mln/core/image/private/image_operators.hpp>
#include <mln/core/image/image2d.hpp>

#include <mln/core/algorithm/fill.hpp>

#include <mln/core/rangev3/foreach.hpp>

#include <gtest/gtest.h>
#include <helpers.hpp>


struct vector_domain : public std::vector<mln::point2d>
{
  using base = std::vector<mln::point2d>;

  using base::base;
  bool has(mln::point2d p) const { return std::binary_search(this->begin(), this->end(), p); }
};


TEST(View, clip)
{
  mln::image2d<int> ima = {{0, 1, 2, 3, 4},
                      {5, 6, 4, 8, 9},
                      {10, 11, 12, 13, 14}};

  mln::image2d<int> ref = {{42, 1, 2, 3, 42},
                      {5, 42, 4, 42, 9},
                      {10, 11, 42, 13, 14}};

  vector_domain domain = { {0,0}, {0,4}, {1,1}, {1,3}, {2,2} };


  auto clipped = mln::view::clip(ima, domain);
  fill(clipped, 42);

  for (auto p : clipped.domain())
  {
    ASSERT_EQ(42, ima(p));
    ASSERT_EQ(42, clipped(p));
  }

  ASSERT_TRUE(mln::new_all(mln::new_eq(ima, ref)));
}


TEST(View, clip_twice)
{
  mln::image2d<int> ima = {{0, 1, 2, 3, 4},
                           {5, 6, 4, 8, 9},
                           {10, 11, 12, 13, 14}};

  mln::image2d<int> ref = {{0, 1, 2, 3, 4},
                           {5, 42, 4, 42, 9},
                           {10, 11, 12, 13, 14}};

  vector_domain domain_a = { {0,0}, {0,4}, {1,1}, {1,3}, {2,2} };
  vector_domain domain_b = { {1,1}, {1,3} };


  auto A = mln::view::clip(ima, domain_a);
  auto B = mln::view::clip(ima, domain_b);
  fill(B, 42);

  for (auto p : B.domain())
  {
    ASSERT_EQ(42, ima(p));
    ASSERT_EQ(42, B(p));
  }

  ASSERT_TRUE(mln::new_all(mln::new_eq(ima, ref)));
}

