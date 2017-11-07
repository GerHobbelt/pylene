#include <mln/core/image/image2d.hpp>
#include <numeric>
#define BOOST_TEST_MODULE Core
#include <tests/test.hpp>

using namespace mln;

struct iterator_test
{
  image2d<int> ima;
  std::vector<int> values;
  std::vector<int> indexes;

  void test_forward_viter()
  {
    auto&& rng = ima.values();
    auto&& it = rng.iter();
    it.init();
    for (unsigned i = 0; i < values.size(); ++i)
      {
        BOOST_CHECK_EQUAL(*it, values[i]);
        it.next();
      }
    BOOST_CHECK(it.finished());
  }

  void test_backward_viter()
  {
    auto&& rng = ima.values();
    auto&& it = rng.riter();
    it.init();
    for (int i = static_cast<int>(values.size()) - 1; i >= 0; --i)
      {
        BOOST_CHECK_EQUAL(*it, values[i]);
        it.next();
      }
    BOOST_CHECK(it.finished());
  }

  void test_forward_pixter()
  {
    auto&& rng = ima.pixels();
    auto&& it = rng.iter();
    auto pit = ima.domain().iter();
    it.init();
    pit.init();
    for (unsigned i = 0; i < values.size(); ++i)
      {
        BOOST_CHECK_EQUAL(it->point(), *pit);
        BOOST_CHECK_EQUAL(it->val(), values[i]);
        BOOST_CHECK_EQUAL(it->index(), indexes[i]);
        it.next();
        pit.next();
      }
    BOOST_CHECK(it.finished());
  }

  void test_backward_pixter()
  {
    auto&& rng = ima.pixels();
    auto&& it = rng.riter();
    auto pit = ima.domain().riter();
    it.init();
    pit.init();
    for (int i = static_cast<int>(values.size()) - 1; i >= 0; --i)
      {
        BOOST_CHECK_EQUAL(it->point(), *pit);
        BOOST_CHECK_EQUAL(it->val(), values[i]);
        BOOST_CHECK_EQUAL(it->index(), indexes[i]);
        it.next();
        pit.next();
      }
    BOOST_CHECK(it.finished());
  }
};


/******************************************/
/****           Empty Domain           ****/
/******************************************/

struct iterator_test_empty_domain : iterator_test
{
};

BOOST_FIXTURE_TEST_SUITE(empty_domain, iterator_test_empty_domain)

BOOST_AUTO_TEST_CASE(forward_viter)
{
  this->test_forward_viter();
}

BOOST_AUTO_TEST_CASE(backward_viter)
{
  this->test_backward_viter();
}

BOOST_AUTO_TEST_CASE(forward_pixter)
{
  this->test_forward_pixter();
}

BOOST_AUTO_TEST_CASE(backward_pixter)
{
  this->test_backward_pixter();
}

BOOST_AUTO_TEST_SUITE_END()

/******************************************/
/****         Origin at (0,0)         ****/
/******************************************/

struct iterator_test_zero_origin_domain : iterator_test
{
  iterator_test_zero_origin_domain()
  {
    ima = image2d<int> {
      {0,1,2,3},
      {4,5,6,7},
      {8,9,10,11}
    };

    indexes = {
      33, 34, 35, 36,
      43, 44, 45, 46,
      53, 54, 55, 56
    };
    values.resize(12);
    std::iota(values.begin(), values.end(), 0);
  }
};


BOOST_FIXTURE_TEST_SUITE(zero_origin_domain, iterator_test_zero_origin_domain)

BOOST_AUTO_TEST_CASE(forward_viter)
{
  this->test_forward_viter();
}

BOOST_AUTO_TEST_CASE(backward_viter)
{
  this->test_backward_viter();
}

BOOST_AUTO_TEST_CASE(forward_pixter)
{
  this->test_forward_pixter();
}

BOOST_AUTO_TEST_CASE(backward_pixter)
{
  this->test_backward_pixter();
}

BOOST_AUTO_TEST_SUITE_END()
