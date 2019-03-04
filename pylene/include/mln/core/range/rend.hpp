#pragma once

namespace mln
{

  template <class C>
  auto rend(C& c) -> decltype(c.rend())
  {
    return c.rend();
  }

} // namespace mln
