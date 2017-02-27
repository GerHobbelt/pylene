#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/neighborhood/dyn_wneighborhood.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/transform/chamfer_distance_transform.hpp>

#define BOOST_TEST_MODULE Transform
#include <tests/test.hpp>

BOOST_AUTO_TEST_SUITE(chamfer_distance_transform)

BOOST_AUTO_TEST_CASE(chamfer_distance_transform_1)
{
  using namespace mln;

  image2d<bool> f = { {0,0,0,0,0},
                      {0,1,1,1,0},
                      {0,1,1,1,0},
                      {0,1,1,1,0},
                      {0,0,0,0,0} };

  image2d<int> ref = { {0,0,0,0,0},
                       {0,1,1,1,0},
                       {0,1,2,1,0},
                       {0,1,1,1,0},
                       {0,0,0,0,0} };


  auto res = transform::chamfer_distance_transform(f, c4);

  MLN_CHECK_IMEQUAL(res, ref);
}

BOOST_AUTO_TEST_CASE(chamfer_distance_transform_2)
{
  using namespace mln;

  image2d<bool> f = { {1,0,0,0,0,1},
                      {0,1,1,1,1,0},
                      {0,1,1,1,1,0},
                      {0,1,1,1,1,0},
                      {1,0,0,0,0,1} };

  image2d<int> ref = { {1,0,0,0,0,1},
                       {0,1,1,1,1,0},
                       {0,1,2,2,1,0},
                       {0,1,1,1,1,0},
                       {1,0,0,0,0,1}};


  auto res = transform::chamfer_distance_transform(f, c4);

  MLN_CHECK_IMEQUAL(res, ref);
}


// Input and output are the same image
BOOST_AUTO_TEST_CASE(chamfer_distance_transform_3)
{
  using namespace mln;

  image2d<int> f = { {1,0,0,0,0,1},
                     {0,1,1,1,1,0},
                     {0,1,1,1,1,0},
                     {0,1,1,1,1,0},
                     {1,0,0,0,0,1} };

  image2d<int> ref = { {1,0,0,0,0,1},
                       {0,1,1,1,1,0},
                       {0,1,2,2,1,0},
                       {0,1,1,1,1,0},
                       {1,0,0,0,0,1}};


  transform::chamfer_distance_transform(f, c4, f);

  MLN_CHECK_IMEQUAL(f, ref);
}


BOOST_AUTO_TEST_CASE(bg_is_zero_distance_transform)
{
  using namespace mln;

  image2d<int> f = { {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1}
  };

  image2d<int> ref = { {1,1,1,1,1,1,1},
                       {1,2,2,2,2,2,1},
                       {1,2,3,3,3,2,1},
                       {1,2,3,4,3,2,1},
                       {1,2,3,3,3,2,1},
                       {1,2,2,2,2,2,1},
                       {1,1,1,1,1,1,1}
  };

  auto res = transform::chamfer_distance_transform(f, c4);

  MLN_CHECK_IMEQUAL(res, ref);
}

BOOST_AUTO_TEST_CASE(bg_is_one_distance_transform)
{
  using namespace mln;

  image2d<int> f = { {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1}
  };

  int maxv = value_traits<int>::max();

  image2d<int> ref = { {maxv,maxv,maxv,maxv,maxv,maxv,maxv},
                     {maxv,maxv,maxv,maxv,maxv,maxv,maxv},
                     {maxv,maxv,maxv,maxv,maxv,maxv,maxv},
                     {maxv,maxv,maxv,maxv,maxv,maxv,maxv},
                     {maxv,maxv,maxv,maxv,maxv,maxv,maxv},
                     {maxv,maxv,maxv,maxv,maxv,maxv,maxv},
                     {maxv,maxv,maxv,maxv,maxv,maxv,maxv}
  };


  auto res = transform::chamfer_distance_transform(f, c4, true);

  MLN_CHECK_IMEQUAL(res, ref);
}

BOOST_AUTO_TEST_CASE(bg_is_zero_weighted_distance_transform_float)
{
  using namespace mln;

  image2d<int> f = { {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,0,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1}
  };


  image2d<float> ref = { {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
                         {1.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 1.0f},
                         {1.0f, 2.0f, 1.5f, 1.0f, 1.5f, 2.0f, 1.0f},
                         {1.0f, 2.0f, 1.0f, 0.0f, 1.0f, 2.0f, 1.0f},
                         {1.0f, 2.0f, 1.5f, 1.0f, 1.5f, 2.0f, 1.0f},
                         {1.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 1.0f},
                         {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}
  };


  constexpr std::array<point2d, 8> siteset = {{ {-1,-1}, {-1,+0}, {-1,1},
                                               {+0,-1},          {+0,1},
                                                {+1,-1}, {+1,+0}, {+1,1} }};

  constexpr std::array<float, 8> weights = {{ 1.5f, 1.0f, 1.5f,
                                              1.0f,       1.0f,
                                              1.5f, 1.0f, 1.5f }};

  auto nbh = make_dynamic_wneighborhood(siteset, weights, constant_neighborhood_tag ());
  image2d<float> res = transform::chamfer_distance_transform<float>(f, nbh);

  MLN_CHECK_IMEQUAL(res, ref);
}

BOOST_AUTO_TEST_CASE(bg_is_one_weighted_distance_transform_float)
{
  using namespace mln;

  image2d<int> f = { {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,0,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1}
  };


  image2d<float> ref = { {4.5f, 4.0f, 3.5f, 3.0f, 3.5f, 4.0f, 4.5f},
                         {4.0f, 3.0f, 2.5f, 2.0f, 2.5f, 3.0f, 4.0f},
                         {3.5f, 2.5f, 1.5f, 1.0f, 1.5f, 2.5f, 3.5f},
                         {3.0f, 2.0f, 1.0f, 0.0f, 1.0f, 2.0f, 3.0f},
                         {3.5f, 2.5f, 1.5f, 1.0f, 1.5f, 2.5f, 3.5f},
                         {4.0f, 3.0f, 2.5f, 2.0f, 2.5f, 3.0f, 4.0f},
                         {4.5f, 4.0f, 3.5f, 3.0f, 3.5f, 4.0f, 4.5f},
  };


  constexpr std::array<point2d, 8> siteset = {{ {-1,-1}, {-1,+0}, {-1,1},
                                                {+0,-1},          {+0,1},
                                                {+1,-1}, {+1,+0}, {+1,1} }};

  constexpr std::array<float, 8> weights = {{ 1.5f, 1.0f, 1.5f,
                                              1.0f,       1.0f,
                                              1.5f, 1.0f, 1.5f }};

  auto nbh = make_dynamic_wneighborhood(siteset, weights, constant_neighborhood_tag ());
  image2d<float> res = transform::chamfer_distance_transform<float>(f, nbh, true);

  MLN_CHECK_IMEQUAL(res, ref);
}


BOOST_AUTO_TEST_CASE(bg_is_one_weighted_distance_transform_int_approx_5x5)
{
  using namespace mln;

  image2d<int> f = { {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,0,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1},
                     {1,1,1,1,1,1,1}
  };

  constexpr int a = 5, b = 7, c = 11;
  image2d<int> ref = {
    {21, 18, 16, 15, 16, 18, 21},
    {18, 14, 11, 10, 11, 14, 18},
    {16, 11, 7,  5,  7,  11, 16},
    {15, 10, 5,  0,  5,  10, 15},
    {16, 11, 7,  5,  7,  11, 16},
    {18, 14, 11, 10, 11, 14, 18},
    {21, 18, 16, 15, 16, 18, 21}
  };

  constexpr std::array<point2d, 16> siteset = {{
      {-2,-1}, {-2,+1}, {-1,-2}, {-1,-1}, {-1,+0}, {-1,+1}, {-1,+2},{+0,-1},
      {+0,+1}, {+1,-2}, {+1,-1}, {+1,+0}, {+1,+1}, {+1,+2}, {+2,-1},{+2,+1}
    }};


  constexpr std::array<int, 16> weights =  {{
      c, c, c, b, a, b, c, a,
      a, c, b, a, b, c, c, c
    }};

  auto nbh = make_dynamic_wneighborhood(siteset, weights, constant_neighborhood_tag ());
  image2d<int> res = transform::chamfer_distance_transform(f, nbh, true);

  MLN_CHECK_IMEQUAL(res, ref);
}





BOOST_AUTO_TEST_SUITE_END()
