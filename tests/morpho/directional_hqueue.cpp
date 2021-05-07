#include <mln/morpho/private/directional_hqueue.hpp>

#include <mln/core/functional_ops.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/range/foreach.hpp>

#include <gtest/gtest.h>

TEST(Morpho, DirectionalHQueue)
{
  mln::image2d<std::uint8_t> ima = {
      {10, 15, 32}, //
      {12, 16, 8},  //
      {7, 28, 20}   //
  };

  auto dist = [](std::uint8_t a, std::uint8_t b) -> std::uint8_t { return mln::functional::l2dist_t<>()(a, b); };
  using F   = decltype(dist);
  using V   = mln::image_value_t<decltype(ima)>;
  using W   = std::invoke_result_t<F, V, V>;

  constexpr std::array<W, 12> ref = {1, 2, 4, 5, 5, 8, 8, 12, 12, 17, 21, 24};

  auto hqueue = mln::morpho::details::directional_hqueue<decltype(ima), mln::c4_t, W>();

  mln_foreach (auto p, ima.domain())
  {
    std::size_t i = 0;
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