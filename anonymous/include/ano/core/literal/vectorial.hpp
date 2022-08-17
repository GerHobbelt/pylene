#pragma once

namespace ano
{
  namespace literal
  {

    struct zero_t
    {
      constexpr operator int() const { return 0; }
    };

    struct one_t
    {
      constexpr operator int() const { return 1; }
    };

    static const zero_t zero = {};
    static const one_t  one  = {};

  } // namespace literal

} // namespace ano
