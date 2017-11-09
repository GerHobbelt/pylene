#include <mln/accu/accumulators/count.hpp>

#include <gtest/gtest.h>

TEST(Accu, Sum)
{
    using namespace mln::accu;

    accumulators::count<unsigned> acc;
    accumulators::count<unsigned> acc2;

    acc.take(12);
    acc.take("blabla");

    ASSERT_EQ(extractor::count(acc), 2);

    acc.untake(13);

    ASSERT_EQ(extractor::count(acc), 1);

    acc2.take(69);
    acc.take(69);
    acc2.take(acc);

    ASSERT_EQ(extractor::count(acc2), 3);
}
