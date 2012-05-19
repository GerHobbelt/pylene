#ifndef MLN_CORE_NDIMAGE_PIXEL_HPP
# define MLN_CORE_NDIMAGE_PIXEL_HPP

# include <mln/core/ndimage.hpp>

namespace mln
{


  template <typename T, unsigned dim, typename I>
  struct ndimage_pixel
  {
    typedef typename point<short, dim>::type    site_type;
    typedef T                                   value_type;
    typedef T&                                  reference_type;
    typedef ptrdiff_t                           difference_type;
    typedef I                                   image_type;

    reference_type      val() const;
    site_type           point()  const;
    difference_type     offset() const;
    image_type&         image() const;

    template <typename, unsigned, typename> friend struct ndimage_pixel_iterator;
    template <typename, unsigned, typename> friend struct ndimage_base;

  private:
    site_type           point_;
    image_type*         ima_;
    char*               ptr_;
  };


  template <typename T, unsigned dim, typename I>
  inline
  typename ndimage_pixel<T,dim,I>::reference_type
  ndimage_pixel<T,dim,I>::val() const
  {
    mln_precondition(ptr_ != NULL);
    return *reinterpret_cast<T*>(ptr_);
  }

  template <typename T, unsigned dim, typename I>
  inline
  typename ndimage_pixel<T,dim,I>::site_type
  ndimage_pixel<T,dim,I>::point() const
  {
    return point_;
  }

  template <typename T, unsigned dim, typename I>
  inline
  typename ndimage_pixel<T,dim,I>::difference_type
  ndimage_pixel<T,dim,I>::offset() const
  {
    mln_precondition(ima_ != NULL && ptr_ != NULL);
    return static_cast<char*>(ptr_) - ima_->ptr_;
  }

  template <typename T, unsigned dim, typename I>
  inline
  typename ndimage_pixel<T,dim,I>::image_type&
  ndimage_pixel<T,dim,I>::image() const
  {
    mln_precondition(ima_ != NULL);
    return *ima_;
  }


}

#endif
