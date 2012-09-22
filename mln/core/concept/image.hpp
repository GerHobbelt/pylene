#ifndef MLN_CORE_CONCEPT_IMAGE_HPP
# define MLN_CORE_CONCEPT_IMAGE_HPP

# include <mln/core/image_traits.hpp>
# include <mln/core/concept/object.hpp>
# include <boost/concept_check.hpp>
# include <boost/range/iterator.hpp>
# include <boost/static_assert.hpp>

namespace mln {


  void check(const std::true_type&)
  {
  }


  template <typename I>
  struct Image : Object_<I>
  {
  private:
    typedef image_traits<I> traits;

  public:
    typedef typename traits::accessible  accessible;
    typedef typename traits::category    category;

    BOOST_CONCEPT_USAGE(Image)
    {
      typedef typename I::value_type         value;
      typedef typename I::reference          reference;
      typedef typename I::const_reference    const_reference;
      typedef typename I::pixel_type         pixel;
      typedef typename I::const_pixel_type   const_pixel;


      typedef typename I::site_type          site_type;
      typedef typename I::point_type         point_type;
      typedef typename I::domain_type        domain_type;

      const domain_type& (I::*method) () const = &I::domain;
      (void) method;

      check(std::is_convertible<typename pixel::value_type, value> ());
      check(std::is_same<typename pixel::reference, reference> ());
      check(std::is_convertible<typename const_pixel::value_type, value> ());
      check(std::is_same<typename const_pixel::reference, const_reference> ());
    }
  };

  template <typename I>
  struct AccessibleImage : Image<I>
  {
  public:
    typedef typename Image<I>::accessible accessible;

    static_assert(accessible::value, "Image must be accessible");

    BOOST_CONCEPT_USAGE(AccessibleImage)
    {
      typedef typename I::point_type        point_type;
      typedef typename I::reference         reference;
      typedef typename I::const_reference   const_reference;

      reference (I::*ptr) (point_type) = &I::operator();
      const_reference (I::*ptr2) (point_type) const = &I::operator();
      (void) ptr; (void) ptr2;
    }
  };

  template <typename I>
  struct IterableImage : Image<I>
  {
  public:
    typedef typename Image<I>::category category;

    static_assert(std::is_convertible<category, forward_image_tag>::value,
                  "Image category must be iterable");

    BOOST_CONCEPT_USAGE(IterableImage)
    {
      typedef typename I::value_type         value_type;
      typedef typename I::reference         reference;
      typedef typename I::const_reference   const_reference;
      typedef typename I::pixel_type        pixel_type;
      typedef typename I::const_pixel_type  const_pixel_type;

      typedef typename I::value_range               value_range;
      typedef typename I::const_value_range         const_value_range;
      typedef typename I::pixel_range               pixel_range;
      typedef typename I::const_pixel_range         const_pixel_range;


      value_range       (I::*ptr1) () = &I::values;
      const_value_range (I::*ptr2) () const = &I::values;
      pixel_range       (I::*ptr3) () = &I::pixels;
      const_pixel_range (I::*ptr4) () const = &I::pixels;

      (void) ptr1;
      (void) ptr2;
      (void) ptr3;
      (void) ptr4;

      typedef typename boost::range_iterator<value_range>::type       value_iterator;
      typedef typename boost::range_iterator<const_value_range>::type const_value_iterator;
      typedef typename boost::range_iterator<pixel_range>::type       pixel_iterator;
      typedef typename boost::range_iterator<const_pixel_range>::type const_pixel_iterator;

      check(std::is_convertible<typename std::iterator_traits<value_iterator>::value_type, value_type> ());
      // "Iterator's value type is expected to be the image value type.");
      check(std::is_same<typename std::iterator_traits<value_iterator>::reference, reference> ());
      // "Iterator's reference is expected to be the image reference");
      check(std::is_convertible<typename std::iterator_traits<const_value_iterator>::value_type, value_type> ());
      // "Iterator's value type is expected to be the image value type");
      check(std::is_same<typename std::iterator_traits<const_value_iterator>::reference, const_reference> ());
      // "Iterator's reference type is expected to be the image const reference");

      //check(std::is_const<typename std::remove_reference<const_reference>::type > ());

      check(std::is_convertible<typename std::iterator_traits<pixel_iterator>::reference, pixel_type> ());
      check(std::is_same<typename std::iterator_traits<pixel_iterator>::value_type, pixel_type> ());
      // "Pixel Iterator's value type is expected to be the image pixel type");

      check(std::is_convertible<typename std::iterator_traits<const_pixel_iterator>::reference, const_pixel_type> ());
      check(std::is_same<typename std::iterator_traits<const_pixel_iterator>::value_type, const_pixel_type> ());
      // "Pixel Iterator's value type is expected to be the image const pixel type");

      check(std::is_same<typename pixel_type::image_type, I> ());
      check(std::is_same<typename const_pixel_type::image_type, const I> ());
    }
  };




} // end of namespace mln


#endif //!MLN_CORE_CONCEPT_IMAGE_HPP
