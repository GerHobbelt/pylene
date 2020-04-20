#include <mln/core/image/ndimage.hpp>

#include <mln/core/range/view/reverse.hpp>
#include <mln/core/range/foreach.hpp>

#include <fmt/core.h>
#include <iostream>

int main()
{
  mln::image2d<int> f = {{1, 2, 3}, {4, 5, 6}};

  std::cout << "== Traversing forward ==\n";
  std::cout << "Traversing through points.\n";
  mln_foreach_new (auto p, f.domain())
    fmt::print("f({},{}) = {}\n", p.x(), p.y(), f(p));

  std::cout << "Traversing on values.\n";
  mln_foreach_new (int x, f.values())
    std::cout << x << "\n";

  std::cout << "Traversing with pixels.\n";
  mln_foreach_new (auto px, f.pixels())
    fmt::print("f({},{}) = {}\n", px.point().x(), px.point().y(), px.val());


  std::cout << "\n== Traversing backward ==\n";
  std::cout << "Traversing through points.\n";
  mln_foreach_new (auto p, mln::ranges::view::reverse(f.domain()))
    fmt::print("f({},{}) = {}\n", p.x(), p.y(), f(p));

  std::cout << "Traversing on values.\n";
  mln_foreach_new (auto x, mln::ranges::view::reverse(f.values()))
    std::cout << x << "\n";

  std::cout << "Traversing with pixels.\n";
  mln_foreach_new (auto px, mln::ranges::view::reverse(f.pixels()))
    fmt::print("f({},{}) = {}\n", px.point().x(), px.point().y(), px.val());
}
