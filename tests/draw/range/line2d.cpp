#include <mln/core/foreach.hpp>
#include <mln/draw/range/line2d.hpp>

#include <gtest/gtest.h>

TEST(Draw, line2d)
{
    using namespace mln;
    {
        auto r = draw::line2d({0, 0}, {4, 11});
        unsigned i = 0;
        mln_foreach (point2d p, r)
        {
            std::cout << p;
            ASSERT_TRUE(i++ < 50);
        }
        std::cout << std::endl;
    }

    // 8 octhans
    {
        auto r = draw::line2d({0, 0}, {4, -11});
        mln_foreach (point2d p, r)
            std::cout << p;
        std::cout << std::endl;
    }

    {
        auto r = draw::line2d({0, 0}, {-4, -11});
        mln_foreach (point2d p, r)
            std::cout << p;
        std::cout << std::endl;
    }

    {
        auto r = draw::line2d({0, 0}, {-4, 11});
        mln_foreach (point2d p, r)
            std::cout << p;
        std::cout << std::endl;
    }

    {
        auto r = draw::line2d({0, 0}, {11, 4});
        mln_foreach (point2d p, r)
            std::cout << p;
        std::cout << std::endl;
    }

    {
        auto r = draw::line2d({0, 0}, {-11, 4});
        mln_foreach (point2d p, r)
            std::cout << p;
        std::cout << std::endl;
    }

    {
        auto r = draw::line2d({0, 0}, {-11, -4});
        mln_foreach (point2d p, r)
            std::cout << p;
        std::cout << std::endl;
    }

    {
        auto r = draw::line2d({0, 0}, {11, -4});
        mln_foreach (point2d p, r)
            std::cout << p;
        std::cout << std::endl;
    }

    // Vertical
    {
        auto r = draw::line2d({0, 0}, {-11, 0});
        mln_foreach (point2d p, r)
            std::cout << p;
        std::cout << std::endl;
    }

    // Horizontal
    {
        auto r = draw::line2d({0, 0}, {0, 11});
        mln_foreach (point2d p, r)
            std::cout << p;
        std::cout << std::endl;
    }
}
