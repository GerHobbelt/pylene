#pragma once

#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/iref.hpp>
#include <mln/core/iterator/filter_iterator.hpp>
#include <mln/core/iterator/transform_iterator.hpp>
#include <mln/core/pixel_utility.hpp>

#include <boost/mpl/and.hpp>
#include <boost/mpl/or.hpp>

#include <type_traits>


// FIXME:
namespace to_migrate
{
  namespace impl
  {
    // Clone by copy construction
    template <typename Image>
    mln_concrete(Image) clone(const Image& ima, std::true_type _use_copy_construction_)
    {
      (void)_use_copy_construction_;
      mln_concrete(Image) x(ima);
      return x;
    }

    // Clone by deep copy
    template <typename Image>
    mln_concrete(Image) clone(const Image& ima, std::false_type _use_copy_construction_)
    {
      (void)_use_copy_construction_;
      mln_concrete(Image) x = imconcretize(ima);
      mln::copy(ima, x);
      return x;
    }
  } // namespace impl


  template <typename I>
  [[deprecated]] mln_concrete(I) __clone(const mln::Image<I>& ima) {
    return impl::clone(exact(ima), mln::check_t<(std::is_convertible<I, mln_concrete(I)>::value and
                                                 not mln::image_traits<mln_concrete(I)>::shallow_copy::value)>());
  }
} // namespace to_migrate

namespace mln
{

  /// \defgroup free_functions Free functions
  /// \ingroup image
  /// \{

  template <typename I, typename J>
  bool are_indexes_compatible(const Image<I>& f, const Image<J>& g);

  /// \brief Return a concrete version of the image.
  ///
  /// \p eval returns a concrete version of \p ima. If
  /// If \p ima is already concrete then it returns the
  /// image unmodified. Otherwise, it clones the content of
  /// \p ima.
  ///
  /// \param ima Image to set concrete
  ///
  /// \return A concrete duplicate of the image or the image itself.
  ///
  template <typename InputImage>
  typename std::enable_if<image_traits<InputImage>::concrete::value, InputImage&&>::type eval(InputImage&& ima);

  ///\}

  /*************************/
  /***  Implementation   ***/
  /*************************/

  namespace internal
  {
    /*
    template <typename... I>
    inline
    constexpr
    typename
    std::enable_if< not boost::mpl::and_<typename image_traits<I>::indexable ...>::value,
                    bool >::type
    are_indexes_compatible(const I&...)
    {
      return false;
    }


    template <typename I, typename J, typename... R>
    inline
    typename
    std::enable_if< boost::mpl::and_<
                      typename image_traits<I>::indexable,
                      typename image_traits<J>::indexable,
                      typename image_traits<R>::indexable...>::value,
                    bool>::type
    are_indexes_compatible(const I& f, const J& g, const R&... rest)
    {
      return f.is_index_compatible(g) and are_indexes_compatible(f, rest...);
    }
    */
  }

  template <typename I, typename J>
  inline bool are_indexes_compatible(const Image<I>& f, const Image<J>& g)
  {
    (void)f;
    (void)g;
    return false;
    // return internal::are_indexes_compatible(exact(f), exact(g));
  }

  template <typename InputImage>
  inline typename std::enable_if<image_traits<InputImage>::concrete::value, InputImage&&>::type eval(InputImage&& ima)
  {
    return std::forward<InputImage>(ima);
  }

  template <typename InputImage>
  inline typename std::enable_if<!image_traits<InputImage>::concrete::value,
                                 mln_concrete(typename std::remove_reference<InputImage>::type)>::type
      eval(InputImage&& ima)
  {
    // FIXME:
    return ::to_migrate::__clone(std::forward<InputImage>(ima));
  }
} // namespace mln
