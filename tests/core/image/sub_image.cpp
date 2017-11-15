#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/io/imprint.hpp>

#include <gtest/gtest.h>

namespace mln
{

  template <class I, class J>
  void imcheck_equal(const Image<I>& f, const Image<J>& g)
  {
    mln_pixter(px1, exact(f));
    mln_pixter(px2, exact(g));

    mln_forall (px1, px2)
    {
      if (px1->point() != px2->point() or px1->val() != px2->val())
      {
        std::stringstream msg;
        msg << "The following images differs:\n";
        io::imprint(f, msg);
        msg << " and\n:";
        io::imprint(g, msg);

        ASSERT_TRUE(msg.str() == ""); // fail
        return;
      }
    }
  }
}

#define MLN_CHECK_IMEQUAL(f, g) mln::imcheck_equal(f, g);

TEST(Core, SubImage_sub_domain_with_box)
{
  using namespace mln;

  image2d<int> ima(5, 5);

  {
    image2d<int> ref = {
        {0, 1, 2, 3, 4}, {5, 42, 42, 42, 9}, {10, 42, 42, 42, 14}, {15, 42, 42, 42, 19}, {20, 21, 22, 23, 24}};

    iota(ima, 0);
    fill(ima | box2d{{1, 1}, {4, 4}}, 42);
    MLN_CHECK_IMEQUAL(ima, ref);
  }

  {
    image2d<int> ref = {
        {0, 1, 2, 3, 4}, {5, 42, 42, 8, 9}, {10, 11, 12, 13, 14}, {15, 16, 17, 18, 19}, {20, 21, 22, 23, 24}};

    iota(ima, 0);
    fill(ima | box2d{{1, 1}, {4, 4}} | box2d{{1, 1}, {2, 3}}, 42);
    MLN_CHECK_IMEQUAL(ima, ref);
  }

  static_assert(std::is_same<decltype(ima | box2d()), image2d<int>>::value, "");
}

TEST(Core, SubImage_sub_domain)
{
    using namespace mln;

  image2d<int> ima(5, 5);

  {
    image2d<int> ref = {
        {0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}, {10, 42, 42, 42, 42}, {42, 42, 42, 42, 42}, {42, 42, 42, 42, 42}};
    iota(ima, 0);
    fill(ima | where(ima > 10), 42);
    MLN_CHECK_IMEQUAL(ima, ref);
  }

  {
    image2d<int> ref = {
        {0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}, {10, 11, 12, 13, 14}, {15, 16, 17, 18, 19}, {20, 42, 42, 42, 42}};

    iota(ima, 0);
    fill((ima | where(ima > 10)) | where(ima > 20), 42);
    MLN_CHECK_IMEQUAL(ima, ref);
  }

  {
    image2d<int> ref = {
        {0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}, {10, 42, 42, 42, 42}, {42, 42, 42, 42, 42}, {20, 21, 22, 23, 24}};
    iota(ima, 0);
    fill(ima | where(land(ima > 10, ima < 20)), 42);
    MLN_CHECK_IMEQUAL(ima, ref);
  }
}

// This should throw because
// where(ima < 20) is not included in where(ima > 10)
// This test is disabled because Boost Unit Test does not
// support checking for failure (assert)

// FIXME : this test fails to pass (it doesn't throw)

/*
TEST(Core, SubImage_sub_domain_failure)
{
    using namespace mln;

    image2d<int> ima(5, 5);
    iota(ima, 0);
    fill(ima | where(ima > 10), 42);

    {
        iota(ima, 0);
        ASSERT_ANY_THROW(fill((ima | where(ima > 10)) | where(ima < 20), 44));
    }
}
*/