#ifndef MLN_MOPRHO_TOS_IRANGE_HPP
# define MLN_MOPRHO_TOS_IRANGE_HPP

# include <iostream>

namespace mln
{

  namespace morpho
  {

    namespace tos
    {

      template <class V>
      struct irange
      {
	V lower, upper;
      };


      template <typename V>
      operator <<(std::ostream& os, const irange<V>& rng)
      {
	return (os << '[' << rng.lower << ',' << rng.upper << ']' << std::endl);
      }

    }

  }

}

#endif // ! MLN_MOPRHO_TOS_IRANGE_HPP
