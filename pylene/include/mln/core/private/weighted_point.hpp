#pragma once

namespace mln
{

  template <class P, class W>
  struct weighted_point
  {
    P p;
    W w;

    constexpr W weight() const noexcept { return w; }
    constexpr P point() const noexcept { return p; }

    // FIXME: requires Arithmetic P
    weighted_point& operator+=(P dp) noexcept
    {
      p += dp;
      return *this;
    }

    weighted_point operator+(P dp) const noexcept
    {
      auto q = *this;
      q.p += dp;
      return q;
    }
  };
}
