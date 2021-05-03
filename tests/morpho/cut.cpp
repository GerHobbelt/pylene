#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/cut.hpp>

#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/image/ndimage.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>

#include <mln/io/imprint.hpp>

TEST(Morpho, AlphaTreeCutMeanLabelization)
{
    mln::image2d<std::uint8_t> ima = {
        {1, 1, 5},
        {2, 5, 6},
        {0, 1, 4}
    };

    mln::image2d<std::uint8_t> cut_1 = {
        {1, 1, 5},
        {1, 5, 5},
        {0, 0, 4}
    };

    mln::image2d<std::uint8_t> cut_2 = {
        {1, 1, 5},
        {1, 5, 5},
        {1, 1, 5}
    };

    mln::image2d<std::uint8_t> cut_3 = {
        {2, 2, 2},
        {2, 2, 2},
        {2, 2, 2}
    };

    auto [t, nm] = mln::morpho::alphatree(ima, mln::c4);
    auto val = t.compute_attribute_on_values(nm, ima, mln::accu::accumulators::mean<std::uint8_t, std::uint8_t>());

    ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 0, val), ima);
    ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 1, val), cut_1);
    ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 2, val), cut_2);
    ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 3, val), cut_3);
}

TEST(Morpho, AlphaTreeCutRangeLabelization)
{
    mln::image2d<std::uint8_t> ima = {
        {1, 1, 5},
        {2, 5, 6},
        {0, 1, 4}
    };

    mln::image2d<std::uint8_t> cut_1 = {
        {1, 1, 5},
        {1, 5, 5},
        {0, 0, 4}
    };

    mln::image2d<std::uint8_t> cut_2 = {
        {1, 1, 5},
        {1, 5, 5},
        {1, 1, 5}
    };

    mln::image2d<std::uint8_t> cut_3 = {
        {2, 2, 2},
        {2, 2, 2},
        {2, 2, 2}
    };

    auto [t, nm] = mln::morpho::alphatree(ima, mln::c4);
    auto val = t.compute_attribute_on_values(nm, ima, mln::accu::accumulators::mean<std::uint8_t, std::uint8_t>());

    ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 0, val), ima);
    ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 1, val), cut_1);
    ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 2, val), cut_2);
    ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 3, val), cut_3);
}