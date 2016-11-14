#ifndef MLN_CORE_IMAGE_IMAGE_HPP
# warning "You should not include this file directly but <mln/core/image/image.hpp> instead"
# include <mln/core/image/image.hpp>
#endif

#ifndef MLN_CORE_IMAGE_IMAGE_EXPR_HPP
# define MLN_CORE_IMAGE_IMAGE_EXPR_HPP

# include <type_traits>
# include <mln/core/image/morphers/zip_image.hpp>
# include <mln/core/image/morphers/transformed_image.hpp>

namespace mln
{

  namespace internal
  {
    template <typename Function, class... ArgType>
    struct func_call_from_tupleargs;
  }

  template <typename UnaryFunction, typename Image>
  using unary_image_expr = internal::transformed_image<Image, UnaryFunction, false>;

  template <typename BinaryFunction, typename Image1, typename Image2>
  using binary_image_expr = internal::transformed_image<
    zip_image<Image1, Image2>,
    internal::func_call_from_tupleargs<BinaryFunction, mln_reference(Image1), mln_reference(Image2)>,
    false>;

  template <typename BinaryFunction, typename Image, typename Scalar>
  using binary_image_scalar_expr = internal::transformed_image<
    Image,
    decltype( std::bind(std::declval<const BinaryFunction&>(), std::placeholders::_1, std::declval<const Scalar&>()) ),
    false>;

  template <typename BinaryFunction, typename Scalar, typename Image>
  using binary_scalar_image_expr = internal::transformed_image<
    Image,
    decltype( std::bind(std::declval<const BinaryFunction&>(), std::declval<const Scalar&>(), std::placeholders::_1) ),
    false>;

  template <typename TernaryFunction, typename Image1, typename Image2, typename Image3>
  using ternary_image_image_image_expr = internal::transformed_image<
    zip_image<Image1, Image2, Image3>,
    internal::func_call_from_tupleargs<TernaryFunction,
                                       mln_reference(Image1),
                                       mln_reference(Image2),
                                       mln_reference(Image3)>,
    false>;

  template <typename TernaryFunction, typename Image1, typename Image2, typename Scalar>
  using ternary_image_image_scalar_expr = internal::transformed_image<
    zip_image<Image1, Image2>,
    internal::func_call_from_tupleargs<
      decltype( std::bind(std::declval<const TernaryFunction&>(), std::placeholders::_1, std::placeholders::_2, std::declval<const Scalar&>()) ),
      mln_reference(Image1),
      mln_reference(Image2)>,
    false>;

  template <typename TernaryFunction, typename Image1, typename Scalar, typename Image2>
  using ternary_image_scalar_image_expr = internal::transformed_image<
    zip_image<Image1, Image2>,
    internal::func_call_from_tupleargs<
      decltype( std::bind(std::declval<const TernaryFunction&>(), std::placeholders::_1, std::declval<const Scalar&>(), std::placeholders::_2) ),
      mln_reference(Image1),
      mln_reference(Image2)>,
    false>;

  template <typename TernaryFunction, typename Image1, typename Scalar1, typename Scalar2>
  using ternary_image_scalar_scalar_expr = internal::transformed_image<
    Image1,
    decltype( std::bind(std::declval<const TernaryFunction&>(), std::placeholders::_1, std::declval<const Scalar1&>(), std::declval<const Scalar2&>()) ),
    false>;



  // template <typename UnaryFunction, typename I>
  // unary_image_expr<UnaryFunction, I>
  // make_unary_image_expr(I&& ima, UnaryFunction f);




  /*
  template <typename I>
  I& eval(Image<I>& ima);

  template <typename I>
  const I& eval(const Image<I>& ima);

  template <typename Expr>
  typename eval_type<Expr>::type
  eval(const image_expr<Expr>& ima)
  */


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace internal
  {

    template <typename Function, class... ArgType>
    struct func_call_from_tupleargs
    {
      func_call_from_tupleargs() = default;

      func_call_from_tupleargs(const Function& f_)
      : f(f_)
      {
      }

      // template <class TTuple, int... N>
      // inline
      // typename std::result_of<const Function(ArgType...)>::type
      // call(const TTuple& x, intseq<N...>) const
      // {
      //   return f(static_cast<typename std::tuple_element<N, TTuple>::type>(std::get<N>(x))...);
      // }

      // inline
      // typename std::result_of<const Function(ArgType...)>::type
      // operator() (const std::tuple<ArgType...>& x) const
      // {
      //   return call(x, typename int_list_seq<sizeof...(ArgType)>::type ());
      // }

      template <class TTuple, std::size_t... N>
      inline
      typename std::result_of<const Function(ArgType&&...)>::type
      call(TTuple&& x, std::index_sequence<N...>) const
      {
        return f(std::get<N>(std::move(x))...);
      }

      inline
      typename std::result_of<const Function(ArgType&&...)>::type
      operator() (std::tuple<ArgType...>&& x) const
      {
        return call(std::move(x), std::make_index_sequence<sizeof...(ArgType)> ());
      }


    private:
      Function f;
    };

  }


  /******************************************/
  /****         Helper functions         ****/
  /******************************************/

  template <typename UnaryFunction, typename I>
  unary_image_expr<UnaryFunction, I>
  make_unary_image_expr(I&& ima, const UnaryFunction& f)
  {
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I>::type>));
    return unary_image_expr<UnaryFunction, I>(std::forward<I>(ima), f);
  }

  template <typename BinaryFunction, typename I1, typename I2>
  binary_image_expr<BinaryFunction, I1, I2>
  make_binary_image_expr(I1&& ima1, I2&& ima2, const BinaryFunction& f)
  {
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I1>::type>));
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I2>::type>));
    auto z = imzip(std::forward<I1>(ima1), std::forward<I2>(ima2));
    internal::func_call_from_tupleargs<BinaryFunction, mln_reference(I1), mln_reference(I2)> fun(f);
    return binary_image_expr<BinaryFunction, I1, I2>(std::move(z), fun);
  }


  template <typename BinaryFunction, typename I, typename Scalar>
  binary_image_scalar_expr<BinaryFunction, I, Scalar>
  make_binary_image_scalar_expr(I&& ima, const Scalar& x, const BinaryFunction& f)
  {
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I>::type>));
    auto fun = std::bind(f, std::placeholders::_1, x);
    return binary_image_scalar_expr<BinaryFunction, I, Scalar>(std::forward<I>(ima), fun);
  }

  template <typename BinaryFunction, typename Scalar, typename I>
  binary_scalar_image_expr<BinaryFunction, Scalar, I>
  make_binary_scalar_image_expr(const Scalar& x, I&& ima, const BinaryFunction& f)
  {
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I>::type>));
    auto fun = std::bind(f, x, std::placeholders::_1);
    return binary_scalar_image_expr<BinaryFunction, Scalar, I>(std::forward<I>(ima), fun);
  }

  template <typename TernaryFunction, typename I1, typename I2, typename I3>
  ternary_image_image_image_expr<TernaryFunction, I1, I2, I3>
  make_ternary_image_image_image_expr(I1&& ima1, I2&& ima2, I3&& ima3, const TernaryFunction& f)
  {
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I1>::type>));
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I2>::type>));
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I3>::type>));
    auto z = imzip(std::forward<I1>(ima1), std::forward<I2>(ima2), std::forward<I3>(ima3));
    internal::func_call_from_tupleargs<TernaryFunction, mln_reference(I1), mln_reference(I2), mln_reference(I3)> fun(f);
    return ternary_image_image_image_expr<TernaryFunction, I1, I2, I3>(std::move(z), fun);
  }

  template <typename TernaryFunction, typename I1, typename I2, typename S>
  ternary_image_image_scalar_expr<TernaryFunction, I1, I2, S>
  make_ternary_image_image_scalar_expr(I1&& ima1, I2&& ima2, const S& s, const TernaryFunction& f)
  {
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I1>::type>));
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I2>::type>));
    auto z = imzip(std::forward<I1>(ima1), std::forward<I2>(ima2));
    auto f2 = std::bind(f, std::placeholders::_1, std::placeholders::_2, s);
    internal::func_call_from_tupleargs<decltype(f2), mln_reference(I1), mln_reference(I2)> fun(f2);
    return ternary_image_image_scalar_expr<TernaryFunction, I1, I2, S>(std::move(z), fun);
  }

  template <typename TernaryFunction, typename I1, typename S, typename I2>
  ternary_image_scalar_image_expr<TernaryFunction, I1, S, I2>
  make_ternary_image_scalar_image_expr(I1&& ima1, const S& s, I2&& ima2, const TernaryFunction& f)
  {
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I1>::type>));
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I2>::type>));
    auto z = imzip(std::forward<I1>(ima1), std::forward<I2>(ima2));
    auto f2 = std::bind(f, std::placeholders::_1, s, std::placeholders::_2);
    internal::func_call_from_tupleargs<decltype(f2), mln_reference(I1), mln_reference(I2)> fun(f2);
    return ternary_image_scalar_image_expr<TernaryFunction, I1, S, I2>(std::move(z), fun);
  }

  template <typename TernaryFunction, typename I1, typename S1, typename S2>
  ternary_image_scalar_scalar_expr<TernaryFunction, I1, S1, S2>
  make_ternary_image_scalar_scalar_expr(I1&& ima1, const S1& s1, const S2& s2, const TernaryFunction& f)
  {
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I1>::type>));
    auto fun = std::bind(f, std::placeholders::_1, s1, s2);
    return ternary_image_scalar_scalar_expr<TernaryFunction, I1, S1, S2>(std::forward<I1>(ima1), fun);
  }


}

#endif // !MLN_CORE_IMAGE_IMAGE_EXPR_HPP
