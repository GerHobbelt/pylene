#include <mln/core/image/image2d.hpp>

int main()
{
  mln::image2d<int> f = {{1, 2, 3}, {4, 5, 6}};

  std::cout << "== Traversing forward ==\n";
  std::cout << "Traversing through points.\n";
  mln_foreach (mln::point2d p, f.domain())
    std::cout << p << ":" << f(p) << "\n";

  std::cout << "Traversing on values.\n";
  mln_foreach (int x, f.values())
    std::cout << x << "\n";

  std::cout << "Traversing with pixels.\n";
  mln_foreach (auto x, f.pixels())
    std::cout << x.point() << ":" << x.val() << "\n";

  std::cout << "\n== Traversing backward ==\n";
  std::cout << "Traversing through points.\n";
  mln_reverse_foreach (mln::point2d p, f.domain())
    std::cout << p << ":" << f(p) << "\n";

  std::cout << "Traversing on values.\n";
  mln_reverse_foreach (int x, f.values())
    std::cout << x << "\n";

  std::cout << "Traversing with pixels.\n";
  mln_reverse_foreach (auto x, f.pixels())
    std::cout << x.point() << ":" << x.val() << "\n";
}
