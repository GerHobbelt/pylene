#include <mln/colors/lsh.hpp>

#include <gtest/gtest.h>

TEST(Colors, LSH)
{
    using namespace mln;

    typedef vec3i V;
    {
        rgb8 v = {0, 0, 0};
        auto res = lsh2rgb(rgb2lsh(v));
        ASSERT_EQ((V)v, (V)res);
    }

    {
        rgb8 v = {255, 255, 255};
        auto res = lsh2rgb(rgb2lsh(v));
        ASSERT_EQ((V)v, (V)res);
    }

    /* FIXME: rounding problems
    {
      rgb8 v = {255,128,0};
      ASSERT_EQ( (V) v,  (V) lsh2rgb(rgb2lsh(v)));
    }
    */

    {
        lsh8 v = {0, 0, 128}; // Hue not significant
        ASSERT_EQ((int)v[0], (int)rgb2lsh(lsh2rgb(v))[0]);
    }

    {
        lsh8 v = {255, 0, 128}; // Hue and sat not significant
        ASSERT_EQ((int)v[0], (int)rgb2lsh(lsh2rgb(v))[0]);
    }

    /* FIXME: rounding problems
    {
      lsh8 v = {128,128,251}; // Hue on 252 values
      ASSERT_EQ( (V) v,  (V) rgb2lsh(lsh2rgb(v)));
    }
    */
}
