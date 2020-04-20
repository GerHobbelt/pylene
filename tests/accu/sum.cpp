#include <mln/accu/accumulators/sum.hpp>

#include <gtest/gtest.h>

TEST(Accu, Sum)
{
  using namespace mln::accu;

  accumulators::sum<unsigned> acc;
  accumulators::sum<unsigned> acc2;

  acc.take(12);
  acc.take(13);

  ASSERT_EQ(extractor::sum(acc), 25u);

  acc.untake(10);

  ASSERT_EQ(extractor::sum(acc), 15u);

  acc2.take(69);
  acc2.take(acc);

  ASSERT_EQ(extractor::sum(acc2), 84u);
}
