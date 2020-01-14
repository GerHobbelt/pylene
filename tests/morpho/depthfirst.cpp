#include <mln/io/experimental/imprint.hpp>
#include <mln/morpho/experimental/canvas/depthfirst.hpp>

#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>

#include <functional>
#include <bitset>

#include <fmt/core.h>

struct ordervisitor
{
  void on_flood_start(int l, mln::experimental::point2d) noexcept { ++nb_component_entering; levelset.set(l); }
  void on_flood_end(int l, int) noexcept { ++nb_component_exiting; levelset.reset(l); }
  void on_flood_end(int l) noexcept { ++nb_component_exiting; levelset.reset(l); }
  void on_done(int, mln::experimental::point2d p) { out(p) = count++; cnt(p)++; }
  bool has_level_flooding_started(int l) const { return levelset.test(l); }

  int                                 nb_component_entering = 0;
  int                                 nb_component_exiting = 0;
  int                                 count = 0;
  mln::experimental::image2d<uint8_t> out;
  mln::experimental::image2d<int>     cnt;

  std::bitset<256> levelset;
};


TEST(Morpho, depthfirst_max)
{
  using namespace mln::view::ops;

  const mln::experimental::image2d<uint8_t> input = {{10, 11, 11, 15, 16, 11, +2}, //
                                                     {+2, 10, 10, 10, 10, 10, 10}, //
                                                     {18, +2, 18, 19, 18, 14, +6}, //
                                                     {16, +2, 16, 10, 10, 10, 10}, //
                                                     {18, 16, 18, +2, +2, +2, +2}};


  ordervisitor viz;
  viz.out.resize(input.domain());
  viz.cnt.resize(input.domain());

  mln::fill(viz.cnt, 0);
  mln::morpho::experimental::canvas::depthfirst(input, mln::experimental::c4, viz, {0,0});

  // Counting = anti-leveling
  mln_foreach_new(auto p, input.domain())
    for (auto q : mln::experimental::c4(p))
      if (input.domain().has(q))
      {
        if (input(p) > input(q)) {
          ASSERT_LT(viz.out(p), viz.out(q));
        }
      }

  EXPECT_EQ(viz.nb_component_entering, viz.nb_component_exiting);
  EXPECT_EQ(viz.nb_component_exiting, 13);
  EXPECT_TRUE(mln::all_of(viz.cnt == 1));
}
