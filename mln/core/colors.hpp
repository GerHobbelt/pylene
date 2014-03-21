#ifndef MLN_CORE_COLORS_HPP
# define MLN_CORE_COLORS_HPP

# include <mln/core/value/int.hpp>
# include <mln/core/ops.hpp>
# include <mln/core/vec_base.hpp>
# include <mln/core/image/morphers/transformed_image.hpp>

namespace mln
{

  struct rgb_tag {};
  struct bgr_tag {};

  namespace internal
  {
    template <>
    struct vec_base_traits<rgb_tag>
    {
      static const bool is_additive = true;
      static const bool is_additive_ext = true;
      static const bool is_multiplicative = false;
      static const bool is_multiplicative_ext = true;
      static const bool is_less_than_comparable = false;
      static const bool is_equality_comparable = true;
    };

    template <>
    struct vec_base_traits<bgr_tag>
    {
      static const bool is_additive = true;
      static const bool is_additive_ext = true;
      static const bool is_multiplicative = false;
      static const bool is_multiplicative_ext = true;
      static const bool is_less_than_comparable = false;
      static const bool is_equality_comparable = true;
    };
  }

  template <typename T>
  using rgb = internal::vec_base<T, 3, rgb_tag>;

  typedef rgb<uint8> rgb8;
  typedef rgb<uint16> rgb16;

  template <typename T>
  using bgr = internal::vec_base<T, 3, bgr_tag>;

  typedef bgr<uint8> bgr8;
  typedef bgr<uint16> bgr16;


  MLN_DECLARE_IMAGE_LVALUE_OPERATOR(red,   (mln::getter<0, const rgb8&>), (mln::getter<0, rgb8&>));
  MLN_DECLARE_IMAGE_LVALUE_OPERATOR(green, (mln::getter<1, const rgb8&>), (mln::getter<1, rgb8&>));
  MLN_DECLARE_IMAGE_LVALUE_OPERATOR(blue,  (mln::getter<2, const rgb8&>), (mln::getter<2, rgb8&>));

}

#endif // !MLN_CORE_COLORS_HPP
