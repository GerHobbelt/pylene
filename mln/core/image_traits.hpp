#ifndef MLN_CORE_IMAGE_TRAITS_HPP
# define MLN_CORE_IMAGE_TRAITS_HPP

# include <mln/core/image_category.hpp>
# include <type_traits>

# define mln_value(I) typename I::value_type
# define mln_point(I) typename I::point_type
# define mln_site(I)  typename I::point_type

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
    struct apply { typedef typename image_traits<Image>::accessible type; };
  };

  template <typename Image>
  struct image_category { typedef typename image_traits<Image>::category type; };

  struct image_meta_category {
    template <typename Image>
    struct apply { typedef typename image_traits<Image>::category type; };
  };

  template <typename Image>
  struct image_value
  {
    typedef typename Image::value_type type;
  };

  template <typename Image>
  struct image_pixel
  {
    typedef typename std::conditional<std::is_const<Image>::value,
                                      typename Image::const_pixel_type,
                                      typename Image::pixel_type>::type type;
  };

  template <typename Image>
  struct image_const_pixel
  {
    typedef typename Image::const_pixel_type type;
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
  struct image_pointer
  {
    typedef typename std::conditional<std::is_const<Image>::value,
                                      typename Image::const_pointer,
                                      typename Image::pointer>::type type;
  };

  template <typename Image>
  struct image_const_pointer
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
    typedef typename image_value_range<Image>::type::iterator type;
  };

  template <typename Image>
  struct image_pixel_iterator
  {
    typedef typename image_pixel_range<Image>::type::iterator type;
  };

  template <typename Image>
  struct image_const_value_iterator
  {
    typedef typename image_const_value_range<Image>::type::iterator type;
  };

  template <typename Image>
  struct image_const_pixel_iterator
  {
    typedef typename image_const_pixel_range<Image>::type::iterator type;
  };


# define MLN_GENERATE_META_IMAGE_OPERATORS(meta_op, op)			\
  struct meta_op							\
  {									\
    template <typename Image>						\
      struct apply { typedef typename op< typename std::remove_reference<Image>::type >::type type; }; \
  };

MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_value, image_value)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_reference, image_reference)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_const_reference, image_const_reference)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_pixel, image_pixel)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_const_pixel, image_const_pixel)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_pointer, image_pointer)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_const_pointer, image_const_pointer)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_value_range, image_value_range)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_const_value_range, image_const_value_range)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_pixel_range, image_pixel_range)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_const_pixel_range, image_const_pixel_range)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_value_iterator, image_value_iterator)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_const_value_iterator, image_const_value_iterator)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_pixel_iterator, image_pixel_iterator)
MLN_GENERATE_META_IMAGE_OPERATORS(image_meta_const_pixel_iterator, image_const_pixel_iterator)

}



#endif /* !MLN_CORE_IMAGE_TRAITS_HPP */
