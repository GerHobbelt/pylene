#pragma once

namespace mln
{

  template <class C>
  auto rbegin(C& c) -> decltype(c.rbegin())
  {
    return c.rbegin();
  }

} // namespace mln
