#include <mln/accu/accumulator.hpp>
#include <mln/core/algorithm/accumulate_local.hpp>
#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/se/rect2d.hpp>


#include <gtest/gtest.h>


struct accu_sum_impl
{
  using argument_type = int;
  using result_type   = int;
  int sum             = 0;

  void init() { sum = 0; }
  void take(int x) { sum += x; }
  void untake(int x) { sum -= x; }
  void take(const accu_sum_impl& other) { sum += other.sum; }
  int  to_result() const { return sum; }
};


struct accu_incremental : accu_sum_impl, mln::Accumulator<accu_incremental>
{
  using has_untake = std::true_type;
};

struct accu_non_incremental : accu_sum_impl, mln::Accumulator<accu_non_incremental>
{
  using has_untake = std::false_type;
};


TEST(Core, Accumulate_Incremental_Border_Is_Large_Enough)
{
  mln::image2d<uint8_t> input(5, 6);

  mln::image2d<uint8_t> ref = {
      {19, 27, 33, 39, 45, 35},       //
      {42, 63, 72, 81, 90, 66},       //
      {78, 117, 126, 135, 144, 102},  //
      {114, 171, 180, 189, 198, 138}, //
      {91, 135, 141, 147, 153, 107},  //
  };


  mln::iota(input, 0);
  // FIXME: gcc8 random compilation failure in release
  /*
  auto output = mln::accumulate_local(input, mln::experimental::se::rect2d{3, 3}, accu_incremental{},
                                      mln::extension::bm::fill(uint8_t(1)));

  using namespace mln::view::ops;

  ASSERT_TRUE(mln::all_of(output == ref));
  */
}

TEST(Core, Accumulate_NonIncremental_Border_Is_Large_Enough)
{
  mln::image2d<uint8_t> input(5, 6);

  mln::image2d<uint8_t> ref = {
      {19, 27, 33, 39, 45, 35},       //
      {42, 63, 72, 81, 90, 66},       //
      {78, 117, 126, 135, 144, 102},  //
      {114, 171, 180, 189, 198, 138}, //
      {91, 135, 141, 147, 153, 107},  //
  };


  mln::iota(input, 0);

  auto output = mln::accumulate_local(input, mln::experimental::se::rect2d{3, 3}, accu_non_incremental{},
                                      mln::extension::bm::fill(uint8_t(1)));

  using namespace mln::view::ops;

  ASSERT_TRUE(mln::all_of(output == ref));
}

TEST(Core, Accumulate_Incremental_Border_Is_Not_Large_Enough)
{
  mln::image2d<uint8_t> input(mln::box2d{{0, 0}, {5, 6}}, 0);


  mln::image2d<uint8_t> ref = {
      {19, 27, 33, 39, 45, 35},       //
      {42, 63, 72, 81, 90, 66},       //
      {78, 117, 126, 135, 144, 102},  //
      {114, 171, 180, 189, 198, 138}, //
      {91, 135, 141, 147, 153, 107},  //
  };


  mln::iota(input, 0);

  auto output = mln::accumulate_local(input, mln::experimental::se::rect2d{3, 3}, accu_incremental{},
                                      mln::extension::bm::fill(uint8_t(1)));

  using namespace mln::view::ops;

  ASSERT_TRUE(mln::all_of(output == ref));
}


TEST(Core, Accumulate_NonIncremental_Border_Is_Not_Large_Enough)
{
  mln::image2d<uint8_t> input(mln::box2d{{0, 0}, {5, 6}}, 0);


  mln::image2d<uint8_t> ref = {
      {19, 27, 33, 39, 45, 35},       //
      {42, 63, 72, 81, 90, 66},       //
      {78, 117, 126, 135, 144, 102},  //
      {114, 171, 180, 189, 198, 138}, //
      {91, 135, 141, 147, 153, 107},  //
  };


  mln::iota(input, 0);

  auto output = mln::accumulate_local(input, mln::experimental::se::rect2d{3, 3}, accu_non_incremental{},
                                      mln::extension::bm::fill(uint8_t(1)));

  using namespace mln::view::ops;

  ASSERT_TRUE(mln::all_of(output == ref));
}
