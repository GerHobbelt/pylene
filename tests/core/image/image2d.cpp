#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/extension/mirror.hpp>
#include <mln/io/imprint.hpp>

#include <numeric>

#include <helpers.hpp>

#include <gtest/gtest.h>

TEST(UTImage2D, Extension_Fill)
{
  using namespace mln;

  image2d<int> ima(5, 5);
  image2d<int> ref(5, 5);

  iota(ima, 1);
  iota(ref, 1);

  mln::extension::fill(ima, 69);
  ASSERT_EQ(ima.at(-1, -1), 69);

  mln_pixter(p1, p2, ima, ref);
  mln_iter(q1, c4(p1));
  mln_iter(q2, c4(p2));

  int i = 1;
  mln_forall (p1, p2)
  {
    ASSERT_EQ(p1->val(), p2->val());
    ASSERT_EQ(p1->val(), i++);
    mln_forall (q1, q2)
    {
      if (ima.domain().has(q1->point()))
        ASSERT_EQ(q1->val(), q2->val());
      else
        ASSERT_EQ(q1->val(), 69);
    }
  }
}

TEST(UTImage2D, Extension_mirror)
{
  using namespace mln;

  image2d<int> ima(5, 5);
  iota(ima, 1);

  mln::extension::mirror(ima);
  box2d dom = {{-3, -3}, {8, 8}};

  // MIRROR f(p) = f(n - |p % 2n - n|
  {
    mln_foreach (point2d p, dom)
    {
      point2d q = p % 10;
      q[0] = q[0] < 0 ? q[0] + 10 : q[0];
      q[1] = q[1] < 0 ? q[1] + 10 : q[1]; // because c++ modulo rounds toward 0 (also for neg)
      q[0] = q[0] < 5 ? q[0] : 9 - q[0];
      q[1] = q[1] < 5 ? q[1] : 9 - q[1];

      ASSERT_TRUE(ima.at(p) == ima.at(q));
    }
  }
}

TEST(UTImage2D, From_Buffer)
{
  using namespace mln;
  int buffer[12];
  std::iota(buffer, buffer + 12, 1);

  image2d<int> ima = image2d<int>::from_buffer(buffer, box2d{{0, 0}, {3, 4}});
  image2d<int> ref(3, 4);

  iota(ref, 1);

  ASSERT_IMAGES_EQ(ima, ref);
}

TEST(UTImage2D, inflate_domain)
{
  const int border = 3;
  const mln::box2d original_domain = {{0,0}, {10,10}};
  const mln::box2d inflated_domain = {{-2,-2}, {12, 12}};

  const int inside_value = 42;
  const int outside_value = -42;

  mln::image2d<int> f(original_domain, border, inside_value);
  mln::image2d<int> ref(inflated_domain, border, outside_value);
  mln::extension::fill(f, outside_value);
  mln::extension::fill(ref, outside_value);
  mln::fill(ref | original_domain, inside_value);

  f.inflate_domain(2);
  ASSERT_TRUE(ref.domain() == f.domain());
  ASSERT_EQ(1, f.border());
  ASSERT_IMAGES_EQ(ref, f);
}

TEST(UTImage2D, deflated_domain)
{
  const int border = 3;
  const mln::box2d original_domain = {{0,0}, {10,10}};
  const mln::box2d inflated_domain = {{2,2}, {8, 8}};

  mln::image2d<int> f(original_domain, border);
  mln::iota(f, 0);
  auto ref = f | inflated_domain;

  f.inflate_domain(-2);
  ASSERT_TRUE(ref.domain() == f.domain());
  ASSERT_EQ(5, f.border());
  ASSERT_IMAGES_EQ(ref, f);
}
