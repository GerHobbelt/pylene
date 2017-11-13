#include <mln/accu/accumulators/infsup.hpp>
#include <mln/accu/accumulators/minmax.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/vec.hpp>

#include <gtest/gtest.h>

TEST(Accu, Infsup)
{
  using namespace mln;
  using namespace mln::accu;

  typedef vec3i Vec;

  // product order comparison
  {
    accumulators::infsup<Vec> acc;

    acc.take(Vec(2, -5, 6));
    acc.take(Vec(4, -1, -3));
    acc.take(Vec(-2, -5, 7));

    ASSERT_EQ(extractor::inf(acc), Vec(-2, -5, -3));
    ASSERT_EQ(extractor::sup(acc), Vec(4, -1, 7));
  }

  // defaut comparison on generic vectors is lexicographical
  {
    accumulators::minmax<Vec> acc;

    acc.take(Vec(4, -5, 6));
    acc.take(Vec(4, -1, -3));
    acc.take(Vec(-2, -5, 7));

    ASSERT_EQ(extractor::min(acc), Vec(-2, -5, 7));
    ASSERT_EQ(extractor::max(acc), Vec(4, -1, -3));
  }

  // colors are partially ordered
  {
    typedef rgb<int> Vec;
    accumulators::infsup<Vec> acc;

    acc.take(Vec(4, -5, 6));
    acc.take(Vec(4, -1, -3));
    acc.take(Vec(-2, -5, 7));

    ASSERT_EQ(extractor::inf(acc), Vec(-2, -5, -3));
    ASSERT_EQ(extractor::sup(acc), Vec(4, -1, 7));
  }

  // colors are not totally ordered => this does not compile
  /*
  {
    typedef rgb<int> Vec;
    accumulators::minmax<Vec> acc;

    acc.take(Vec(4,-5,6));
    acc.take(Vec(4,-1,-3));
    acc.take(Vec(-2,-5,7));

    ASSERT_EQ( extractor::min(acc), Vec(-2,-5,-3));
    ASSERT_EQ( extractor::max(acc), Vec(4,-1,6));
  }
  */
}
