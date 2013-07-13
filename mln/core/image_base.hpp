#ifndef MLN_CORE_IMAGE_IMAGE_HPP
# warning "Do not include this file directly but <mln/core/image/image.hpp>"
# include <mln/core/image/image.hpp>
#endif

#ifndef MLN_CORE_IMAGE_BASE_HPP
# define MLN_CORE_IMAGE_BASE_HPP

/// \file

# include <mln/core/image/image.hpp>
# include <cstddef>

namespace mln
{

  struct undef {};

# ifndef MLN_DOXYGEN

  /// \brief base image
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

    //typedef SizeType size_type;
    //typedef Difference difference_type;

    /*
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
    */
  };


# else /* MLN_DOXYGEN */

  /// \brief base class for images
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
  public:
    /**
    * \typedef implementation_defined site_type
    * \brief Type of point/site of the image.
    *
    * \typedef implementation_defined point_type
    * \brief Alias for site_type
    *
    * \typedef implementation_defined domain_type
    * \brief Type of the image domain.
    *
    * \typedef implementation_defined value_type
    * \brief Type of image values.
    *
    * \typedef implementation_defined reference
    * \brief Reference type to a pixel value.
    *
    * \typedef implementation_defined const_reference
    * \brief Const reference type to a pixel value.
    */
    typedef implementation_defined site_type;
    typedef implementation_defined point_type;
    typedef implementation_defined value_type;
    typedef implementation_defined domain_type;

    /** \name Element Access
    * \{ \}
    */

    /**
    * \brief access specified element by its index
    *
    * Accessing pixel values through their indexes allows a faster
    * processing compared to point/site access (see operator()(const
    * site_type& p) const ).
    *
    * \param i index of the element
    * \return Reference to requested element
    */
    virtual const_reference operator[] (size_type i) const;
    virtual reference	    operator[] (size_type i);

    /**
    * \brief access specified element by its point/site
    *
    * Access a pixel values through their point/site. Note that the
    * point must belong to the domain of the image. To access the
    * extension, use at(const site_type&).
    *
    * \param p location of the element
    * \return Reference to the requested element
    * \pre \p p must belongs to the domain
    */
    const_reference operator() (const site_type& p) const;
    reference	    operator() (const site_type& p);
    /**
    * \brief access specified element by its point/site without bound cheking
    *
    * This accessor allows to access a value which is in the extension
    * of the image, contrary to the operator () that allows to access
    * only values within the image domain.
    *
    * \param p location of the element
    * \return Reference to the requested element
    * \pre \p p must belongs to the domain.
    */
    reference		at (const site_type& p);
    const_reference	at (const site_type& p) const;
    /**
    * \{ \}
    */
  }

# endif // !MLN_DOXYGEN

} // end of namespace mln

#endif //!MLN_CORE_IMAGE_BASE_HPP
