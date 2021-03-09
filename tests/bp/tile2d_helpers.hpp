#pragma once

#include <mln/bp/tile.hpp>

#include <iosfwd>
#include <fmt/ostream.h>

namespace mln::bp
{

  template <class U, class V>
  bool operator== (Tile2DView<U> a, Tile2DView<V> b)
  {
    for (int y = 0; y < a.height(); ++y)
    {
      const U* lineptr_a = a.row(y);
      const U* lineptr_b = b.row(y);
      if (!std::equal(lineptr_a, lineptr_a + a.width(), lineptr_b))
        return false;
    }
    return true;
  }

  template <class U>
  void PrintTo(Tile2DView<U> m, std::ostream* os)
  {
    *os << "\n";
    for (int y = 0; y < m.height(); ++y)
    {
      const U* lineptr = m.row(y);
      for (int x = 0; x < m.width() - 1; ++x)
        fmt::print(*os, "{:03} ", lineptr[x]);
      fmt::print(*os, "{:03}", lineptr[m.width() - 1]);
      *os << "\n";
    }
  }
}
