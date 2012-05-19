#ifndef MLN_CORE_IMAGE_TRAITS_HPP
# define MLN_CORE_IMAGE_TRAITS_HPP

# include <mln/core/image_category.hpp>

namespace mln
{
  struct image_dynamic_tag {};
  struct image_static_tag {};

  template <typename I>
  struct image_traits
  {
    // Type of image (dynamic or static)
    typedef typename I::category category;
    static const bool accessible = I::accessible;

    // struct value_access
    // {
    //   static const bool readable = I::value_access::readable;
    //   static const bool writable = I::value_access::writable;
    //   static const bool lvalue = I::value_access::lvalue;

    // };
  };

}



#endif /* !MLN_CORE_IMAGE_TRAITS_HPP */
