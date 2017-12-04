#ifndef MLN_MOPRHO_TOS_IMPL_IRANGE_HPP
#define MLN_MOPRHO_TOS_IMPL_IRANGE_HPP

#include <mln/io/format.hpp>

namespace mln
{

  namespace morpho
  {

    namespace ToS
    {

      namespace impl
      {

        template <class V>
        struct irange
        {
          using value_type = V;

          irange() = default;
          irange(const V& v) : lower(v), upper(v) {}
          irange(const V& lower_, const V& upper_) : lower(lower_), upper(upper_) {}

          irange extend(const irange& other) const
          {
            return {std::min(lower, other.lower), std::max(upper, other.upper)};
          }

          bool operator==(const irange& other) const
          {
            return (lower == other.lower) && (upper == other.upper);
          }

          bool operator!=(const irange& other) const
          {
            return !(*this == other);
          }

          V lower, upper;
        };

        template <typename V>
        std::ostream& operator<<(std::ostream& os, const irange<V>& rng)
        {
          using namespace mln::io;
          os << '[';
          format(os, rng.lower);
          os << ',';
          format(os, rng.upper);
          return os << ']';
        }
      }
    }
  }
}

#endif // ! MLN_MOPRHO_TOS_IRANGE_HPP
