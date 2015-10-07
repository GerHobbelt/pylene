#include <mln/accu/accumulators/h_rank.hpp>
#include <mln/core/value/int.hpp>
#include <ratio>

#define BOOST_TEST_MODULE Accu
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(Rank)
{
  using namespace mln::accu;
  using mln::uint8;

  typedef std::ratio<1,2> R;


  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(0);
    acc.take(1);
    acc.take(2);

    BOOST_CHECK_EQUAL(extractor::h_rank<R>(acc), 1);
  }

  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(2);
    acc.take(1);
    acc.take(0);

    BOOST_CHECK_EQUAL(extractor::h_rank<R>(acc), 1);
  }

  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(2);
    acc.take(1);
    acc.take(1);
    acc.take(0);

    BOOST_CHECK_EQUAL(extractor::h_rank<R>(acc), 1);
  }

  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(2);
    acc.take(1);
    acc.take(1);
    acc.take(2);

    BOOST_CHECK_EQUAL(extractor::h_rank<R>(acc), 2);
  }

  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(2);
    acc.take(1);
    acc.take(1);
    acc.take(2);
    acc.take(3);
    acc.take(0);

    BOOST_CHECK_EQUAL(extractor::h_rank<R>(acc), 2);
  }

  {
    accumulators::h_rank<uint8, R> acc;
    acc.take(1);
    acc.take(2);
    acc.take(3);
    acc.take(5);
    acc.take(6);
    acc.untake(3);

    BOOST_CHECK_EQUAL(extractor::h_rank<R>(acc), 5);
  }

  {
    accumulators::h_rank<bool, R> acc;
    acc.take(0);
    acc.take(1);
    acc.take(1);
    acc.take(0);

    BOOST_CHECK_EQUAL(extractor::h_rank<R>(acc), true);
  }

  {
    accumulators::h_rank<bool, R> acc;
    acc.take(1);
    acc.take(1);
    acc.take(1);
    acc.take(0);

    BOOST_CHECK_EQUAL(extractor::h_rank<R>(acc), true);
  }

}
