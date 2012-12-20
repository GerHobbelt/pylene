#ifndef FORMAT_HPP
# define FORMAT_HPP

# include <iostream>
# include <mln/core/grays.hpp>

namespace mln
{

  namespace io
  {

    template <typename T>
    inline
    std::ostream&
    format(std::ostream& os, const T& v)
    {
      return os << v;
    }


    template <>
    inline
    std::ostream&
    format<uint8>(std::ostream& os, const uint8& v)
    {
      return os << (int)v;
    }


  }

}

#endif // ! FORMAT_HPP
