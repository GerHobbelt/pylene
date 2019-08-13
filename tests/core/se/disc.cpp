#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/se/disc.hpp>

#include <mln/core/rangev3/foreach.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <gtest/gtest.h>

void naive_dilate(mln::experimental::image2d<bool>& f, const mln::experimental::se::periodic_line2d& se)
{
  mln::experimental::image2d<bool> g;
  mln::resize(g, f).set_init_value(false);


  mln_foreach_new ((auto [pxIn, pxOut]), mln::ranges::view::zip(f.new_pixels(), g.new_pixels()))
    if (pxIn.val())
      for (auto nxOut : se(pxOut))
        if (f.domain().has(nxOut.point()))
          nxOut.val() = true;

  f = std::move(g);
}


mln::experimental::image2d<bool> draw_ball_by_decomposition(float radius, int extent, int& computed_extent)
{
  mln::experimental::box2d domain({-extent, -extent}, {extent + 1, extent + 1});

  mln::image_build_params params;
  params.init_value = false;
  params.border     = 3;

  mln::experimental::image2d<bool> f(domain, params);


  auto ball = mln::experimental::se::disc(radius, mln::experimental::se::disc::approx::PERIODIC_LINES_8);
  auto ses  = ball.decompose();

  f({0, 0}) = true;

  for (auto line : ses)
    naive_dilate(f, line);

  int k = 0;
  for (int i = -extent; i <= extent; ++i)
    k += f({i, 0});
  computed_extent = k;

  return f;
}


float compute_disc_error(const mln::experimental::image2d<bool>& f, float radius)
{
  int  nerror = 0;
  auto sqr    = [](auto x) { return x * x; };

  mln_foreach_new (auto px, f.new_pixels())
  {
    bool ref = sqr(px.point().x()) + sqr(px.point().y()) <= sqr(radius);
    nerror += (ref != px.val());
  }
  return nerror / radius;
}


// Best coef tuples (computed by brute force) from radius r=1 to 199
int best_decomposition_coeff[199][3] = {
    {1, 0, 0},    {2, 0, 0},    {1, 1, 0},    {2, 1, 0},    {1, 2, 0},    {2, 2, 0},    {1, 0, 1},    {2, 3, 0},    //
    {1, 1, 1},    {2, 1, 1},    {1, 2, 1},    {2, 2, 1},    {3, 2, 1},    {2, 0, 2},    {3, 0, 2},    {2, 1, 2},    //
    {3, 1, 2},    {4, 4, 1},    {3, 2, 2},    {4, 2, 2},    {3, 0, 3},    {4, 3, 2},    {5, 3, 2},    {4, 1, 3},    //
    {5, 1, 3},    {6, 4, 2},    {5, 2, 3},    {6, 5, 2},    {5, 3, 3},    {6, 3, 3},    {5, 1, 4},    {6, 4, 3},    //
    {7, 4, 3},    {6, 2, 4},    {7, 2, 4},    {8, 5, 3},    {7, 3, 4},    {8, 6, 3},    {9, 6, 3},    {8, 4, 4},    //
    {9, 7, 3},    {8, 5, 4},    {9, 5, 4},    {10, 5, 4},   {9, 3, 5},    {10, 6, 4},   {9, 4, 5},    {10, 7, 4},   //
    {11, 7, 4},   {10, 5, 5},   {11, 5, 5},   {12, 8, 4},   {11, 6, 5},   {12, 6, 5},   {11, 4, 6},   {12, 7, 5},   //
    {13, 7, 5},   {12, 5, 6},   {13, 8, 5},   {12, 6, 6},   {13, 6, 6},   {14, 9, 5},   {13, 7, 6},   {14, 7, 6},   //
    {15, 10, 5},  {14, 8, 6},   {15, 8, 6},   {14, 6, 7},   {15, 9, 6},   {16, 9, 6},   {15, 7, 7},   {16, 10, 6},  //
    {15, 8, 7},   {16, 8, 7},   {17, 8, 7},   {16, 9, 7},   {17, 9, 7},   {16, 7, 8},   {17, 10, 7},  {18, 10, 7},  //
    {17, 8, 8},   {18, 11, 7},  {17, 9, 8},   {18, 9, 8},   {19, 9, 8},   {18, 10, 8},  {19, 10, 8},  {20, 10, 8},  //
    {19, 11, 8},  {20, 11, 8},  {19, 9, 9},   {20, 9, 9},   {21, 12, 8},  {20, 10, 9},  {21, 10, 9},  {20, 11, 9},  //
    {21, 11, 9},  {22, 11, 9},  {21, 12, 9},  {22, 12, 9},  {21, 10, 10}, {22, 10, 10}, {23, 13, 9},  {22, 11, 10}, //
    {23, 11, 10}, {24, 14, 9},  {23, 12, 10}, {24, 12, 10}, {23, 10, 11}, {24, 13, 10}, {25, 13, 10}, {24, 11, 11}, //
    {25, 14, 10}, {24, 12, 11}, {25, 12, 11}, {26, 15, 10}, {25, 13, 11}, {26, 13, 11}, {25, 11, 12}, {26, 14, 11}, //
    {27, 14, 11}, {26, 12, 12}, {27, 15, 11}, {28, 15, 11}, {27, 13, 12}, {28, 13, 12}, {27, 14, 12}, {28, 14, 12}, //
    {29, 14, 12}, {28, 15, 12}, {29, 15, 12}, {28, 13, 13}, {29, 16, 12}, {30, 16, 12}, {29, 14, 13}, {30, 14, 13}, //
    {31, 17, 12}, {30, 15, 13}, {31, 15, 13}, {30, 16, 13}, {31, 16, 13}, {30, 14, 14}, {31, 14, 14}, {32, 17, 13}, //
    {31, 15, 14}, {32, 15, 14}, {33, 18, 13}, {32, 16, 14}, {33, 16, 14}, {34, 19, 13}, {33, 17, 14}, {34, 17, 14}, //
    {33, 15, 15}, {34, 18, 14}, {35, 18, 14}, {34, 16, 15}, {35, 19, 14}, {34, 17, 15}, {35, 17, 15}, {36, 20, 14}, //
    {35, 18, 15}, {36, 18, 15}, {35, 16, 16}, {36, 19, 15}, {37, 19, 15}, {36, 17, 16}, {37, 20, 15}, {38, 20, 15}, //
    {37, 18, 16}, {38, 18, 16}, {37, 19, 16}, {38, 19, 16}, {37, 17, 17}, {38, 20, 16}, {39, 20, 16}, {38, 18, 17}, //
    {39, 18, 17}, {40, 21, 16}, {39, 19, 17}, {40, 19, 17}, {41, 22, 16}, {40, 20, 17}, {41, 20, 17}, {40, 21, 17}, //
    {41, 21, 17}, {42, 21, 17}, {41, 19, 18}, {42, 22, 17}, {41, 20, 18}, {42, 20, 18}, {43, 23, 17}, {42, 21, 18}, //
    {43, 21, 18}, {42, 19, 19}, {43, 22, 18}, {44, 22, 18}, {43, 20, 19}, {44, 23, 18}, {45, 23, 18}};


TEST(Disc, decomposition_8_check_extent)
{
  for (int r = 1; r < 200; ++r)
  {
    auto c      = mln::se::details::disc_compute_decomposition_coeff(r);
    int  extent = c[0] + 2 * c[1] + 6 * c[2];
    ASSERT_EQ(r, extent);
  }
}

TEST(Disc, decomposition_8_check_close_to_best_coef)
{
  for (int r = 1; r < 200; ++r)
  {
    auto k = mln::se::details::disc_compute_decomposition_coeff(r);
    ASSERT_NEAR(best_decomposition_coeff[r - 1][0], k[0], 2);
    ASSERT_NEAR(best_decomposition_coeff[r - 1][1], k[1], 3);
    ASSERT_NEAR(best_decomposition_coeff[r - 1][2], k[2], 1);
  }
}


TEST(Disc, decomposition_8_errors_does_not_degenerate)
{
  int max_radius = MLN_HAS_DEBUG ? 40 : 100; // Allow processing further in release
  for (int radius = 5; radius <= max_radius; ++radius)
  {
    int  extent;
    auto f = draw_ball_by_decomposition(radius, max_radius, extent);
    ASSERT_EQ(2 * radius + 1, extent) << " for radius " << radius;
    ASSERT_LE(compute_disc_error(f, radius), 6.5) << " for radius " << radius;
  }
}

TEST(Disc, euclidean_disc_is_not_decomposable)
{
  mln::experimental::se::disc d(5, mln::experimental::se::disc::approx::EXACT);
  EXPECT_FALSE(d.is_decomposable());
  EXPECT_ANY_THROW(d.decompose());
}

TEST(Disc, approx_disc_is_decomposable)
{
  mln::experimental::se::disc d(5);
  EXPECT_TRUE(d.is_decomposable());
  EXPECT_NO_THROW(d.decompose());
}

#ifdef PYLENE_CONCEPT_TS_ENABLED
static_assert(mln::concepts::DecomposableStructuringElement<mln::experimental::se::disc, mln::experimental::point2d>);
#endif
