#ifndef MLN_CORE_VEC_VEC_IO_HPP
# define MLN_CORE_VEC_VEC_IO_HPP

# include <mln/core/vec.hpp>
# include <mln/io/format.hpp>

namespace mln
{

  namespace internal
  {

    template <typename T, unsigned dim, typename tag>
    inline
    std::ostream&
    operator<< (std::ostream& os, const vec_base<T, dim, tag>& x)
    {
      os << '[';
      for (unsigned i = 0; i < dim-1; ++i)
	os << x[i] << ',';
      os << x[dim-1] << ']';
      return os;
    }


    template <typename T, unsigned dim, typename tag>
    inline
    std::ostream&
    format (std::ostream& os, const vec_base<T, dim, tag>& x)
    {
      using io::format;

      os << '[';
      for (unsigned i = 0; i < dim-1; ++i)
	format(os, x[i]) << ',';
      format(os, x[dim-1]) << ']';
      return os;
    }

  }

}

#endif // ! MLN_CORE_VEC_VEC_IO_HPP
