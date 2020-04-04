#include <mln/core/image/experimental/ndimage.hpp>

#include <mln/core/rangev3/view/reverse.hpp>
#include <mln/core/rangev3/foreach.hpp>

#include <fmt/core.h>
#include <iostream>

int main()
{
  mln::experimental::image2d<int> f = {{1, 2, 3}, {4, 5, 6}};

  std::cout << "== Traversing forward ==\n";
  std::cout << "Traversing through points.\n";
  mln_foreach_new (auto p, f.domain())
    fmt::print("f({},{}) = {}\n", p.x(), p.y(), f(p));

  std::cout << "Traversing on values.\n";
  mln_foreach_new (int x, f.new_values())
    std::cout << x << "\n";

  std::cout << "Traversing with pixels.\n";
  mln_foreach_new (auto px, f.new_pixels())
    fmt::print("f({},{}) = {}\n", px.point().x(), px.point().y(), px.val());


  std::cout << "\n== Traversing backward ==\n";
  std::cout << "Traversing through points.\n";
  mln_foreach_new (auto p, mln::ranges::view::reverse(f.domain()))
    fmt::print("f({},{}) = {}\n", p.x(), p.y(), f(p));

  std::cout << "Traversing on values.\n";
  mln_foreach_new (auto x, mln::ranges::view::reverse(f.new_values()))
    std::cout << x << "\n";

  std::cout << "Traversing with pixels.\n";
  mln_foreach_new (auto px, mln::ranges::view::reverse(f.new_pixels()))
    fmt::print("f({},{}) = {}\n", px.point().x(), px.point().y(), px.val());
}
