#pragma once
#include <range/v3/range_fwd.hpp>
#include <iostream>
namespace mln::ranges::details
{
  template <std::size_t Rank, class Derived>
  struct forward_multidimensional_cursor_facade
  {
  protected:
    // The following method have to be implemented in derived class:
    //
    // bool __is_at_end(int rnk);
    // void __next(int rnk);
    // void __reset_to_begin(int rnk);

    void next()
    {
      std::size_t k;
      for (k = Rank; k-- > 0;)
      {
        this->derived()->__next(k);
        if (!this->derived()->__is_at_end(k))
        {
          // unstack
          while (++k < Rank)
            this->derived()->__reset_to_begin(k);
          return;
        }
      }
    }

    bool equal(::ranges::default_sentinel) const { return this->derived()->__is_at_end(0); }

  protected:
    const Derived* derived() const { return static_cast<const Derived*>(this); }
    Derived* derived() { return static_cast<Derived*>(this); }
  };
} // namespace mln::ranges::details
