#include <mln/morpho/private/directional_hqueue.hpp>

#include <mln/core/algorithm/iota.hpp>
#include <mln/core/functional_ops.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c6.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/range/foreach.hpp>

#include <gtest/gtest.h>

TEST(Morpho, DirectionalHQueueC4)
{
  mln::image2d<std::uint8_t> ima = {
      {10, 15, 32}, //
      {12, 16, 8},  //
      {7, 28, 20}   //
  };

  auto dist = [](std::uint8_t a, std::uint8_t b) -> std::uint8_t { return mln::functional::l2dist_t<>()(a, b); };
  using F   = decltype(dist);
  using V   = mln::image_value_t<decltype(ima)>;
  using P   = mln::image_point_t<decltype(ima)>;
  using W   = std::invoke_result_t<F, V, V>;

  constexpr std::array<W, 12> ref = {1, 2, 4, 5, 5, 8, 8, 12, 12, 17, 21, 24};

  auto hqueue = mln::morpho::details::directional_hqueue<P, mln::c4_t, W>();

  mln_foreach (auto p, ima.domain())
  {
    int i = 0;
    for (auto q : mln::c4.after(p))
    {
      if (ima.domain().has(q))
        hqueue.insert(i, dist(ima(p), ima(q)), p);
      i++;
    }
  }

  std::size_t n = 0;
  while (!hqueue.empty())
  {
    auto [p, q, w] = hqueue.pop();
    auto rw        = ref[n];
    ASSERT_EQ(w, rw);
    n++;
  }
  ASSERT_EQ(n, 12);
}

TEST(Morpho, DirectionalHQueueC8)
{
  mln::image2d<std::uint8_t> ima = {
      {10, 15, 32}, //
      {12, 16, 8},  //
      {7, 28, 20}   //
  };

  auto dist = [](std::uint8_t a, std::uint8_t b) -> std::uint8_t { return mln::functional::l2dist_t<>()(a, b); };
  using F   = decltype(dist);
  using V   = mln::image_value_t<decltype(ima)>;
  using P   = mln::image_point_t<decltype(ima)>;
  using W   = std::invoke_result_t<F, V, V>;

  constexpr std::array<W, 20> ref = {1, 2, 3, 4, 4, 5, 5, 6, 7, 8, 8, 9, 12, 12, 16, 16, 17, 20, 21, 24};

  auto hqueue = mln::morpho::details::directional_hqueue<P, mln::c8_t, W>();

  mln_foreach (auto p, ima.domain())
  {
    int i = 0;
    for (auto q : mln::c8.after(p))
    {
      if (ima.domain().has(q))
        hqueue.insert(i, dist(ima(p), ima(q)), p);
      i++;
    }
  }

  std::size_t n = 0;
  while (!hqueue.empty())
  {
    auto [p, q, w] = hqueue.pop();
    auto rw        = ref[n];
    ASSERT_EQ(w, rw);
    n++;
  }
  ASSERT_EQ(n, 20);
}

TEST(Morpho, DirectionalHQueueC6)
{
  mln::image3d<std::uint8_t> ima = {
      {{10, 15, 32}, //
       {12, 16, 8},  //
       {7, 28, 20}}, //
      {{12, 6, 3},   //
       {27, 1, 9},   //
       {7, 15, 2}}   //
  };

  auto dist = [](std::uint8_t a, std::uint8_t b) -> std::uint8_t { return mln::functional::l2dist_t<>()(a, b); };
  using F   = decltype(dist);
  using V   = mln::image_value_t<decltype(ima)>;
  using P   = mln::image_point_t<decltype(ima)>;
  using W   = std::invoke_result_t<F, V, V>;

  constexpr std::array<W, 33> ref = {0, 1,  1,  2,  2,  3,  4,  5,  5,  5,  6,  6,  7,  8,  8,  8, 8,
                                     9, 12, 12, 13, 13, 14, 15, 15, 15, 17, 18, 20, 21, 24, 26, 29};

  auto hqueue = mln::morpho::details::directional_hqueue<P, mln::c6_t, W>();

  mln_foreach (auto p, ima.domain())
  {
    int i = 0;
    for (auto q : mln::c6.after(p))
    {
      if (ima.domain().has(q))
        hqueue.insert(i, dist(ima(p), ima(q)), p);
      i++;
    }
  }

  std::size_t n = 0;
  while (!hqueue.empty())
  {
    auto [p, q, w] = hqueue.pop();
    auto rw        = ref[n];
    ASSERT_EQ(w, rw);
    n++;
  }
  ASSERT_EQ(n, 33);
}

TEST(Morpho, DirectionalHQueueResizeHVector)
{
  using I = mln::image2d<std::uint16_t>;
  using P = mln::image_point_t<I>;

  mln::image_build_params params;
  params.init_value = std::make_any<std::uint16_t>(0);
  I img(100, 100, params);
  img({0, 0}) = 20;

  auto hqueue = mln::morpho::details::directional_hqueue<P, mln::c4_t, std::uint16_t>();
  auto dist   = [](const std::uint16_t a, std::uint16_t b) -> std::uint16_t {
    return std::abs(static_cast<int>(a) - static_cast<int>(b));
  };

  mln_foreach (auto p, img.domain())
  {
    int i = 0;
    for (auto q : mln::c4.after(p))
    {
      if (img.domain().has(q))
        hqueue.insert(i, dist(img(p), img(q)), p);
      i++;
    }
  }

  std::uint16_t prev = 0;
  while (!hqueue.empty())
  {
    auto [p, q, w] = hqueue.pop();
    ASSERT_LE(prev, w);
    prev = w;
  }
}