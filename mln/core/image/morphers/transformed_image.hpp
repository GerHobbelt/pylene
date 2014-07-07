// Transformed image is core morpher, include image.hpp to ensure dependencies
#include <mln/core/image/image.hpp>

#ifndef MLN_CORE_IMAGE_MORPHERS_TRANSFORMED_IMAGE_HPP
# define MLN_CORE_IMAGE_MORPHERS_TRANSFORMED_IMAGE_HPP

// FIXME: pixel do not preserve properties

# include <mln/core/range/transform.hpp>
# include <mln/core/pixel_utility.hpp>

namespace mln
{
  // Exposition only
  namespace internal
  {
    template <typename I, class UnaryFunction, bool use_pix = use_pix_helper<I, UnaryFunction>::type::value >
    struct transformed_image;
  }
  // End

  template <typename I, class UnaryFunction>
  using transformed_image = internal::transformed_image<I, UnaryFunction>;

  template <typename I, class UnaryFunction>
  transformed_image<I, UnaryFunction>
  imtransform(Image<I>&& ima, const UnaryFunction& f);

  template <typename I, class UnaryFunction>
  transformed_image<const I&, UnaryFunction>
  imtransform(const Image<I>& ima, const UnaryFunction& f);

  template <typename I, class UnaryFunction>
  transformed_image<I&, UnaryFunction>
  imtransform(Image<I>& ima, const UnaryFunction& f);
}

  /******************************************/
  /****           HELPER MACROS          ****/
  /******************************************/

# define  MLN_INTERNAL_IMAGE_LVALUE_OPERATOR(NAME, TYPE, F)             \
  namespace mln { namespace internal {                                  \
                                                                        \
  template <typename I>                                                 \
  transformed_image<I, BOOST_PP_TUPLE_REM () F, false>                  \
  NAME##_helper(I&& ima, BOOST_PP_TUPLE_REM () TYPE *)                  \
  {                                                                     \
    BOOST_PP_TUPLE_REM () F f;                                          \
    return transformed_image<I, BOOST_PP_TUPLE_REM () F, false>(std::forward<I>(ima), f); \
  }                                                                     \
  } }

# define  MLN_DECLARE_IMAGE_LVALUE_OPERATOR_OVERLOAD(NAME, TYPE, F_CONSTREF, F_REF) \
  MLN_INTERNAL_IMAGE_LVALUE_OPERATOR(NAME, TYPE, F_REF);                \
  MLN_INTERNAL_IMAGE_LVALUE_OPERATOR(NAME##_const, TYPE, F_CONSTREF);

# define  MLN_DECLARE_IMAGE_LVALUE_OPERATOR(NAME, TYPE, F_CONSTREF, F_REF) \
  MLN_INTERNAL_IMAGE_LVALUE_OPERATOR(NAME, TYPE, F_REF);                \
  MLN_INTERNAL_IMAGE_LVALUE_OPERATOR(NAME##_const, TYPE, F_CONSTREF);   \
                                                                        \
  namespace mln {                                                       \
                                                                        \
  template <typename I>							\
  internal::transformed_image<I, BOOST_PP_TUPLE_REM () F_REF, false>    \
  NAME(Image<I>&& ima)                                                  \
  {									\
    return internal::NAME##_helper(move_exact<I>(ima), (mln_value(I)*)NULL); \
  }									\
									\
  template <typename I>							\
  internal::transformed_image<I&, BOOST_PP_TUPLE_REM () F_REF, false>	\
  NAME(Image<I>& ima)                                                   \
  {									\
    return internal::NAME##_helper(exact(ima), (mln_value(I)*) NULL);   \
  }									\
  									\
  template <typename I>							\
  internal::transformed_image<const I&, BOOST_PP_TUPLE_REM () F_CONSTREF, false> \
  NAME(const Image<I>& ima)                                             \
  {									\
    return internal::NAME##_const_helper(exact(ima), (mln_value(I)*) NULL); \
  }                                                                     \
                                                                        \
  }                                                                     \



  /******************************************/
  /****              Traits              ****/
  /******************************************/
namespace mln
{

  template <typename I, class UnaryFunction, bool b>
  struct image_traits< internal::transformed_image<I, UnaryFunction, b> >
  {
  private:
    typedef typename std::remove_reference<I>::type image_t;

  public:
    typedef typename std::common_type<typename image_traits<image_t>::category,
                                      random_access_image_tag>::type category;
    typedef std::false_type				 concrete;
    typedef typename image_traits<image_t>::accessible   accessible;
    typedef typename image_traits<image_t>::indexable    indexable;
    typedef mln::extension::none_extension_tag		 extension; // FIXME
  };

  template <class I, class UnaryFunction, bool b>
  struct image_concrete< internal::transformed_image<I, UnaryFunction, b> >
  {
    typedef typename std::result_of<UnaryFunction(mln_reference(I))>::type __vtype;
    typedef typename std::decay<__vtype>::type                             value_type;

    typedef mln_ch_value(typename std::remove_reference<I>::type, value_type) type;
  };

  namespace internal
  {
    template <class I, class UnaryFunction, bool b>
    struct image_init_from< transformed_image<I, UnaryFunction, b> >
    {
      typedef typename image_init_from<typename std::decay<I>::type>::type type;
    };
  }

  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace internal
  {

    // With a value transformation: V -> W
    template <typename I, class UnaryFunction>
    struct transformed_image<I, UnaryFunction, false>
      : morpher_base< transformed_image<I, UnaryFunction, false>,
                      I, mln_point(I),
                      typename std::remove_reference
                      <typename std::result_of<UnaryFunction(mln_reference(I))>::type>::type >
    {
    private:
      typedef typename std::remove_reference<I>::type image_t;
      typedef transformed_image<I, UnaryFunction, false> this_t;
      friend struct mln::morpher_core_access;


    public:
      typedef typename std::result_of<UnaryFunction(mln_reference(I))>::type	reference;
      typedef typename std::decay<reference>::type				value_type;
      typedef typename std::conditional<
        std::is_reference<reference>::value,
        typename std::add_lvalue_reference<typename std::add_const<value_type>::type>::type,
        reference>::type							const_reference;

      //typedef typename std::result_of<UnaryFunction(mln_creference(I))>::type	const_reference;

      struct pixel_type;
      struct const_pixel_type;


    private:
      struct val_fun_t {
        reference operator() (mln_reference(I) v) const {
          return m_fun(v);
        }
        UnaryFunction m_fun;
      };

      struct const_val_fun_t {
        const_val_fun_t(const UnaryFunction& fun) : m_fun(fun)
        {
        }

        const_val_fun_t(const val_fun_t& other) : m_fun (other.m_fun)
        {
        }

        const_reference operator() (mln_reference(I) v) const {
          return m_fun(v);
        }
        UnaryFunction m_fun;
      };

      struct pix_fun_t {
        pixel_type operator() (const mln_pixel(I)& px) const{
          return pixel_type(px, m_ima);
        }
        this_t* m_ima;
      };

      struct const_pix_fun_t {
        const_pix_fun_t(const this_t* ima)
        : m_ima(ima)
        {
        }

        const_pix_fun_t(const pix_fun_t& other)
        : m_ima (other.m_ima)
        {
        }

        const_pixel_type operator() (const mln_pixel(I)& px) const {
          return const_pixel_type(px, m_ima);
        }
        const this_t* m_ima;
      };


    public:
      // Ranges
      // \{
      typedef transformed_range<typename image_value_range<image_t>::type, val_fun_t>		value_range;
      typedef transformed_range<typename image_value_range<image_t>::type, const_val_fun_t>	const_value_range;
      typedef transformed_range<typename image_pixel_range<image_t>::type, pix_fun_t>		pixel_range;
      typedef transformed_range<typename image_pixel_range<image_t>::type, const_pix_fun_t>	const_pixel_range;
      // \}

      transformed_image() = default;

      transformed_image(I&& ima, const UnaryFunction& f)
        : m_ima(std::forward<I>(ima)),
          m_fun(f)
      {
      }


      friend
      internal::initializer<mln_concrete(transformed_image),
                            typename internal::image_init_from<transformed_image>::type>
      imconcretize(const transformed_image& f)
      {
        using mln::imchvalue;
        return std::move(imchvalue<value_type>(f.m_ima));
      }

      template <typename V>
      friend
      internal::initializer<mln_ch_value(transformed_image, V),
                            typename internal::image_init_from<transformed_image>::type>
      imchvalue(const transformed_image& f)
      {
        using mln::imchvalue;
        return std::move(imchvalue<V>(f.m_ima));
      }


      const_value_range
      values() const
      {
        const_val_fun_t f {m_fun};
        return const_value_range(m_ima.values(), f);
      }

      value_range
      values()
      {
        val_fun_t f {m_fun};
        return value_range(m_ima.values(), f);
      }

      const_pixel_range
      pixels() const
      {
        const_pix_fun_t fun = { this };
        return const_pixel_range(m_ima.pixels(), fun);
      }

      pixel_range
      pixels()
      {
        pix_fun_t fun = { this };
        return pixel_range(m_ima.pixels(), fun);
      }



      template <typename dummy = reference>
      typename std::enable_if< image_traits<this_t>::accessible::value, dummy >::type
      operator() (const mln_point(I)& p)
      {
        mln_precondition(this->domain().has(p));
        return m_fun(m_ima(p));
      }

      template <typename dummy = const_reference>
      typename std::enable_if< image_traits<this_t>::accessible::value, dummy >::type
      operator() (const mln_point(I)& p) const
      {
        mln_precondition(this->domain().has(p));
        return m_fun(m_ima(p));
      }

      template <typename dummy = reference>
      typename std::enable_if< image_traits<this_t>::accessible::value, dummy >::type
      at (const mln_point(I)& p)
      {
        return m_fun(m_ima.at(p));
      }

      template <typename dummy = const_reference>
      typename std::enable_if< image_traits<this_t>::accessible::value, dummy >::type
      at (const mln_point(I)& p) const
      {
        return m_fun(m_ima.at(p));
      }

      template <typename dummy = pixel_type>
      typename std::enable_if< image_traits<this_t>::accessible::value, dummy >::type
      pixel (const mln_point(I)& p)
      {
        mln_precondition(this->domain().has(p));
        return pixel_type(m_ima.pixel(p), this);
      }

      template <typename dummy = const_pixel_type>
      typename std::enable_if< image_traits<this_t>::accessible::value, dummy >::type
      pixel (const mln_point(I)& p) const
      {
        mln_precondition(this->domain().has(p));
        return const_pixel_type(m_ima.pixel(p), this);
      }

      template <typename dummy = pixel_type>
      typename std::enable_if< image_traits<this_t>::accessible::value, dummy >::type
      pixel_at (const mln_point(I)& p)
      {
        return pixel_type(m_ima.pixel_at(p), this);
      }

      template <typename dummy = const_pixel_type>
      typename std::enable_if< image_traits<this_t>::accessible::value, dummy >::type
      pixel_at (const mln_point(I)& p) const
      {
        return const_pixel_type(m_ima.pixel_at(p), this);
      }


      template <typename dummy = reference, typename image_type = image_t>
      typename std::enable_if< image_traits<this_t>::indexable::value, dummy >::type
      operator[] (typename image_type::size_type i)
      {
        return m_fun(m_ima[i]);
      }

      template <typename dummy = const_reference, typename image_type = image_t>
      typename std::enable_if< image_traits<this_t>::indexable::value, dummy >::type
      operator[] (typename image_type::size_type i) const
      {
        return m_fun(m_ima[i]);
      }

    private:
      mutable I m_ima;
      UnaryFunction m_fun;
    };


    /*****************************/
    /**   Pixel definition      **/
    /*****************************/

    template <typename I, class UnaryFunction>
    struct transformed_image<I, UnaryFunction, false>::pixel_type
      : morpher_pixel_base<transformed_image<I, UnaryFunction, false>::pixel_type, mln_pixel(I)>
    {
      friend struct mln::morpher_core_access;
      friend struct const_pixel_type;
      typedef transformed_image<I, UnaryFunction, false> image_type;
      typedef typename std::result_of<UnaryFunction(mln_reference(I))>::type	reference;
      typedef typename std::remove_reference<reference>::type			value_type;

      pixel_type(const mln_pixel(I)& px, image_type* ima)
	: m_pix(px), m_ima(ima)
      {
      }

      reference val() const
      {
	return m_ima->m_fun(m_pix.val());
      }

      image_type&	image() const
      {
	return *m_ima;
      }


    private:
      mln_pixel(I)		m_pix;
      image_type*		m_ima;
    };

    template <typename I, class UnaryFunction>
    struct transformed_image<I, UnaryFunction, false>::const_pixel_type
      : morpher_pixel_base<transformed_image<I, UnaryFunction, false>::const_pixel_type, mln_pixel(I)>
    {
      friend struct mln::morpher_core_access;
      typedef const transformed_image<I, UnaryFunction, false> image_type;
      typedef transformed_image<I, UnaryFunction, false>::value_type		value_type;
      typedef transformed_image<I, UnaryFunction, false>::const_reference	reference;

      const_pixel_type(const mln_pixel(I)& px, image_type* ima)
	: m_pix(px), m_ima(ima)
      {
      }

      // Interop
      const_pixel_type(const pixel_type& other)
      : m_pix(other.m_pix), m_ima(other.m_ima)
      {
      }


      reference val() const
      {
	return m_ima->m_fun(m_pix.val());
      }

      image_type&	image() const
      {
	return *m_ima;
      }


    private:
      mln_pixel(I)	   m_pix;
      image_type*	   m_ima;
    };

  }


  template <typename I, class UnaryFunction>
  inline
  transformed_image<I, UnaryFunction>
  imtransform(Image<I>&& ima, const UnaryFunction& f)
  {
    return transformed_image<I, UnaryFunction>(move_exact<I>(ima), f);
  }

  template <typename I, class UnaryFunction>
  transformed_image<const I&, UnaryFunction>
  imtransform(const Image<I>& ima, const UnaryFunction& f)
  {
    return transformed_image<const I&, UnaryFunction>(exact(ima), f);
  }

  template <typename I, class UnaryFunction>
  transformed_image<I&, UnaryFunction>
  imtransform(Image<I>& ima, const UnaryFunction& f)
  {
    return transformed_image<I&, UnaryFunction>(exact(ima), f);
  }

} // end of namespace mln

#endif //!MLN_CORE_IMAGE_MORPHERS_TRANSFORMED_IMAGE_HPP
