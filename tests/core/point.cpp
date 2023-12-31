#include <mln/core/point.hpp>

#include <mln/core/concepts/point.hpp>

#include <gtest/gtest.h>

#include <concepts>

template <typename U, typename V = U>
concept AddableWith = requires(U a, V b)
{
#if __GNUC__ == 9
  { a += b } -> U&;
  { a -= b } -> U&;
  { b += a } -> V&;
  { b -= a } -> V&;
#else
  { a += b } -> std::same_as<U&>;
  { a -= b } -> std::same_as<U&>;
  { b += a } -> std::same_as<V&>;
  { b -= a } -> std::same_as<V&>;
#endif
  { a + b };
  { a - b };
  { b + a};
  { b - a};
};


template <typename U, typename V>
concept Interopable = AddableWith<U, V> && std::totally_ordered_with<U, V>; // Implies EqualityComparable<U, V>


TEST(Point, ConceptChecking)
{
  static_assert(mln::concepts::Point<mln::point1d>);
  static_assert(mln::concepts::Point<mln::point2d>);
  static_assert(mln::concepts::Point<mln::point3d>);
  static_assert(mln::concepts::Point<mln::Point>);

  static_assert(mln::concepts::Point<mln::point1d_ref>);
  static_assert(mln::concepts::Point<mln::point2d_ref>);
  static_assert(mln::concepts::Point<mln::point3d_ref>);
  static_assert(mln::concepts::Point<mln::PointRef>);
  static_assert(mln::concepts::Point<mln::ConstPointRef>);

  // Static to * conversion
  static_assert(std::convertible_to<mln::ndpoint<+1, short>, mln::ndpoint<+1, int>>);
  static_assert(std::convertible_to<mln::ndpoint<+1, short>, mln::ndpoint<-1, int>>);
  static_assert(std::convertible_to<mln::ndpoint<+1, int>&, mln::ndpointref<-1, int>>);
  static_assert(std::convertible_to<mln::ndpoint<+1, int>&, mln::ndpointref<+1, int>>);
  static_assert(std::convertible_to<const mln::ndpoint<+1, int>&, mln::ndpointref<-1, const int>>);
  static_assert(std::convertible_to<const mln::ndpoint<+1, int>&, mln::ndpointref<+1, const int>>);

  // Dynamic to * conversion
  static_assert(std::convertible_to<mln::ndpoint<-1, short>, mln::ndpoint<+1, int>>);
  static_assert(std::convertible_to<mln::ndpoint<-1, short>, mln::ndpoint<-1, int>>);
  static_assert(std::convertible_to<mln::ndpoint<-1, int>&, mln::ndpointref<-1, int>>);
  static_assert(std::convertible_to<mln::ndpoint<-1, int>&, mln::ndpointref<+1, int>>);
  static_assert(std::convertible_to<const mln::ndpoint<-1, int>&, mln::ndpointref<-1, const int>>);
  static_assert(std::convertible_to<const mln::ndpoint<-1, int>&, mln::ndpointref<+1, const int>>);

  // Static Ref to * conversion
  static_assert(std::convertible_to<mln::ndpointref<+1, short>, mln::ndpoint<+1, int>>);
  static_assert(std::convertible_to<mln::ndpointref<+1, short>, mln::ndpoint<-1, int>>);
  static_assert(std::convertible_to<mln::ndpointref<+1, short>, mln::ndpointref<-1, const short>>);
  static_assert(std::convertible_to<mln::ndpointref<+1, short>, mln::ndpointref<+1, short>>);

  // Dynamic Ref to * conversion
  static_assert(std::convertible_to<mln::ndpointref<-1, short>, mln::ndpoint<+1, int>>);
  static_assert(std::convertible_to<mln::ndpointref<-1, short>, mln::ndpoint<-1, int>>);
  static_assert(std::convertible_to<mln::ndpointref<-1, short>, mln::ndpointref<-1, short>>);
  static_assert(std::convertible_to<mln::ndpointref<-1, short>, mln::ndpointref<+1, const short>>);



  /*
  static_assert(mln::concepts::stl::ConvertibleTo<mln::ndpoint<-1, short>, mln::ndpoint<-1, int>>); // Dyn -> dyn
  static_assert(mln::concepts::stl::ConvertibleTo<mln::ndpoint<+1, short>, mln::ndpoint<+1, int>>); // Static -> static
  static_assert(mln::concepts::stl::ConvertibleTo<mln::ndpoint<+1, short>, mln::ndpoint<-1, int>>); // Static -> Dyn
  static_assert(mln::concepts::stl::ConvertibleTo<mln::ndpoint<-1, short>, mln::ndpoint<+1, int>>); // Dyn -> Static
  */



  static_assert(Interopable<mln::ndpoint<-1, short>, mln::ndpoint<-1, int>>); // Dyn <-> dyn
  static_assert(Interopable<mln::ndpoint<+1, short>, mln::ndpoint<+1, int>>); // Static <-> static
  static_assert(Interopable<mln::ndpoint<+1, short>, mln::ndpoint<-1, int>>); // Dyn <-> Static

  static_assert(Interopable<mln::ndpointref<-1, short>, mln::ndpointref<-1, int>>); // dyn ref <-> dyn ref
  static_assert(Interopable<mln::ndpointref<+1, short>, mln::ndpointref<+1, int>>); // static ref <-> static ref
  static_assert(Interopable<mln::ndpointref<+1, short>, mln::ndpointref<-1, int>>); // dyn ref <-> static ref


  static_assert(Interopable<mln::ndpoint<-1, short>, mln::ndpointref<-1, int>>);
  static_assert(Interopable<mln::ndpoint<+1, short>, mln::ndpointref<-1, int>>);
  static_assert(Interopable<mln::ndpoint<-1, short>, mln::ndpointref<+1, int>>);
  static_assert(Interopable<mln::ndpoint<+1, short>, mln::ndpointref<+1, int>>);

}


TEST(DynamicPoint, Constructors)
{
  {
    mln::Point p = {}; // 0-init
    ASSERT_EQ(p.dim(), 0);
  }

  {
    mln::Point p(2); // Create a 2d-point with undefined values
    ASSERT_EQ(p.dim(), 2);
  }

  {
    mln::Point p = {2}; // Create a 1d-point with value 2
    ASSERT_EQ(p.dim(), 1);
    ASSERT_EQ(p[0], 2);
  }
}

TEST(Point, constexpr_points)
{
  {
    constexpr mln::Point p1d = {1};
    constexpr mln::Point p2d = {1, 2};
    constexpr mln::Point p3d = {1, 2, 3};

    static_assert(p1d[0] == 1 && p1d.x() == 1);
    static_assert(p2d[0] == 1 && p2d.x() == 1);
    static_assert(p3d[0] == 1 && p3d.x() == 1);
    static_assert(p2d[1] == 2 && p2d.y() == 2);
    static_assert(p3d[1] == 2 && p3d.y() == 2);
    static_assert(p3d[2] == 3 && p3d.z() == 3);
    static_assert(p1d.dim() == 1);
    static_assert(p2d.dim() == 2);
    static_assert(p3d.dim() == 3);
  }
  {
    constexpr mln::point1d p1d = {1};
    constexpr mln::point2d p2d = {1, 2};
    constexpr mln::point3d p3d = {1, 2, 3};

    static_assert(p1d[0] == 1 && p1d.x() == 1);
    static_assert(p2d[0] == 1 && p2d.x() == 1);
    static_assert(p3d[0] == 1 && p3d.x() == 1);
    static_assert(p2d[1] == 2 && p2d.y() == 2);
    static_assert(p3d[1] == 2 && p3d.y() == 2);
    static_assert(p3d[2] == 3 && p3d.z() == 3);
    static_assert(p1d.dim() == 1);
    static_assert(p2d.dim() == 2);
    static_assert(p3d.dim() == 3);
  }
}

template <class P1A, class P2A, class P3A, class P1B, class P2B, class P3B>
void test_interop()
{
  P1A p1d = {1};
  P2A p2d = {1, 2};
  P3A p3d = {1, 2, 3};

  ASSERT_EQ(p1d[0], 1);     ASSERT_EQ(p1d.x(), 1);
  ASSERT_EQ(p2d[0], 1);     ASSERT_EQ(p2d.x(), 1);
  ASSERT_EQ(p3d[0], 1);     ASSERT_EQ(p3d.x(), 1);
  ASSERT_EQ(p2d[1], 2);     ASSERT_EQ(p2d.y(), 2);
  ASSERT_EQ(p3d[1], 2);     ASSERT_EQ(p3d.y(), 2);
  ASSERT_EQ(p3d[2], 3);     ASSERT_EQ(p3d.z(), 3);

  ASSERT_EQ(p1d.dim(), 1);
  ASSERT_EQ(p2d.dim(), 2);
  ASSERT_EQ(p3d.dim(), 3);

  // 1. Conversion
  P1B r1d = p1d;
  P2B r2d = p2d;
  P3B r3d = p3d;
  {

    ASSERT_EQ(r1d[0], 1);     ASSERT_EQ(r1d.x(), 1);
    ASSERT_EQ(r2d[0], 1);     ASSERT_EQ(r2d.x(), 1);
    ASSERT_EQ(r3d[0], 1);     ASSERT_EQ(r3d.x(), 1);
    ASSERT_EQ(r2d[1], 2);     ASSERT_EQ(r2d.y(), 2);
    ASSERT_EQ(r3d[1], 2);     ASSERT_EQ(r3d.y(), 2);
    ASSERT_EQ(r3d[2], 3);     ASSERT_EQ(r3d.z(), 3);

    ASSERT_EQ(r1d.dim(), 1);
    ASSERT_EQ(r2d.dim(), 2);
    ASSERT_EQ(r3d.dim(), 3);
  }

  // 2. Arithmetic Ops
  {
    // Addition
    {
      auto r1 = p1d + r1d;
      auto r2 = p2d + r2d;
      auto r3 = p3d + r3d;
      ASSERT_EQ(r1, mln::Point({2}));
      ASSERT_EQ(r2, mln::Point({2,4}));
      ASSERT_EQ(r3, mln::Point({2,4,6}));
    }
    // Soustraction
    {
      auto r1 = p1d - r1d;
      auto r2 = p2d - r2d;
      auto r3 = p3d - r3d;
      ASSERT_EQ(r1, mln::Point({0}));
      ASSERT_EQ(r2, mln::Point({0,0}));
      ASSERT_EQ(r3, mln::Point({0,0,0}));
    }
  }
}


TEST(Point, interopability)
{
  test_interop<mln::ndpoint<+1, int>,
               mln::ndpoint<+2, int>,
               mln::ndpoint<+3, int>,
               mln::ndpoint<+1, long>,
               mln::ndpoint<+2, long>,
               mln::ndpoint<+3, long>>();

  test_interop<mln::ndpoint<+1, int>,
               mln::ndpoint<+2, int>,
               mln::ndpoint<+3, int>,
               mln::ndpoint<-1, int>,
               mln::ndpoint<-1, int>,
               mln::ndpoint<-1, int>>();

  test_interop<mln::ndpoint<+1, int>,
               mln::ndpoint<+2, int>,
               mln::ndpoint<+3, int>,
               mln::ndpointref<+1, const int>,
               mln::ndpointref<+2, const int>,
               mln::ndpointref<+3, const int>>();

  test_interop<mln::ndpoint<-1, int>,
               mln::ndpoint<-1, int>,
               mln::ndpoint<-1, int>,
               mln::ndpointref<+1, const int>,
               mln::ndpointref<+2, const int>,
               mln::ndpointref<+3, const int>>();


  test_interop<mln::ndpoint<+1, int>,
               mln::ndpoint<+2, int>,
               mln::ndpoint<+3, int>,
               mln::ndpointref<-1, const int>,
               mln::ndpointref<-1, const int>,
               mln::ndpointref<-1, const int>>();

  test_interop<mln::ndpoint<-1, int>,
               mln::ndpoint<-1, int>,
               mln::ndpoint<-1, int>,
               mln::ndpointref<-1, const int>,
               mln::ndpointref<-1, const int>,
               mln::ndpointref<-1, const int>>();

}

