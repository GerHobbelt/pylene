#ifndef MLN_CORE_ALGORITHM_CLONE_HPP
# define MLN_CORE_ALGORITHM_CLONE_HPP

# include <mln/core/algorithm/copy.hpp>
# include <mln/core/image_traits.hpp>

namespace mln
{

  template <typename I>
  mln_concrete(I)
  clone(const Image<I>& ima);

  /******************/
  /* Implem         */
  /******************/

  namespace impl
  {

    // Clone by copy construction
    template <typename Image>
    mln_concrete(I)
    clone(const Image& ima, std::true_type _use_copy_construction_)
    {
      (void) _use_copy_construction_;
      mln_concrete(I) x(ima);
      return x
    }

    // Clone by deep copy
    template <typename Image>
    mln_concrete(I)
    clone(const Image& ima, std::false_type _use_copy_construction_)
    {
      (void) _use_copy_construction_;
      mln_concrete(I) x;
      resize(x, ima);
      copy(ima, out);
      return x;
    }

  }

  template <typename I>
  mln_concrete(I)
  clone(const Image<I>& ima)
  {
    return impl::clone(ima, std::integral_constant<bool,
						   std::is_convertible<I, mln_concrete(I)>::value and
						   image_traits<mln_concrete(I)>::shallow_copy::value> ());
  }


}

#endif // !MLN_CORE_ALGORITHM_CLONE_HPP
