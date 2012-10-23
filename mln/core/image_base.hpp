#ifndef MLN_CORE_IMAGE_BASE_HPP
# define MLN_CORE_IMAGE_BASE_HPP


# include <mln/core/image/image.hpp>
# include <cstddef>

namespace mln
{
  /// Free functions
  /// \{

  template <typename I, typename J>
  typename std::enable_if<image_traits<I>::indexable::value and image_traits<J>::indexable::value>::type
  reindex(Image<I>& ima, const Image<J>& ima2)
  {
    exact(ima).reindex(exact(ima2).index_of_point(exact(ima2).domain().pmin));
  }

  template <typename I, typename J>
  typename std::enable_if<!(image_traits<I>::indexable::value and image_traits<J>::indexable::value)>::type
  reindex(Image<I>&, const Image<J>&)
  {
  }

  /// \brief Initialize an image from an other such that
  /// domains correspond
  ///
  /// \param ima Image to be initialized
  /// \param other Image to initialize with.
  ///
  template <typename I, typename J>
  void resize(Image<I>& ima, const Image<J>& other)
  {
    exact(ima).resize(exact(other).domain());
    reindex(ima, other);
  }


  template <typename I, typename J>
  void resize(Image<I>& ima, const Image<J>& other, int border, mln_value(I) v)
  {
    exact(ima).resize(exact(other).domain(), border, v);
    reindex(ima, other);
  }
  /// \}


  struct undef {};

  template <typename Derived,
            typename Point,
            typename Value,
            typename Pixel = undef,
            typename ConstPixel = undef,
            typename Reference = Value&,
            typename ConstReference = const Value&,
            typename Pointer = Value*,
            typename ConstPointer = const Value*,
            typename SizeType = std::size_t,
            typename Difference = std::ptrdiff_t
            >
  struct image_base : mln::Image<Derived>
  {
    typedef Point point_type;
    typedef Point site_type;
    typedef Pixel pixel_type;
    typedef ConstPixel const_pixel_type;

    typedef Value value_type;
    typedef Reference reference;
    typedef ConstReference const_reference;
    typedef Pointer pointer;
    typedef ConstPointer const_pointer;

    typedef SizeType size_type;
    typedef Difference difference_type;


    reference
    operator () (const pixel_type& pix)
    {
      mln_precondition(pix.image() == this);
      return pix.val();
    }

    const_reference
    operator () (const const_pixel_type& pix) const
    {
      mln_precondition(pix.image() == this);
      return pix.val();
    }
  };

} // end of namespace mln

#endif //!MLN_CORE_IMAGE_BASE_HPP
