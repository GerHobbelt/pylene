#include <mln/accu/accumulators/mean.hpp>

#define BOOST_TEST_MODULE Accu
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(Mean)
{
  using namespace mln::accu;

 accumulators::mean<unsigned> acc;
 accumulators::mean<unsigned> acc2;

 acc.take(5);
 acc.take(15);

 BOOST_CHECK_EQUAL( extractor::sum(acc), 20);
 BOOST_CHECK_EQUAL( extractor::count(acc), 2);
 BOOST_CHECK_EQUAL( extractor::mean(acc), 10);

 acc.untake(10);

 BOOST_CHECK_EQUAL( extractor::sum(acc), 10);
 BOOST_CHECK_EQUAL( extractor::count(acc), 1);
 BOOST_CHECK_EQUAL( extractor::mean(acc), 10);


 acc2.take(20);
 acc2.take(40);
 acc2.take(acc);

 BOOST_CHECK_EQUAL( extractor::sum(acc2), 70);
 BOOST_CHECK_EQUAL( extractor::count(acc2), 3);
 BOOST_CHECK_EQUAL( extractor::mean(acc2), 23);
}
