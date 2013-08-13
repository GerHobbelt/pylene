#ifndef MLN_IO_FORMAT_HPP
# define MLN_IO_FORMAT_HPP

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


    inline
    std::ostream&
    format(std::ostream& os, const uint8& v)
    {
      return os << (int)v;
    }

  }

}

#endif // ! MLN_IO_FORMAT_HPP
