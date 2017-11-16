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

  acc.take(rgb8{(uint8)5, (uint8)5, (uint8)255});
  acc.take(rgb8{(uint8)255, (uint8)5, (uint8)255});
  acc.take(rgb8{(uint8)2, (uint8)2, (uint8)25});

  {
    auto res = extractor::minimal_elements(acc);
    ASSERT_EQ(res.size(), 1u);
    ASSERT_EQ(res[0], (rgb8{(uint8)2, (uint8)2, (uint8)25}));
  }

  acc.take(rgb8{(uint8)5, (uint8)8, (uint8)50});
  acc.take(rgb8{(uint8)4, (uint8)1, (uint8)25});

  {
    auto res = extractor::minimal_elements(acc);
    ASSERT_EQ(res.size(), 2u);
    ASSERT_EQ(res[0], (rgb8{(uint8)2, (uint8)2, (uint8)25}));
    ASSERT_EQ(res[1], (rgb8{(uint8)4, (uint8)1, (uint8)25}));
  }
}
