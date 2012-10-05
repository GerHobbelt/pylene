#ifndef MLN_CORE_IMAGE_IMAGE2D_HPP
# define MLN_CORE_IMAGE_IMAGE2D_HPP

# include <mln/core/image/ndimage.hpp>

namespace mln {

  // FWD declaration
  template <typename T> struct image2d;


  // Specialization of traits
  template <typename T>
  struct image_traits< image2d<T> > : image_traits< ndimage_base<T, 2, image2d<T> > >
  {
  };


  ///
  /// \brief The standard 2D image type.
  ///
  /// \p image2d is a type to represent 2d image. They are implemented with contiguous
  /// data buffer to allow pointer arithmetic. However data are not contiguous in this buffer
  /// since each row is aligned on a 128-bits boundary to get performance. \p image2d are soft image
  /// since two images can share the same buffer of data to avoid uncessary copies.
  ///
  /// \p image2d models a Writable Point-Accessible Image concept.
  /// See \see image2d \see image3d
  template <typename T>
  struct image2d : ndimage_base<T, 2, image2d<T> >
  {
  private:
    typedef ndimage_base<T, 2, image2d<T> > base;

  public:
    typedef typename base::domain_type domain_type;


    explicit image2d (unsigned border = 3) : ndimage_base<T,2,image2d<T> > (border)
    {
    }

    explicit image2d(const domain_type& domain, unsigned border = 3)
      : ndimage_base<T,2, image2d<T> >(domain, border)
    {
    }


    image2d(short nrows, short ncols, unsigned border = 3)
      : ndimage_base<T,2, image2d<T> >( (box<short,2>) {{{0,0}},{{nrows, ncols}}}, border)
    {
    }

    image2d(image2d<T>&& other)
      : base( std::move(other) )
    {
    }

    image2d(image2d<T>& other)
      : base( other )
    {
    }

    template <typename U>
    image2d(const image2d<U>& other,
	    typename std::enable_if< std::is_const<T>::value and
	    std::is_convertible<U*, T*>::value>::type* = NULL)
      : base( other )
    {
    }


    unsigned nrows() const { return this->domain_.pmax[0] - this->domain_.pmin[0]; }
    unsigned ncols() const { return this->domain_.pmax[1] - this->domain_.pmin[1]; }

  };



} // end of namespace mln



#endif //!MLN_CORE_IMAGE_IMAGE2D_HPP
