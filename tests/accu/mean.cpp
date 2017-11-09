#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/colors.hpp>

#include <gtest/gtest.h>

TEST(Accu, Mean)
{
    using namespace mln::accu;

    accumulators::mean<unsigned> acc;
    accumulators::mean<unsigned> acc2;

    acc.take(5);
    acc.take(15);

    ASSERT_EQ(extractor::sum(acc), 20);
    ASSERT_EQ(extractor::count(acc), 2);
    ASSERT_EQ(extractor::mean(acc), 10);

    acc.untake(10);

    ASSERT_EQ(extractor::sum(acc), 10);
    ASSERT_EQ(extractor::count(acc), 1);
    ASSERT_EQ(extractor::mean(acc), 10);

    acc2.take(20);
    acc2.take(40);
    acc2.take(acc);

    ASSERT_EQ(extractor::sum(acc2), 70);
    ASSERT_EQ(extractor::count(acc2), 3);
    ASSERT_EQ(extractor::mean(acc2), 23);
}

TEST(Accu, Mean_vec)
{
    using namespace mln;
    using namespace mln::accu;

    accumulators::mean<rgb8> acc;
    accumulators::mean<rgb8> acc2;

    acc.take(rgb8{(uint8)5, (uint8)5, (uint8)255});
    acc.take(rgb8{(uint8)255, (uint8)5, (uint8)255});

    ASSERT_EQ(extractor::sum(acc), (rgb<int>{260, 10, 510}));
    ASSERT_EQ(extractor::count(acc), 2);
    ASSERT_EQ(extractor::mean(acc), (rgb8{(uint8)130, (uint8)5, (uint8)255}));

    acc.untake(rgb8{(uint8)10, (uint8)10, (uint8)10});

    ASSERT_EQ(extractor::sum(acc), (rgb<int>{250, 0, 500}));
    ASSERT_EQ(extractor::count(acc), 1);
    ASSERT_EQ(extractor::mean(acc), (rgb<int>{250, 0, 500}));
}
