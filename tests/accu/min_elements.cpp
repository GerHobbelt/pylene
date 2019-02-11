#include <mln/accu/accumulators/min_elements.hpp>
#include <mln/core/colors.hpp>

#include <gtest/gtest.h>

TEST(Accu, Min_elements)
{
  using namespace mln::accu;

  accumulators::minimal_elements<unsigned> acc;

  acc.take(5);
  acc.take(15);
  acc.take(2);

  {
    auto res = extractor::minimal_elements(acc);
    ASSERT_EQ(res.size(), 1u);
    ASSERT_EQ(res[0], 2u);
  }

  acc.take(18);
  acc.take(2);

  {
    auto res = extractor::minimal_elements(acc);
    ASSERT_EQ(res.size(), 1u);
    ASSERT_EQ(res[0], 2u);
  }
}

TEST(Accu, Min_elements_vec)
{
  using namespace mln;
  using namespace mln::accu;

  accumulators::minimal_elements<rgb8> acc;

  acc.take(rgb8{5, 5, 255});
  acc.take(rgb8{255, 5, 255});
  acc.take(rgb8{2, 2, 25});

  {
    auto res = extractor::minimal_elements(acc);
    ASSERT_EQ(res.size(), 1u);
    ASSERT_EQ(res[0], (rgb8{2, 2, 25}));
  }

  acc.take(rgb8{5, 8, 50});
  acc.take(rgb8{4, 1, 25});

  {
    auto res = extractor::minimal_elements(acc);
    ASSERT_EQ(res.size(), 2u);
    ASSERT_EQ(res[0], (rgb8{2, 2, 25}));
    ASSERT_EQ(res[1], (rgb8{4, 1, 25}));
  }
}
