#include <mln/core/image/image2d.hpp>

#include <numeric>
#include <vector>

#include <gtest/gtest.h>

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
      ASSERT_EQ(*it, values[i]);
      it.next();
    }
    ASSERT_TRUE(it.finished());
  }

  void test_backward_viter()
  {
    auto&& rng = ima.values();
    auto&& it = rng.riter();
    it.init();
    for (int i = static_cast<int>(values.size()) - 1; i >= 0; --i)
    {
      ASSERT_EQ(*it, values[i]);
      it.next();
    }
    ASSERT_TRUE(it.finished());
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
      ASSERT_EQ(it->point(), *pit);
      ASSERT_EQ(it->val(), values[i]);
      ASSERT_EQ(it->index(), indexes[i]);
      it.next();
      pit.next();
    }
    ASSERT_TRUE(it.finished());
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
      ASSERT_EQ(it->point(), *pit);
      ASSERT_EQ(it->val(), values[i]);
      ASSERT_EQ(it->index(), indexes[i]);
      it.next();
      pit.next();
    }
    ASSERT_TRUE(it.finished());
  }
};

/******************************************/
/****           Empty Domain           ****/
/******************************************/

struct iterator_test_empty_domain : iterator_test
{
};

TEST(Core, Image2d_empty_domain_iterator_test_forward_viter)
{
  iterator_test_empty_domain it{};
  it.test_forward_viter();
}

TEST(Core, Image2d_empty_domain_iterator_test_backward_viter)
{
  iterator_test_empty_domain it{};
  it.test_backward_viter();
}

TEST(Core, Image2d_empty_domain_iterator_test_forward_pixter)
{
  iterator_test_empty_domain it{};
  it.test_forward_pixter();
}

TEST(Core, Image2d_empty_domain_iterator_test_backward_pixter)
{
  iterator_test_empty_domain it{};
  it.test_backward_pixter();
}

/******************************************/
/****         Origin at (0,0)         ****/
/******************************************/

struct iterator_test_zero_origin_domain : iterator_test
{
  iterator_test_zero_origin_domain()
  {
    ima = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};
    indexes = {33, 34, 35, 36, 43, 44, 45, 46, 53, 54, 55, 56};
    values.resize(12);
    std::iota(values.begin(), values.end(), 0);
  }
};

TEST(Core, Image2d_iterator_test_zero_origin_domain_forward_viter)
{
  iterator_test_zero_origin_domain it{};
  it.test_forward_viter();
}

TEST(Core, Image2d_iterator_test_zero_origin_domain_backward_viter)
{
  iterator_test_zero_origin_domain it{};
  it.test_backward_viter();
}

TEST(Core, Image2d_iterator_test_zero_origin_domain_forward_pixter)
{
  iterator_test_zero_origin_domain it{};
  it.test_forward_pixter();
}

TEST(Core, Image2d_iterator_test_zero_origin_domain_backward_pixter)
{
  iterator_test_zero_origin_domain it{};
  it.test_backward_pixter();
}
