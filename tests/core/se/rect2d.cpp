#include <gtest/gtest.h>
#include <mln/core/se/rect2d.hpp>

#ifdef PYLENE_CONCEPT_TS_ENABLED
static_assert(mln::concepts::StructuringElement<mln::experimental::se::rect2d, mln::point2d>);
static_assert(mln::concepts::SeparableStructuringElement<mln::experimental::se::rect2d, mln::point2d>);
static_assert(mln::concepts::DecomposableStructuringElement<mln::experimental::se::rect2d, mln::point2d>);
static_assert(mln::concepts::IncrementalStructuringElement<mln::experimental::se::rect2d, mln::point2d>);
#endif


template <class SE>
void test_decompose(SE se)
{
  auto ses = se.decompose();

  mln::point2d origin = {0,0};


  std::set<mln::point2d> v1;
  {
    for (auto p : se(origin))
      v1.insert(p);
  }

  std::set<mln::point2d> v2;
  {
    v2.insert(origin);
    for (auto se : ses)
    {
      std::set<mln::point2d> tmp;
      for (auto p : v2)
        for (auto q : se(p))
          tmp.insert(q);
      v2 = std::move(tmp);
    }
  }
  EXPECT_EQ(v1, v2);
}


TEST(Core, Rect2d)
{
  mln::experimental::se::rect2d win(5, 3);


  auto rng = win(mln::point2d{0,0});
  auto p = ::ranges::begin(rng);

  for (int i = -1; i <= 1; ++i)
    for (int j = -2; j <= 2; ++j, ++p)
      ASSERT_EQ(*p, mln::point2d(i, j));

  EXPECT_EQ(::ranges::end(rng), p) << "Iterators end do not match.";
}

TEST(Core, Rect2d_decompose)
{
  mln::experimental::se::rect2d win(5, 3);
  test_decompose(win);
}


