#ifndef MLN_CORE_IMAGE_IMAGE_EXPR_HPP
# define MLN_CORE_IMAGE_IMAGE_EXPR_HPP

# include <mln/core/functional.hpp>

# include <mln/core/concept/image.hpp>

# include <mln/core/image/zip_image.hpp>

# include <mln/core/range/iterator_range.hpp>
# include <mln/core/iterator/transform_iterator.hpp>
# include <mln/core/iterator/transform_pixel_iterator.hpp>


namespace mln
{

  template <typename Expr>
  struct image_expr;

  template <typename UnaryFunction, typename Image>
  struct unary_image_expr;

  template <typename BinaryFunction, typename Image1, typename Image2>
  struct binary_image_expr;

  template <typename BinaryFunction, typename Image, typename Scalar>
  struct binary_image_scalar_expr;

  template <typename BinaryFunction, typename Scalar, typename Image>
  struct binary_scalar_image_expr;

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
    return binary_image_expr<BinaryFunction, I1, I2>(std::forward<I1>(ima1), std::forward<I2>(ima2), f);
  }

  template <typename BinaryFunction, typename I, typename Scalar>
  binary_image_scalar_expr<BinaryFunction, I, Scalar>
  make_binary_image_scalar_expr(I&& ima, const Scalar& x, const BinaryFunction& f)
  {
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I>::type>));
    return binary_image_scalar_expr<BinaryFunction, I, Scalar>(std::forward<I>(ima), x, f);
  }

  template <typename BinaryFunction, typename Scalar, typename I>
  binary_scalar_image_expr<BinaryFunction, Scalar, I>
  make_binary_scalar_image_expr(const Scalar& x, I&& ima, const BinaryFunction& f)
  {
    BOOST_CONCEPT_ASSERT((Image<typename std::decay<I>::type>));
    return binary_scalar_image_expr<BinaryFunction, Scalar, I>(x, std::forward<I>(ima), f);
  }



  /******************************************/
  /****              Traits              ****/
  /******************************************/


  template <typename UnaryFunction, typename Image>
  struct image_traits< unary_image_expr<UnaryFunction, Image> >
  {
    typedef typename image_traits<Image>::accessible accessible;
    typedef typename std::common_type< typename image_traits<Image>::category,
                                       bidirectional_image_tag >::type category;
  };

  template <typename BinaryFunction, typename Image1, typename Image2>
  struct image_traits< binary_image_expr<BinaryFunction, Image1, Image2> >
  {
    typedef typename std::integral_constant<bool,
                                            image_traits<Image1>::accessible::value and
                                            image_traits<Image2>::accessible::value >::type accessible;
    typedef typename std::common_type< typename image_traits<Image1>::category,
                                       typename image_traits<Image2>::category,
                                       bidirectional_image_tag >::type category;
  };

  template <typename BinaryFunction, typename Image, typename Scalar>
  struct image_traits< binary_image_scalar_expr<BinaryFunction, Image, Scalar> >
  {
    typedef typename image_traits<Image>::accessible accessible;
    typedef typename std::common_type< typename image_traits<Image>::category,
                                       bidirectional_image_tag >::type category;
  };

  template <typename BinaryFunction, typename Scalar, typename Image>
  struct image_traits< binary_scalar_image_expr<BinaryFunction, Scalar, Image> >
  {
    typedef typename image_traits<Image>::accessible accessible;
    typedef typename std::common_type< typename image_traits<Image>::category,
                                       bidirectional_image_tag >::type category;
  };




  template <typename Expr>
  struct image_expr : Image<Expr>
  {
    BOOST_CONCEPT_ASSERT((IterableImage<Expr>));
  };

  namespace internal
  {



  template <typename UnaryFunction, typename Image, typename E>
  struct unary_image_expr_base : image_expr<E>
  {

  private:
    typedef E                                                           self_t;
    typedef typename std::remove_reference<Image>::type			image_t;

    typedef typename image_reference<image_t>::type                     arg_t;
    typedef typename image_const_reference<image_t>::type               const_arg_t;

    typedef typename image_value_iterator<image_t>::type                Vit;
    typedef typename image_pixel_iterator<image_t>::type                Pixit;
    typedef typename image_const_value_iterator<image_t>::type          ConstVit;
    typedef typename image_const_pixel_iterator<image_t>::type          ConstPixit;


  public:
    typedef typename image_t::point_type                                                        point_type;
    typedef typename image_t::point_type                                                        site_type;
    typedef typename image_t::domain_type                                                       domain_type;
    typedef typename std::result_of< const UnaryFunction(arg_t) >::type                         reference;
    typedef typename std::result_of< const UnaryFunction(const_arg_t) >::type                   const_reference;
    typedef typename std::remove_reference<reference>::type                                     value_type;

    typedef transform_iterator< Vit, UnaryFunction >                                            value_iterator;
    typedef transform_pixel_iterator<UnaryFunction, Pixit, E>                                     pixel_iterator;

    typedef transform_iterator< ConstVit, UnaryFunction>                                        const_value_iterator;
    typedef transform_pixel_iterator< UnaryFunction, ConstPixit, const E>                       const_pixel_iterator;

    //typedef typename const_value_iterator::reference::empty empty;
    //static_assert(std::is_const<typename std::remove_reference<typename const_value_iterator::reference>::type>::value, "");

    typedef transformed_pixel<UnaryFunction, Pixit, E>                                          pixel_type;
    typedef transformed_pixel<UnaryFunction, ConstPixit, const E>                               const_pixel_type;

    typedef iterator_range<value_iterator>                                               value_range;
    typedef iterator_range<pixel_iterator>                                               pixel_range;
    typedef iterator_range<const_value_iterator>                                         const_value_range;
    typedef iterator_range<const_pixel_iterator>                                         const_pixel_range;

    unary_image_expr_base(Image&& ima_, const UnaryFunction& f_):
      ima (std::forward<Image>(ima_)),
      f (f_)
    {
    }

    const domain_type&
    domain() const
    {
      return ima.domain();
    }

    template <typename dummy = void>
    typename std::enable_if< image_accessibility<self_t>::type::value, const_reference >::type
    operator() (point_type p) const
    {
      return f(ima(p));
    }

    template <typename dummy = void>
    typename std::enable_if< image_accessibility<self_t>::type::value, reference >::type
    operator() (point_type p)
    {
      return f(ima(p));
    }


    value_range
    values()
    {
      value_iterator x(ima.values().iter(), f);
      return make_iterator_range(x);
    }

    const_value_range
    values() const
    {
      const_value_iterator x(ima.values().iter(), f);
      return make_iterator_range(x);
    }


    pixel_range
    pixels()
    {
      pixel_iterator x(f, ima.pixels().iter(), exact(*this));
      return make_iterator_range(x);
    }

    const_pixel_range
    pixels() const
    {
      const_pixel_iterator x(f, ima.pixels().iter(), exact(*this));
      return make_iterator_range(x);
    }

  private:
    Image	  ima;
    UnaryFunction f;
  };
  }

  namespace internal
  {

    template <typename Function, typename U, typename V>
    struct wrap_tuple_to_arg
    {
      typedef typename std::result_of<Function (U, V)>::type result_type;

      wrap_tuple_to_arg() = default;


      wrap_tuple_to_arg(const Function& f_)
        : f(f_)
      {
      }

      result_type
      operator() (boost::tuple<U,V> x) const
      {
        return f(boost::get<0>(x), boost::get<1>(x));
      }

    private:
      Function f;
    };

  }


  template <typename BinaryFunction, typename Image1, typename Image2>
  struct binary_image_expr :
    internal::unary_image_expr_base< internal::wrap_tuple_to_arg< BinaryFunction,
                                                                  typename image_reference<typename std::remove_reference<Image1>::type>::type,
                                                                  typename image_reference<typename std::remove_reference<Image2>::type>::type >,
                                     zip_image<Image1, Image2>,  binary_image_expr<BinaryFunction, Image1, Image2> >
  {
    typedef internal::wrap_tuple_to_arg< BinaryFunction,
                                          typename image_reference<typename std::remove_reference<Image1>::type>::type,
                                          typename image_reference<typename std::remove_reference<Image2>::type>::type > F;
    typedef internal::unary_image_expr_base< F, zip_image<Image1, Image2>,  binary_image_expr<BinaryFunction, Image1, Image2> > base_t;

  public:
    binary_image_expr(Image1&& ima1, Image2&& ima2, const BinaryFunction& f):
      base_t( imzip(std::forward<Image1>(ima1), std::forward<Image2>(ima2)), F(f) )
    {
    }
  };


  template <typename UnaryFunction, typename Image>
  struct unary_image_expr : internal::unary_image_expr_base< UnaryFunction, Image, unary_image_expr<UnaryFunction, Image> >
  {

    unary_image_expr(Image&& ima, const UnaryFunction& f):
      internal::unary_image_expr_base< UnaryFunction, Image, unary_image_expr<UnaryFunction, Image> >(std::forward<Image>(ima), f)
    {
    }
  };


  template <typename BinaryFunction, typename Image, typename Scalar>
  struct binary_image_scalar_expr :
    internal::unary_image_expr_base< mln::binder2nd<BinaryFunction, Scalar>, Image, binary_image_scalar_expr<BinaryFunction, Image, Scalar> >
  {
  private:
    typedef internal::unary_image_expr_base< mln::binder2nd<BinaryFunction, Scalar>, Image, binary_image_scalar_expr<BinaryFunction, Image, Scalar> > base_t;

  public:
    binary_image_scalar_expr(Image&& ima, const Scalar& x, const BinaryFunction& f):
      base_t(std::forward<Image>(ima), mln::bind2nd(f, x))
    {
    }
  };


  template <typename BinaryFunction, typename Scalar, typename Image>
  struct binary_scalar_image_expr
    : internal::unary_image_expr_base< mln::binder1st<BinaryFunction, Scalar>, Image, binary_scalar_image_expr<BinaryFunction,Scalar,Image> >
  {
  private:
    typedef internal::unary_image_expr_base< mln::binder1st<BinaryFunction, Scalar>, Image, binary_scalar_image_expr<BinaryFunction, Scalar, Image> > base_t;

  public:
    binary_scalar_image_expr(const Scalar& x, Image&& ima, const BinaryFunction& f):
      base_t(std::forward<Image>(ima), mln::bind1st(f, x))
    {
    }
  };




}

#endif // !MLN_CORE_IMAGE_IMAGE_EXPR_HPP
