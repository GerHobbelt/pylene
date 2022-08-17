#pragma once
#include "mdrange_facade.hpp"

namespace ano::ranges::details
{
  /// To have a forward ndimensional-cursor.
  /// One must implement:
  ///
  /// c.__reset_to_begin
  /// c.__is_at_end
  /// c.__next
  /// c.move_to_next
  /// c.move_to_end_of_line
  template <class C>
  struct ndcursor_facade : mdcursor_facade<C>
  {
    void next() { this->__next(::meta::int_<1>()); }
    bool equal(::ranges::default_sentinel_t) const { return this->derived()->__is_at_end(C::rank - 1); }

  protected:
    C*       derived() { return static_cast<C*>(this); }
    const C* derived() const { return static_cast<const C*>(this); }

    template <int k>
    void __next(::meta::int_<k>)
    {
      if constexpr (k >= C::rank)
      {
        this->derived()->__goto_end(C::rank - 1);
      }
      else if constexpr (k == C::rank - 1)
      {
        this->derived()->__next(k);
      }
      else
      {
        this->derived()->__next(k);
        if (this->derived()->__is_at_end(k))
        {
          this->__next(::meta::int_<k + 1>());
          this->derived()->__reset_to_begin(k);
        }
      }
    }
  };

} // namespace ano::ranges::details
