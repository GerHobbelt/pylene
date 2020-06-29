#include <mln/core/image/ndimage.hpp>
#include <mln/io/imprint.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/view/filter.hpp>

#include <gtest/gtest.h>

TEST(IO, print2d)
{
  mln::image2d<uint8_t> x = {{1, 2, 3}, {4, 5, 6}};

  mln::io::imprint(x);
}

TEST(IO, print3d)
{
  mln::image3d<uint8_t> x = {{{1, 2, 3}, {4, 5, 6}}, {{7, 8, 9}, {10, 11, 12}}};


  mln::io::imprint(x);
}


TEST(IO, print_unknown_type)
{
  mln::image2d<mln::rgb8> x = {{{1, 1, 1}, {2, 2, 2}, {3, 3, 3}}, //
                                             {{4, 4, 4}, {5, 5, 5}, {6, 6, 6}}};

  mln::io::imprint(x);
}

namespace fmt {
  template <>
  struct formatter<mln::rgb8> : formatter<uint8_t>
  {
    template <typename FormatContext>
    auto format(const mln::rgb8& c, FormatContext& ctx)
    {
      return format_to(ctx.out(), "({}, {}, {})", c[0], c[1], c[2]);
    }
  };

  template <typename T>
  struct formatter<mln::ndpoint<2, T>>
  {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
      return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const mln::ndpoint<2, T>& p, FormatContext& ctx)
    {
      return format_to(ctx.out(), "({:d}, {:d})", p[0], p[1]);
    }
  };
}

TEST(IO, print_non_regular_domain)
{
  mln::image2d<uint8_t> x = {{1, 2, 3}, {4, 5, 6}};

  auto y = mln::view::filter(x, [](int v) { return v % 2 == 0; });
  mln::io::imprint(y);
}



