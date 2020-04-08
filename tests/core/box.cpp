#include <mln/core/box.hpp>

#include <mln/core/concept/domains.hpp>
#include <mln/core/rangev3/concepts.hpp>
#include <concepts/concepts.hpp>


#include <gtest/gtest.h>


template <class A, class B>
concept Interoperable = ::concepts::equality_comparable_with<A, B>
  && requires(A& a, const A& ca, const B& b)
{
  { ca.includes(b) } -> bool;
  { ca.intersects(b) } -> bool;
  { a.clip(b) };
};


TEST(Point, ConceptChecking)
{
  static_assert(mln::ranges::MDRange<mln::experimental::box1d>);
  static_assert(mln::ranges::MDRange<mln::experimental::box2d>);
  static_assert(mln::ranges::MDRange<mln::experimental::box3d>);


  static_assert(mln::concepts::Domain<mln::experimental::box1d>);
  static_assert(mln::concepts::Domain<mln::experimental::box2d>);
  static_assert(mln::concepts::Domain<mln::experimental::box3d>);


  static_assert(mln::concepts::Domain<mln::experimental::const_box1d_ref>);
  static_assert(mln::concepts::Domain<mln::experimental::const_box2d_ref>);
  static_assert(mln::concepts::Domain<mln::experimental::const_box3d_ref>);

  static_assert(::concepts::convertible_to<mln::experimental::ndbox<-1>, mln::experimental::ndbox<-1>>); // Dyn -> dyn
  static_assert(::concepts::convertible_to<mln::experimental::ndbox<+1>, mln::experimental::ndbox<+1>>); // Static -> static
  static_assert(::concepts::convertible_to<mln::experimental::ndbox<+1>, mln::experimental::ndbox<-1>>); // Static -> Dyn
  static_assert(::concepts::convertible_to<mln::experimental::ndbox<-1>, mln::experimental::ndbox<+1>>); // Dyn -> Static


  static_assert(Interoperable<mln::experimental::ndbox<-1>, mln::experimental::ndbox<-1>>); // Dyn <-> dyn
  static_assert(Interoperable<mln::experimental::ndbox<+1>, mln::experimental::ndbox<+1>>); // Static <-> static
  static_assert(Interoperable<mln::experimental::ndbox<+1>, mln::experimental::ndbox<-1>>); // Dyn <-> Static

  static_assert(Interoperable<mln::experimental::ndboxref<-1>, mln::experimental::ndboxref<-1>>); // dyn ref <-> dyn ref
  static_assert(Interoperable<mln::experimental::ndboxref<+1>, mln::experimental::ndboxref<+1>>); // static ref <-> static ref
  static_assert(Interoperable<mln::experimental::ndboxref<+1>, mln::experimental::ndboxref<-1>>); // dyn ref <-> static ref


  static_assert(Interoperable<mln::experimental::ndbox<-1>, mln::experimental::ndboxref<-1>>);
  static_assert(Interoperable<mln::experimental::ndbox<+1>, mln::experimental::ndboxref<-1>>);
  static_assert(Interoperable<mln::experimental::ndbox<-1>, mln::experimental::ndboxref<+1>>);
  static_assert(Interoperable<mln::experimental::ndbox<+1>, mln::experimental::ndboxref<+1>>);
}

TEST(Box, dynamic_box_construction)
{
  constexpr mln::experimental::Box b1d(10);
  constexpr mln::experimental::Box b2d(10, 20);
  constexpr mln::experimental::Box b3d(10, 20, 30);
  constexpr mln::experimental::Box b4d(10, 20, 30, 40);

  static_assert(b1d.width() == 10);
  static_assert(b2d.width() == 10);
  static_assert(b3d.width() == 10);
  static_assert(b4d.width() == 10);

  static_assert(b2d.height() == 20);
  static_assert(b3d.height() == 20);
  static_assert(b4d.height() == 20);

  static_assert(b3d.depth() == 30);
  static_assert(b4d.depth() == 30);

  static_assert(b4d.size(3) == 40);


  static_assert(b1d.size() == 10);
  static_assert(b2d.size() == 10*20);
  static_assert(b3d.size() == 10*20*30);
  static_assert(b4d.size() == 10*20*30*40);

  static_assert(b1d.dim() == 1);
  static_assert(b2d.dim() == 2);
  static_assert(b3d.dim() == 3);
  static_assert(b4d.dim() == 4);
}

TEST(Box, static_box_construction)
{
  constexpr mln::experimental::box1d    b1d(10);
  constexpr mln::experimental::box2d    b2d(10, 20);
  constexpr mln::experimental::box3d    b3d(10, 20, 30);
  constexpr mln::experimental::ndbox<4> b4d(10, 20, 30, 40);

  static_assert(b1d.width() == 10);
  static_assert(b2d.width() == 10);
  static_assert(b3d.width() == 10);
  static_assert(b4d.width() == 10);

  static_assert(b2d.height() == 20);
  static_assert(b3d.height() == 20);
  static_assert(b4d.height() == 20);

  static_assert(b3d.depth() == 30);
  static_assert(b4d.depth() == 30);

  static_assert(b4d.size(3) == 40);


  static_assert(b1d.size() == 10);
  static_assert(b2d.size() == 10*20);
  static_assert(b3d.size() == 10*20*30);
  static_assert(b4d.size() == 10*20*30*40);

  static_assert(b1d.dim() == 1);
  static_assert(b2d.dim() == 2);
  static_assert(b3d.dim() == 3);
  static_assert(b4d.dim() == 4);

  { // From 2 points
    constexpr mln::experimental::point2d p1 = {0, 0};
    constexpr mln::experimental::point2d p2 = {10, 20};
    constexpr mln::experimental::box2d   b2d(p1, p2);

    static_assert(b2d.width() == 10);
    static_assert(b2d.height() == 20);
    static_assert(b2d.size() == 10 * 20);
    static_assert(b2d.dim() == 2);
  }
}


TEST(Box, constboxref_construction_from_dynamic_box)
{
  constexpr mln::experimental::Box b1d(10);
  constexpr mln::experimental::Box b2d(10, 20);
  constexpr mln::experimental::Box b3d(10, 20, 30);
  constexpr mln::experimental::Box b4d(10, 20, 30, 40);


  mln::experimental::ConstBoxRef r1d = b1d;
  mln::experimental::ConstBoxRef r2d = b2d;
  mln::experimental::ConstBoxRef r3d = b3d;
  mln::experimental::ConstBoxRef r4d = b4d;

  ASSERT_EQ(r1d.width(), 10);
  ASSERT_EQ(r2d.width(), 10);
  ASSERT_EQ(r3d.width(), 10);
  ASSERT_EQ(r4d.width(), 10);

  ASSERT_EQ(r2d.height(), 20);
  ASSERT_EQ(r3d.height(), 20);
  ASSERT_EQ(r4d.height(), 20);

  ASSERT_EQ(r3d.depth(), 30);
  ASSERT_EQ(r4d.depth(), 30);

  ASSERT_EQ(r4d.size(3), 40);


  ASSERT_EQ(r1d.size(), 10);
  ASSERT_EQ(r2d.size(), 10 * 20);
  ASSERT_EQ(r3d.size(), 10 * 20 * 30);
  ASSERT_EQ(r4d.size(), 10 * 20 * 30 * 40);

  ASSERT_EQ(r1d.dim(), 1);
  ASSERT_EQ(r2d.dim(), 2);
  ASSERT_EQ(r3d.dim(), 3);
  ASSERT_EQ(r4d.dim(), 4);
}

TEST(Box, constboxref_construction_from_static_box)
{
  constexpr mln::experimental::box1d b1d(10);
  constexpr mln::experimental::box2d b2d(10, 20);
  constexpr mln::experimental::box3d b3d(10, 20, 30);
  constexpr mln::experimental::ndbox<4> b4d(10, 20, 30, 40);


  mln::experimental::ConstBoxRef r1d = b1d;
  mln::experimental::ConstBoxRef r2d = b2d;
  mln::experimental::ConstBoxRef r3d = b3d;
  mln::experimental::ConstBoxRef r4d = b4d;

  ASSERT_EQ(r1d.width(), 10);
  ASSERT_EQ(r2d.width(), 10);
  ASSERT_EQ(r3d.width(), 10);
  ASSERT_EQ(r4d.width(), 10);


  ASSERT_EQ(r2d.height(), 20);
  ASSERT_EQ(r3d.height(), 20);
  ASSERT_EQ(r4d.height(), 20);

  ASSERT_EQ(r3d.depth(), 30);
  ASSERT_EQ(r4d.depth(), 30);

  ASSERT_EQ(r4d.size(3), 40);


  ASSERT_EQ(r1d.size(), 10);
  ASSERT_EQ(r2d.size(), 10 * 20);
  ASSERT_EQ(r3d.size(), 10 * 20 * 30);
  ASSERT_EQ(r4d.size(), 10 * 20 * 30 * 40);

  ASSERT_EQ(r1d.dim(), 1);
  ASSERT_EQ(r2d.dim(), 2);
  ASSERT_EQ(r3d.dim(), 3);
  ASSERT_EQ(r4d.dim(), 4);
}

TEST(Box, emptyness)
{
  {
    constexpr mln::experimental::Box b0d;
    constexpr mln::experimental::Box b1d(0);
    constexpr mln::experimental::Box b2d(0, 0);
    constexpr mln::experimental::Box b3d(0, 0, 0);
    constexpr mln::experimental::Box b4d(0, 0, 0, 0);

    static_assert(b0d.empty());
    static_assert(b1d.empty());
    static_assert(b2d.empty());
    static_assert(b3d.empty());
    static_assert(b4d.empty());
  }
  {
    constexpr mln::experimental::box1d  b1d;
    constexpr mln::experimental::box2d  b2d;
    constexpr mln::experimental::box3d  b3d;
    constexpr mln::experimental::ndbox<4> b4d;

    static_assert(b1d.empty());
    static_assert(b2d.empty());
    static_assert(b3d.empty());
    static_assert(b4d.empty());
  }
  {
    constexpr mln::experimental::box2d  ne(10,10);
    static_assert(!ne.empty());
  }
  {
    constexpr mln::experimental::point2d p1 = {-2, 2};
    constexpr mln::experimental::point2d p2 = {2, 3};
    constexpr mln::experimental::box2d   b(p1,p2); // p2.y > p1.y
    static_assert(!b.empty());
  }
}

/******************************************/
/****            Inclusion            ****/
/******************************************/


TEST(Box, inclusion)
{
  {
    constexpr mln::experimental::Box   A(10,10);
    constexpr mln::experimental::Box   B(5,5);
    static_assert(A.includes(B));
  }
  {
    constexpr mln::experimental::Box   A(10,10);
    constexpr mln::experimental::Box   B(10,10);
    static_assert(A.includes(B));
  }
  {
    constexpr mln::experimental::Box   A(10,5);
    constexpr mln::experimental::Box   B(10,10);
    static_assert(!A.includes(B));
  }
}


TEST(box2d, inclusion)
{
  {
    constexpr mln::experimental::box2d   A(10,10);
    constexpr mln::experimental::box2d   B(5,5);
    static_assert(A.includes(B));
  }
  {
    constexpr mln::experimental::box2d   A(10,10);
    constexpr mln::experimental::box2d   B(10,10);
    static_assert(A.includes(B));
  }
  {
    constexpr mln::experimental::box2d   A(10,5);
    constexpr mln::experimental::box2d   B(10,10);
    static_assert(!A.includes(B));
  }
}


/******************************************/
/****            Intersection          ****/
/******************************************/


TEST(box2d, intersection)
{
  {
    constexpr mln::experimental::box2d   A(10,10);
    constexpr mln::experimental::box2d   B(5,5);
    static_assert(A.intersects(B));
  }
  {
    constexpr mln::experimental::point2d p1 = {0, 0};
    constexpr mln::experimental::point2d p2 = {5, 5};
    constexpr mln::experimental::point2d p3 = {5, 5};
    constexpr mln::experimental::point2d p4 = {10, 10};
    constexpr mln::experimental::box2d   A(p1, p2);
    constexpr mln::experimental::box2d   B(p3, p4);
    static_assert(!A.intersects(B));
  }
  {
    constexpr mln::experimental::point2d p1 = {0, 0};
    constexpr mln::experimental::point2d p2 = {5, 5};
    constexpr mln::experimental::point2d p3 = {3, 3};
    constexpr mln::experimental::point2d p4 = {4, 7};
    constexpr mln::experimental::box2d   A(p1, p2);
    constexpr mln::experimental::box2d   B(p3, p4);
    static_assert(A.intersects(B));
  }
}

TEST(Box, intersection)
{
  {
    constexpr mln::experimental::Box   A(10,10);
    constexpr mln::experimental::Box   B(5,5);
    static_assert(A.intersects(B));
  }
  {
    constexpr mln::experimental::point2d p1 = {0, 0};
    constexpr mln::experimental::point2d p2 = {5, 5};
    constexpr mln::experimental::point2d p3 = {5, 5};
    constexpr mln::experimental::point2d p4 = {10, 10};
    constexpr mln::experimental::Box   A(p1, p2);
    constexpr mln::experimental::Box   B(p3, p4);
    static_assert(!A.intersects(B));
  }
  {
    constexpr mln::experimental::point2d p1 = {0, 0};
    constexpr mln::experimental::point2d p2 = {5, 5};
    constexpr mln::experimental::point2d p3 = {3, 3};
    constexpr mln::experimental::point2d p4 = {4, 7};
    constexpr mln::experimental::Box   A(p1, p2);
    constexpr mln::experimental::Box   B(p3, p4);
    static_assert(A.intersects(B));
  }
}

/******************************************/
/****            Clipping              ****/
/******************************************/


TEST(box2d, clipping)
{
  {
    mln::experimental::box2d   A(10,10);
    constexpr mln::experimental::box2d   B(5,5);
    A.clip(B);
    EXPECT_EQ(A, B);
  }
  {
    constexpr mln::experimental::point2d p1 = {0, 0};
    constexpr mln::experimental::point2d p2 = {5, 5};
    constexpr mln::experimental::point2d p3 = {5, 5};
    constexpr mln::experimental::point2d p4 = {10, 10};
    mln::experimental::box2d   A(p1, p2);
    constexpr mln::experimental::box2d   B(p3, p4);
    A.clip(B);
    EXPECT_TRUE(A.empty());
  }
  {
    constexpr mln::experimental::point2d p1 = {0, 0};
    constexpr mln::experimental::point2d p2 = {5, 5};
    constexpr mln::experimental::point2d p3 = {3, 3};
    constexpr mln::experimental::point2d p4 = {4, 7};
    mln::experimental::box2d             A(p1, p2);
    constexpr mln::experimental::box2d   B(p3, p4);
    A.clip(B);

    constexpr mln::experimental::box2d ref({3, 3}, {4, 5});
    EXPECT_EQ(A, ref);
  }
}

TEST(Box, clipping)
{
  {
    mln::experimental::Box   A(10,10);
    constexpr mln::experimental::Box   B(5,5);
    A.clip(B);
    EXPECT_EQ(A, B);
  }
  {
    constexpr mln::experimental::point2d p1 = {0, 0};
    constexpr mln::experimental::point2d p2 = {5, 5};
    constexpr mln::experimental::point2d p3 = {5, 5};
    constexpr mln::experimental::point2d p4 = {10, 10};
    mln::experimental::Box   A(p1, p2);
    constexpr mln::experimental::Box   B(p3, p4);
    A.clip(B);
    EXPECT_TRUE(A.empty());
  }
  {
    constexpr mln::experimental::point2d p1 = {0, 0};
    constexpr mln::experimental::point2d p2 = {5, 5};
    constexpr mln::experimental::point2d p3 = {3, 3};
    constexpr mln::experimental::point2d p4 = {4, 7};
    mln::experimental::Box             A(p1, p2);
    constexpr mln::experimental::Box   B(p3, p4);
    A.clip(B);

    constexpr mln::experimental::Box ref({3, 3}, {4, 5});
    EXPECT_EQ(A, ref);
  }
}

/******************************************/
/****            Interopability        ****/
/******************************************/

template <class A, class B>
void interop_box2d_test()
{
  A a(30, 10);
  A tmp = {{5, 5}, {10, 10}};
  B b   = tmp;

  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);

  EXPECT_TRUE(a.includes(b));
  EXPECT_TRUE(a.intersects(b));
  a.clip(b);
  EXPECT_TRUE(a == b);
}




TEST(box2d, interop)
{
  interop_box2d_test<mln::experimental::ndbox<-1>, mln::experimental::ndbox<-1>>(); // Dyn <-> dyn
  interop_box2d_test<mln::experimental::ndbox<+2>, mln::experimental::ndbox<+2>>(); // Static <-> static
  interop_box2d_test<mln::experimental::ndbox<+2>, mln::experimental::ndbox<-1>>(); // Dyn <-> Static

  interop_box2d_test<mln::experimental::ndbox<-1>, mln::experimental::ndboxref<-1>>();
  interop_box2d_test<mln::experimental::ndbox<+2>, mln::experimental::ndboxref<-1>>();
  interop_box2d_test<mln::experimental::ndbox<-1>, mln::experimental::ndboxref<+2>>();
  interop_box2d_test<mln::experimental::ndbox<+2>, mln::experimental::ndboxref<+2>>();
}

/******************************************/
/****            Inflate               ****/
/******************************************/

TEST(Box, inflate)
{
  {
    mln::experimental::box2d b1(5, 3);
    mln::experimental::Box   b2({-2, -2}, {7, 5});
    b1.inflate(2);
    EXPECT_EQ(b1, b2);
  }
  {
    mln::experimental::box2d b1(5, 3);
    mln::experimental::Box   b2({1, 1}, {4, 2});
    b1.inflate(-1);
    EXPECT_EQ(b1, b2);
  }
  {
    mln::experimental::box2d b1(5, 3);
    b1.inflate(-2);
    EXPECT_TRUE(b1.empty());
  }
}

/******************************************/
/****            Iteration             ****/
/******************************************/


TEST(box2d, iteration_forward)
{
  mln::experimental::box2d b({2, 3}, {6, 8});
  int x = 2, y = 3;
  for (auto&& r : b.rows())
    for (auto p : r)
    {
      ASSERT_EQ(p[0], x);
      ASSERT_EQ(p[1], y);
      ASSERT_TRUE(b.has(p));
      if (++x == 6)
      {
        x = 2;
        ++y;
      }
    }
}

TEST(box2d, iteration_backward)
{
  mln::experimental::box2d b({2, 3}, {6, 8});

  int x = 5, y = 7;
  for (auto&& r : b.rrows())
    for (auto p : r)
  {
    ASSERT_EQ(p[0], x);
    ASSERT_EQ(p[1], y);
    ASSERT_TRUE(b.has(p));
    if (x-- == 2)
    {
      x = 5;
      --y;
    }
  }
}
