#include <mln/labeling/accumulate.hpp>


#include <mln/core/image/ndimage.hpp>
#include <fmt/core.h>
#include <gtest/gtest.h>


struct mass_center_t : mln::Accumulator<mass_center_t>
{
  using P             = mln::point2d;
  using argument_type = P;
  using result_type   = P;

  void init() { *this = mass_center_t{}; }
  void take(P p) noexcept { xsum += p.x(); ysum += p.y(); count += 1; }
  void take(const mass_center_t& o) noexcept { xsum += o.xsum; ysum += o.ysum; count += o.count; }
  mln::point2d to_result() const noexcept { return {(int)(xsum / (float)count + .5f), (int)(ysum / (float)count + .5f)}; }

private:
  int xsum = 0;
  int ysum = 0;
  int count = 0;
};


TEST(Labeling, accumulate_U)
{
  // mln::image2d<bool> ima = {{0, 1, 0, 0, 0, 1, 0}, //
  //                                         {0, 1, 0, 1, 0, 1, 0}, //
  //                                         {0, 1, 0, 0, 0, 1, 0}, //
  //                                         {0, 1, 1, 1, 1, 1, 0}, //
  //                                         {0, 1, 1, 1, 1, 1, 0}};

  mln::image2d<uint8_t> labels = {{0, 1, 0, 0, 0, 1, 0}, //
                                                {0, 1, 0, 2, 0, 1, 0}, //
                                                {0, 1, 0, 0, 0, 1, 0}, //
                                                {0, 1, 1, 1, 1, 1, 0}, //
                                                {0, 1, 1, 1, 1, 1, 0}};

  std::vector<mln::point2d> ref = {{3,2}, {3,3}, {3,1}};
  auto res = mln::labeling::accumulate(labels, 2, mass_center_t{});
  ASSERT_EQ(res, ref);
}

struct average_t : mln::Accumulator<average_t>
{
  using V             = int;
  using argument_type = int;
  using result_type   = float;

  void init() { *this = average_t{}; }
  void take(V v) noexcept { sum += v; count += 1; }
  void take(const average_t& o) noexcept { sum += o.sum; count += o.count; }
  float to_result() const noexcept { return sum / (float)count; }

private:
  int sum = 0;
  int count = 0;
};



TEST(Labeling, accumulate_on_U)
{
  mln::image2d<uint8_t> ima = {{0, 1, 2, 3, 4, 5, 6}, //
                                             {0, 1, 2, 3, 4, 5, 6}, //
                                             {0, 1, 2, 3, 4, 5, 6}, //
                                             {0, 1, 2, 3, 4, 5, 6}, //
                                             {0, 1, 2, 3, 4, 5, 6}};

  mln::image2d<uint8_t> labels = {{0, 1, 0, 0, 0, 1, 0}, //
                                                {0, 1, 0, 2, 0, 1, 0}, //
                                                {0, 1, 0, 0, 0, 1, 0}, //
                                                {0, 1, 1, 1, 1, 1, 0}, //
                                                {0, 1, 1, 1, 1, 1, 0}};

  std::vector<float> ref = {3,3,3};
  auto res = mln::labeling::accumulate_on(labels, ima, 2, average_t{});
  ASSERT_EQ(res, ref);
}



