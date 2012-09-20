#ifndef MLN_CORE_IMAGE_TRAITS_HPP
# define MLN_CORE_IMAGE_TRAITS_HPP

# include <mln/core/image_category.hpp>
# include <type_traits>
# include <boost/range/iterator.hpp>

namespace mln
{
  struct image_dynamic_tag {};
  struct image_static_tag {};

  template <typename I>
  struct image_traits : image_traits< typename std::decay<I>::type > {};

  template <typename I>
  struct image_traits<const I> : image_traits<I> {};

  template <typename I>
  struct image_traits<volatile I> : image_traits<I> {};

  template <typename I>
  struct image_traits<const volatile I> : image_traits<I> {};

  //{
    // Type of image (dynamic or static)
    //typedef typename image_traits<I>::category category;
    //static const bool accessible = image_traits<I>::accessible;

    // struct value_access
    // {
    //   static const bool readable = I::value_access::readable;
    //   static const bool writable = I::value_access::writable;
    //   static const bool lvalue = I::value_access::lvalue;

    // };
//}


  /// \group Meta-functions
  /// \{
  template <typename Image>
  struct image_accessibility { typedef typename image_traits<Image>::accessible type; };

  struct image_meta_accessibility {
    template <typename Image>
    struct apply { typedef typename image_traits< typename std::decay<Image>::type >::accessible type; };
  };

  template <typename Image>
  struct image_category { typedef typename image_traits<Image>::category type; };

  struct image_meta_category {
    template <typename Image>
    struct apply { typedef typename image_traits< typename std::decay<Image>::type >::category type; };
  };

  template <typename Image>
  struct image_value
  {
    typedef typename std::remove_const<Image>::type::value_type type;
  };

  template <typename Image>
  struct image_pixel
  {
    typedef typename std::conditional<std::is_const<Image>::value,
                                      typename std::remove_const<Image>::type::const_pixel_type,
                                      typename Image::pixel_type>::type type;
  };

  template <typename Image>
  struct image_const_pixel
  {
    typedef typename std::remove_const<Image>::type::const_pixel_type type;
  };

  template <typename Image>
  struct image_reference
  {
    typedef typename std::conditional<std::is_const<Image>::value,
                                      typename Image::const_reference,
                                      typename Image::reference>::type type;
  };

  template <typename Image>
  struct image_const_reference
  {
    typedef typename Image::const_reference type;
  };

  template <typename Image>
  struct image_value_range
  {
    typedef typename std::conditional<std::is_const<Image>::value,
                                      typename Image::const_value_range,
                                      typename Image::value_range>::type type;
  };

  template <typename Image>
  struct image_pixel_range
  {
    typedef typename std::conditional<std::is_const<Image>::value,
                                      typename Image::const_pixel_range,
                                      typename Image::pixel_range>::type type;
  };

  template <typename Image>
  struct image_const_value_range
  {
    typedef typename Image::const_value_range type;
  };

  template <typename Image>
  struct image_const_pixel_range
  {
    typedef typename Image::const_pixel_range type;
  };

  template <typename Image>
  struct image_value_iterator
  {
    typedef typename boost::range_iterator<typename image_value_range<Image>::type>::type type;
  };

  template <typename Image>
  struct image_pixel_iterator
  {
    typedef typename boost::range_iterator<typename image_pixel_range<Image>::type>::type type;
  };

  template <typename Image>
  struct image_const_value_iterator
  {
    typedef typename boost::range_iterator<typename image_const_value_range<Image>::type>::type type;
  };

  template <typename Image>
  struct image_const_pixel_iterator
  {
    typedef typename boost::range_iterator<typename image_const_pixel_range<Image>::type>::type type;
  };


  struct image_meta_value {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::value_type type; };
  };

  struct image_meta_pixel {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::pixel_type type; };

    template <typename Image>
    struct apply<const Image&> { typedef typename std::decay<Image>::type::const_pixel_type type; };
  };

  struct image_meta_const_pixel {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::const_pixel_type type; };
  };

  struct image_meta_reference {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::reference type; };

    template <typename Image>
    struct apply<const Image&> { typedef typename std::decay<Image>::type::const_reference type; };
  };

  struct image_meta_const_reference {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::const_reference type; };
  };

  struct image_meta_pointer {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::pointer type; };

    template <typename Image>
    struct apply<const Image&> { typedef typename std::decay<Image>::type::const_pointer type; };
  };

  struct image_meta_const_pointer {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::const_pointer type; };
  };


  struct image_meta_value_iterator {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::value_iterator type; };

    template <typename Image>
    struct apply<const Image&> { typedef typename std::decay<Image>::type::const_value_iterator type; };
  };

  struct image_meta_const_value_iterator {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::const_value_iterator type; };
  };

  struct image_meta_value_range {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::value_range type; };

    template <typename Image>
    struct apply<const Image&> { typedef typename std::decay<Image>::type::const_value_range type; };
  };

  struct image_meta_const_value_range {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::const_value_range type; };
  };

  struct image_meta_pixel_iterator {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::pixel_iterator type; };

    template <typename Image>
    struct apply<const Image&> { typedef typename std::decay<Image>::type::const_pixel_iterator type; };
  };

  struct image_meta_const_pixel_iterator {
    template <typename Image>
    struct apply { typedef typename std::decay<Image>::type::const_pixel_iterator type; };
  };

  /// \}
}



#endif /* !MLN_CORE_IMAGE_TRAITS_HPP */
