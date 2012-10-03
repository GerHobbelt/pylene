#ifndef MLN_CORE_IMAGE_IMAGE_OPS_HPP
# define MLN_CORE_IMAGE_IMAGE_OPS_HPP

# include <mln/core/image/image_expr.hpp>
# include <mln/core/ops.hpp>
#include <iostream>
namespace mln
{
  /// \defgroup ima_ops Image operators
  /// \{

  /// \brief Check if all elements of image eval to true.
  ///
  /// \param ima The image to check.
  /// \return True iif all elements eval to True.
  ///
  /// \tparam I The type of image. It must be a model of IterableImage.
  /// \pre Image's value type must be convertible to bool.

  template <typename I>
  bool all(const Image<I>& ima);


  /// \brief Check if any element of image eval to true
  ///
  /// \param ima The image to check.
  /// \return True if at least one element evals to True.
  ///
  /// \tparam I The type of image. It must be a model of IterableImage.
  /// \pre Image's value type must be convertible to bool.
  template <typename I>
  bool any(const Image<I>& ima);


  /// \}

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

# define MLN_GENERATE_LVALUE_UNARY_EXPR(Name, Obj)                      \
  template <typename I>                                                 \
  unary_image_expr< Obj<typename I::value_type>, I>                     \
  Name (I&& ima)                                                         \
  {                                                                     \
    BOOST_CONCEPT_ASSERT(( Image<typename std::remove_reference<I>::type )); \
    return make_unary_image_expr(std::forward<I>(ima), Obj<typename I::reference> ()); \
  }




# define MLN_GENERATE_CONST_UNARY_EXPR(Name, Obj)                       \
  template <typename I>                                                 \
  unary_image_expr< Obj<typename I::value_type>, const I&>                     \
  Name (const Image<I>& ima)                                            \
  {                                                                     \
    return make_unary_image_expr(exact(ima), Obj<typename I::value_type> ()); \
  }

# define MLN_GENERATE_CONST_BINARY_EXPR(Name, Obj)                      \
  template <typename I1, typename I2>                                   \
  binary_image_expr< Obj<typename I1::value_type, typename I2::value_type>, const I1&, const I2&> \
  Name (const Image<I1>& ima1, const Image<I2>& ima2)                    \
  {                                                                     \
    typedef Obj<typename I1::value_type, typename I2::value_type> O; \
    return make_binary_image_expr(exact(ima1), exact(ima2), O());    \
  }                                                                  \
                                                                        \
  template <typename I, typename Scalar>                                \
  typename std::enable_if< !std::is_base_of<Image<Scalar>, Scalar>::value, \
                           binary_image_scalar_expr< Obj<typename I::value_type, Scalar>, const I&, Scalar> >::type \
  Name (const Image<I>& ima, const Scalar& x)                           \
  {                                                                     \
    typedef Obj<typename I::value_type, Scalar> O;                      \
    return make_binary_image_scalar_expr(exact(ima), x, O());                  \
  }                                                                     \
                                                                        \
  template <typename I, typename Scalar>                                \
  typename std::enable_if< !std::is_base_of<Image<Scalar>, Scalar>::value, \
                           binary_scalar_image_expr< Obj<Scalar, typename I::value_type>, Scalar, const I&> >::type \
  Name (const Scalar& x, const Image<I>& ima)                           \
  {                                                                     \
    typedef Obj<Scalar, typename I::value_type> O;                      \
    return make_binary_scalar_image_expr(x, exact(ima), O());                  \
  }



  // Point-wise unary operators
  MLN_GENERATE_CONST_UNARY_EXPR(operator-, negate);
  MLN_GENERATE_CONST_UNARY_EXPR(lnot, logical_not);

  // Point-wise arithmetic operators
  MLN_GENERATE_CONST_BINARY_EXPR(operator+, add);
  MLN_GENERATE_CONST_BINARY_EXPR(operator-, substract);
  MLN_GENERATE_CONST_BINARY_EXPR(operator*, multiplies);
  MLN_GENERATE_CONST_BINARY_EXPR(operator/, devides);

  // Point-wise relational operators
  MLN_GENERATE_CONST_BINARY_EXPR(operator==, equal_to);
  MLN_GENERATE_CONST_BINARY_EXPR(operator!=, not_equal_to);
  MLN_GENERATE_CONST_BINARY_EXPR(operator<,  less_than);
  MLN_GENERATE_CONST_BINARY_EXPR(operator>,  greater_than);
  MLN_GENERATE_CONST_BINARY_EXPR(operator<=, less_equal);
  MLN_GENERATE_CONST_BINARY_EXPR(operator>=, greater_equal);



  template <typename I>
  inline
  bool
  all(const Image<I>& ima)
  {
    BOOST_CONCEPT_ASSERT((IterableImage<I>));
    static_assert(std::is_convertible<typename I::value_type, bool>::value,
                  "Image value type must be convertible to bool");

    mln_viter(v, exact(ima));
    mln_forall(v)
      if (!*v) return false;

    return true;
  }

  template <typename I>
  inline
  bool
  any(const Image<I>& ima)
  {
    BOOST_CONCEPT_ASSERT((IterableImage<I>));
    static_assert(std::is_convertible<typename I::value_type, bool>::value,
                  "Image value type must be convertible to bool");

    mln_viter(v, exact(ima));
    mln_forall(v)
      if (*v) return true;

    return false;
  }




} // end of namespace mln



#endif //!MLN_CORE_IMAGE_IMAGE_OPS_HPP
