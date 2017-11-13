#include <mln/core/algorithm/iota.hpp>
#include <mln/core/algorithm/sort_indexes.hpp>
#include <mln/core/image/image2d.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Sort_indexes_fast)
{
    using namespace mln;

    image2d<uint8> ima(5, 5);
    iota(ima, 0);

    typedef typename image2d<uint8>::size_type size_type;
    std::vector<size_type> offset = sort_indexes(ima);
    for (auto x : offset)
        std::cout << x << std::endl;
}
