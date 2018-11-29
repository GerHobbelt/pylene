#include <mln/core/image/image2d.hpp>
#include <mln/core/rangev3/foreach.hpp>
#include <numeric>
#include <vector>

#include <gtest/gtest.h>

using namespace mln;

struct iterator_test
{
  image2d<int>          ima;
  std::vector<int>      values;
  std::vector<unsigned> indexes;

  void test_forward_viter()
  {
    auto&& rng = ima.values();
    auto&& it  = rng.iter();
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
    auto&& it  = rng.riter();
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
    auto&& it  = rng.iter();
    auto   pit = ima.domain().iter();
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
    auto&& it  = rng.riter();
    auto   pit = ima.domain().riter();
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
  iterator_test_empty_domain it;
  it.test_forward_viter();
}

TEST(Core, Image2d_empty_domain_iterator_test_backward_viter)
{
  iterator_test_empty_domain it;
  it.test_backward_viter();
}

TEST(Core, Image2d_empty_domain_iterator_test_forward_pixter)
{
  iterator_test_empty_domain it;
  it.test_forward_pixter();
}

TEST(Core, Image2d_empty_domain_iterator_test_backward_pixter)
{
  iterator_test_empty_domain it;
  it.test_backward_pixter();
}

/******************************************/
/****         Origin at (0,0)         ****/
/******************************************/

struct iterator_test_zero_origin_domain : iterator_test
{
  iterator_test_zero_origin_domain()
  {
    ima = {
        {0, 1, 2, 3},  //
        {4, 5, 6, 7},  //
        {8, 9, 10, 11} //
    };
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


//// NEW

struct new_iterator_test
{
  image2d<int>     ima;
  std::vector<int> values;

  void test_forward_value_range()
  {
    auto rng = std::as_const(ima).new_values();
    auto it  = values.begin();
    mln_foreach_new (int v, rng)
      ASSERT_EQ(*it++, v);
    ASSERT_EQ(values.end(), it);
  }

  void test_backward_value_range()
  {
    auto rng = std::as_const(ima).new_values();
    auto it  = values.rbegin();
    mln_foreach_new (int v, rng.reversed())
      ASSERT_EQ(*it++, v);
    ASSERT_EQ(values.rend(), it);
  }


  void test_forward_pixel_range()
  {
    auto rng = std::as_const(ima).new_pixels();
    auto pit = ima.domain().iter();

    // Row-wise
    {
      auto vit = values.begin();
      pit.init();
      mln_foreach_new (auto&& px, rng)
      {
        ASSERT_EQ(*vit++, px.val());
        ASSERT_EQ(*pit, px.point());
        pit.next();
      }
      ASSERT_EQ(values.end(), vit);
      ASSERT_TRUE(pit.finished());
    }
    // Range
    {
      auto vit = values.begin();
      pit.init();
      mln_foreach_new (auto&& px, rng)
      {
        ASSERT_EQ(*vit++, px.val());
        ASSERT_EQ(*pit, px.point());
        pit.next();
      }
      ASSERT_EQ(values.end(), vit);
      ASSERT_TRUE(pit.finished());
    }
  }

  void test_backward_pixel_range()
  {
    auto rng = std::as_const(ima).new_pixels();
    auto pit = ima.domain().riter();

    // Std Range
    {
      auto vit = values.rbegin();
      pit.init();
      for (auto&& px : rng.reversed())
      {
        ASSERT_EQ(*vit++, px.val());
        ASSERT_EQ(*pit, px.point());
        pit.next();
      }
      ASSERT_EQ(values.rend(), vit);
      ASSERT_TRUE(pit.finished());
    }
    // Row-wise Range
    {
      auto vit = values.rbegin();
      pit.init();
      mln_foreach_new (auto&& px, rng.reversed())
      {
        ASSERT_EQ(*vit++, px.val());
        ASSERT_EQ(*pit, px.point());
        pit.next();
      }
      ASSERT_EQ(values.rend(), vit);
      ASSERT_TRUE(pit.finished());
    }
  }
};

/******************************************/
/****           Empty Domain           ****/
/******************************************/

struct new_iterator_test_empty_domain : new_iterator_test
{
};

TEST(Core, Image2d_empty_domain_new_iterator_test_forward_viter)
{
  new_iterator_test_empty_domain it;
  it.test_forward_value_range();
}

TEST(Core, Image2d_empty_domain_new_iterator_test_backward_viter)
{
  new_iterator_test_empty_domain it;
  it.test_backward_value_range();
}

TEST(Core, Image2d_empty_domain_new_iterator_test_forward_pixter)
{
  new_iterator_test_empty_domain it;
  it.test_forward_pixel_range();
}

TEST(Core, Image2d_empty_domain_new_iterator_test_backward_pixter)
{
  new_iterator_test_empty_domain it;
  it.test_backward_pixel_range();
}

/******************************************/
/****         Origin at (0,0)         ****/
/******************************************/

struct new_iterator_test_zero_origin_domain : new_iterator_test
{
  new_iterator_test_zero_origin_domain()
  {
    ima = {
        {0, 1, 2, 3},  //
        {4, 5, 6, 7},  //
        {8, 9, 10, 11} //
    };
    values.resize(12);
    std::iota(values.begin(), values.end(), 0);
  }
};

TEST(Core, Image2d_new_iterator_test_zero_origin_domain_forward_viter)
{
  new_iterator_test_zero_origin_domain it{};
  it.test_forward_value_range();
}

TEST(Core, Image2d_new_iterator_test_zero_origin_domain_backward_viter)
{
  new_iterator_test_zero_origin_domain it{};
  it.test_backward_value_range();
}

TEST(Core, Image2d_new_iterator_test_zero_origin_domain_forward_pixter)
{
  new_iterator_test_zero_origin_domain it{};
  it.test_forward_pixel_range();
}


TEST(Core, Image2d_new_iterator_test_zero_origin_domain_backward_pixter)
{
  new_iterator_test_zero_origin_domain it{};
  it.test_backward_pixel_range();
}
