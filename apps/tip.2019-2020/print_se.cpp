#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/algorithm/paste.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/image/view/channel.hpp>
#include <mln/core/image/view/filter.hpp>
#include <mln/core/image/view/mask.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/rgb.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/point.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/labeling/blobs.hpp>
#include <mln/morpho/experimental/dilation.hpp>
#include <mln/morpho/experimental/erosion.hpp>
#include <mln/morpho/gaussian2d.hpp>


#include <fmt/core.h>


#include <cmath>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


void print_rect()
{
  auto rect = mln::se::rect2d(5, 5);
  auto pnt  = mln::point2d({2, 2});

  fmt::print("Rectangle SE:");
  for (auto&& nb : rect(pnt))
  {
    fmt::print("({},{}) ", nb.x(), nb.y());
  }
  fmt::print("\n");

  auto ses = rect.decompose();

  fmt::print("Rectangle decomposed:\n");
  for (auto&& se : ses)
  {
    for (auto&& nb : se(pnt))
    {
      fmt::print("({},{}) ", nb.x(), nb.y());
    }
    fmt::print("\n");
  }
  fmt::print("\n");

  fmt::print("Rectangle incremental:\n");
  auto inc = rect.inc();
  for (auto&& nb : inc(pnt))
  {
    fmt::print("({},{}) ", nb.x(), nb.y());
  }
  fmt::print("\n");

  fmt::print("Rectangle decremental:\n");
  auto dec = rect.dec();
  for (auto&& nb : dec(pnt))
  {
    fmt::print("({},{}) ", nb.x(), nb.y());
  }
  fmt::print("\n");
}

void print_disc()
{
  auto disc = mln::se::disc(3);
  auto pnt  = mln::point2d({3, 3});

  fmt::print("Disc SE:");
  for (auto&& nb : disc(pnt))
  {
    fmt::print("({},{}) ", nb.x(), nb.y());
  }
  fmt::print("\n");

  auto ses = disc.decompose();

  fmt::print("Disc decomposed:\n");
  for (auto&& se : ses)
  {
    for (auto&& nb : se(pnt))
    {
      fmt::print("({},{}) ", nb.x(), nb.y());
    }
    fmt::print("\n");
  }
  fmt::print("\n");
}


int main(int, char**)
{
  print_rect();
  print_disc();
}
