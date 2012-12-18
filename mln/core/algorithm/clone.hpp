#ifndef MLN_CORE_ALGORITHM_CLONE_HPP
# define MLN_CORE_ALGORITHM_CLONE_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/algorithm/copy.hpp>

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
    mln_concrete(Image)
    clone(const Image& ima, std::true_type _use_copy_construction_)
    {
      (void) _use_copy_construction_;
      mln_concrete(Image) x(ima);
      return x;
    }

    // Clone by deep copy
    template <typename Image>
    mln_concrete(Image)
    clone(const Image& ima, std::false_type _use_copy_construction_)
    {
      (void) _use_copy_construction_;
      mln_concrete(Image) x;
      resize(x, ima);
      copy(ima, x);
      return x;
    }

  }

  template <typename I>
  mln_concrete(I)
  clone(const Image<I>& ima)
  {
    return impl::clone(exact(ima), check_t<std::is_convertible<I, mln_concrete(I)>::value and
                       not image_traits<mln_concrete(I)>::shallow_copy::value> ());
  }


}

#endif // !MLN_CORE_ALGORITHM_CLONE_HPP
