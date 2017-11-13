#include <mln/accu/accumulators/h_rank.hpp>
#include <mln/core/value/int.hpp>
#include <ratio>

#include <gtest/gtest.h>

TEST(Accu, Rank)
{
  using namespace mln::accu;
  using mln::uint8;

  typedef std::ratio<1, 2> R;

  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(0);
    acc.take(1);
    acc.take(2);

    ASSERT_EQ(extractor::h_rank<R>(acc), 1);
  }

  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(2);
    acc.take(1);
    acc.take(0);

    ASSERT_EQ(extractor::h_rank<R>(acc), 1);
  }

  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(2);
    acc.take(1);
    acc.take(1);
    acc.take(0);

    ASSERT_EQ(extractor::h_rank<R>(acc), 1);
  }

  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(2);
    acc.take(1);
    acc.take(1);
    acc.take(2);

    ASSERT_EQ(extractor::h_rank<R>(acc), 2);
  }

  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(2);
    acc.take(1);
    acc.take(1);
    acc.take(2);
    acc.take(3);
    acc.take(0);

    ASSERT_EQ(extractor::h_rank<R>(acc), 2);
  }

  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(1);
    acc.take(2);
    acc.take(3);
    acc.take(5);
    acc.take(6);
    acc.untake(3);

    ASSERT_EQ(extractor::h_rank<R>(acc), 5);
  }

  {
    accumulators::h_rank<bool, R> acc;
    acc.take(0);
    acc.take(1);
    acc.take(1);
    acc.take(0);

    ASSERT_EQ(extractor::h_rank<R>(acc), true);
  }

  {
    accumulators::h_rank<bool, R> acc;
    acc.take(1);
    acc.take(1);
    acc.take(1);
    acc.take(0);

    ASSERT_EQ(extractor::h_rank<R>(acc), true);
  }
}
