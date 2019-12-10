#include <mln/io/experimental/imprint.hpp>
#include <mln/morpho/experimental/canvas/depthfirst.hpp>

#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/algorithm/all_of.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>
#include <mln/io/experimental/imprint.hpp>
#include <functional>
#include <fmt/core.h>

struct ordervisitor
{
  void on_flood_start(int, mln::experimental::point2d) noexcept { ++nb_component_entering; }
  void on_flood_end(int) noexcept { ++nb_component_exiting; }
  void on_done(int, mln::experimental::point2d p) { out(p) = count++; cnt(p)++; }

  int                                 nb_component_entering = 0;
  int                                 nb_component_exiting = 0;
  int                                 count = 0;
  mln::experimental::image2d<uint8_t> out;
  mln::experimental::image2d<int>     cnt;

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

  mln::morpho::experimental::canvas::depthfirst(input, mln::experimental::c4, viz, {0,0});

  //mln::io::experimental::imprint(input >= 2); fmt::print("\n");
  //mln::io::experimental::imprint(input >= 6); fmt::print("\n");
  //mln::io::experimental::imprint(input >= 10); fmt::print("\n");
  //mln::io::experimental::imprint(input >= 11); fmt::print("\n");
  //mln::io::experimental::imprint(input >= 14); fmt::print("\n");
  //mln::io::experimental::imprint(input >= 15); fmt::print("\n");
  //mln::io::experimental::imprint(input >= 16); fmt::print("\n");
  //mln::io::experimental::imprint(input >= 18); fmt::print("\n");
  //mln::io::experimental::imprint(input >= 19); fmt::print("\n");


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
  //ASSERT_IMAGES_EQ_EXP(ref, res);
}
