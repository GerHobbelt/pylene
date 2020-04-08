#include <mln/core/se/rect2d.hpp>

#include <gtest/gtest.h>

static_assert(mln::concepts::StructuringElement<mln::experimental::se::rect2d, mln::experimental::point2d>);
static_assert(mln::concepts::SeparableStructuringElement<mln::experimental::se::rect2d, mln::experimental::point2d>);
static_assert(mln::concepts::DecomposableStructuringElement<mln::experimental::se::rect2d, mln::experimental::point2d>);
static_assert(mln::concepts::IncrementalStructuringElement<mln::experimental::se::rect2d, mln::experimental::point2d>);



template <class SE>
void test_decompose(SE se)
{
  auto ses = se.decompose();

  mln::experimental::point2d origin = {0,0};


  std::set<mln::experimental::point2d> v1;
  {
    for (auto p : se(origin))
      v1.insert(p);
  }

  std::set<mln::experimental::point2d> v2;
  {
    v2.insert(origin);
    for (auto se : ses)
    {
      std::set<mln::experimental::point2d> tmp;
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


  auto rng = win(mln::experimental::point2d{0,0});
  auto p = ::ranges::begin(rng);

  for (int y = -1; y <= 1; ++y)
    for (int x = -2; x <= 2; ++x, ++p)
      ASSERT_EQ(*p, (mln::experimental::point2d{x, y}));

  EXPECT_EQ(::ranges::end(rng), p) << "Iterators end do not match.";
}

TEST(Core, Rect2d_decompose)
{
  mln::experimental::se::rect2d win(5, 3);
  test_decompose(win);
}


