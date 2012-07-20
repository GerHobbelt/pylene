#ifndef MLN_CORE_IMAGE_ZIP_IMAGE_HPP
# define MLN_CORE_IMAGE_ZIP_IMAGE_HPP

# include <mln/core/assert.hpp>
# include <mln/core/image_base.hpp>
# include <mln/core/iterator/pixel_iterator.hpp>

# include <boost/iterator/zip_iterator.hpp>
# include <boost/range/iterator_range.hpp>


# include <boost/mpl/and.hpp>
# include <boost/tuple/tuple.hpp>

# include <mln/core/image/zip_image_value_iterator.hpp>

# include <boost/preprocessor/repetition/repeat.hpp>
# include <boost/preprocessor/arithmetic/div.hpp>
# include <boost/preprocessor/variadic/to_seq.hpp>
# include <boost/preprocessor/variadic/elem.hpp>
# include <boost/preprocessor/variadic/size.hpp>
# include <boost/preprocessor/seq/rest_n.hpp>
# include <boost/preprocessor/seq/enum.hpp>
# include <boost/preprocessor/seq/elem.hpp>

# define MLN_DECLARE(z, n, var)						\
  if (bool _mln_continue_##n = false) {} else				\
    for (BOOST_PP_SEQ_ELEM(n, var) = boost::get<n>(*_mln_for_cur_); !_mln_continue_##n; _mln_continue_##n = true)

# define forall_v(...)							\
  static_assert( BOOST_PP_VARIADIC_SIZE(__VA_ARGS__) > 0 and		\
		 BOOST_PP_VARIADIC_SIZE(__VA_ARGS__) % 2 == 0,		\
		 "Number of arguments of the forall macros should be odd"); \
  auto _mln_zip_image__ = imzip( BOOST_PP_SEQ_ENUM(			\
    BOOST_PP_SEQ_REST_N(BOOST_PP_DIV( BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2), \
			BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) )));	\
  auto _mln_for_range_ =  _mln_zip_image__.values();			\
  using std::begin;							\
  using std::end;							\
  auto _mln_for_cur_ = begin(_mln_for_range_);				\
  auto _mln_for_end_ = end(_mln_for_range_);				\
  for (; _mln_for_cur_ != _mln_for_end_; ++_mln_for_cur_)		\
    BOOST_PP_REPEAT(							\
      BOOST_PP_DIV( BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 2),		\
      MLN_DECLARE,							\
      BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))


namespace mln {

  template <typename... Images>
  struct zip_image;

  template <typename... I>
  zip_image<I...> imzip(Image<I>&&... images);

  // template <typename... I>
  // zip_image<I...> imzip(const Image<I>&... images);

/******************************************/
/****              Traits              ****/
/******************************************/

  namespace internal
  {
    using namespace boost::detail::tuple_impl_specific;

    template <typename ImageTuple>
    struct zip_image_accessibility
    {
      typedef typename tuple_meta_transform<ImageTuple, image_meta_accessibility>::type accessibility_tuple;
      typedef typename tuple_meta_accumulate<accessibility_tuple, boost::mpl::and_<boost::mpl::_1, boost::mpl::_2>, std::true_type>::type type;
    };

    struct meta_common_type
    {
      template <typename T1, typename T2> struct apply : std::common_type<T1, T2> {};
    };

    template <typename ImageTuple>
    struct zip_image_category
    {
      typedef typename tuple_meta_transform<ImageTuple, image_meta_category>::type category_tuple;
      typedef typename tuple_meta_accumulate<category_tuple, meta_common_type, raw_image_tag>::type type;
    };

  }


  template <typename... Images>
  struct image_traits< zip_image<Images...> >
  {
  private:
    typedef boost::tuple<Images...> ImageTuple;

  public:
    typedef typename internal::zip_image_category<ImageTuple>::type	 category;
    typedef typename internal::zip_image_accessibility<ImageTuple>::type accessible;
  };



/******************************************/
/****          Implementation          ****/
/******************************************/


  // Some helpers
  namespace internal
  {

    template <typename Point>
    struct get_image_operator_call
    {
      get_image_operator_call(const Point& p) : p_ (p) {}

      template <class Image> struct apply;
      template <class Image> struct apply<Image&> { typedef typename Image::reference type; };
      template <class Image> struct apply<const Image&> { typedef typename Image::const_reference type; };

      template <typename Image>
      typename Image::const_reference
      operator() (const Image& ima) const { return ima(p_); }

      template <typename Image>
      typename Image::reference
      operator() (Image& ima) const { return ima(p_); }

    private:
      const Point& p_;
    };
  }


  template <typename... I>
  zip_image<I...>
  imzip(I&&... images)
  {
    return zip_image<I...> (std::forward<I>(images)...);
  }



  // template <typename... I>
  // zip_image<I...>
  // imzip(const Image<I>&... images)
  // {
  //   return zip_image<I...> (exact<I>(images)...);
  // }



  template <typename... Images>
  struct zip_image : mln::Image< zip_image<Images...> >
  {
    static_assert(sizeof...(Images) > 0, "Must have at least one image as parameter.");

  private:
    typedef boost::tuple<Images...> ImageTuple;
    typedef zip_image<Images...> E;
    typedef typename boost::tuples::element<0, ImageTuple>::type Image; 
    typedef typename std::decay<Image>::type VImage;
    typedef boost::tuple<Images...> images_t;

  public:
    typedef typename VImage::domain_type         domain_type;
    typedef typename VImage::point_type          point_type;

    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_reference>::type        value_type;
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_reference>::type        reference;
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_const_reference>::type  const_reference;
    typedef value_type*										   pointer;
    typedef const value_type*									   const_pointer;

    // typedef boost::zip_iterator<typename internal::tuple_meta_transform<ImageTuple, image_meta_value_iterator>::type>         value_iterator;
    // typedef boost::zip_iterator<typename internal::tuple_meta_transform<ImageTuple, image_meta_const_value_iterator>::type>   const_value_iterator;


    // typedef boost::iterator_range<value_iterator> value_range;
    // typedef boost::iterator_range<const_value_iterator> const_value_range;

    typedef zip_image_value_range<typename image_traits<E>::category, ImageTuple>		value_range;
    typedef zip_image_value_range<typename image_traits<E>::category, const ImageTuple>		const_value_range;
    typedef typename value_range::iterator							value_iterator;
    typedef typename const_value_range::iterator						const_value_iterator;


    typedef zip_point_value_pixel<typename domain_type::iterator, value_iterator, E>             pixel_type;
    typedef zip_point_value_pixel<typename domain_type::iterator, const_value_iterator, const E> const_pixel_type;
    typedef mln::pixel_iterator<typename domain_type::iterator, value_iterator, E>               pixel_iterator;
    typedef mln::pixel_iterator<typename domain_type::iterator, const_value_iterator, const E>   const_pixel_iterator;
    typedef boost::iterator_range<pixel_iterator>                                                pixel_range;
    typedef boost::iterator_range<const_pixel_iterator>                                          const_pixel_range;


    /// \brief Constructor
    /// \todo Check that all domain are equals
    /// \todo Add concept checking
    zip_image(Images&&... imas) :
      images_ (std::forward<Images>(imas)...)
    {
    }

    const domain_type& domain() const
    {
      return boost::get<0>(images_).domain();
    }

    value_range values()
    {
      return value_range(images_);
      // using namespace boost::detail::tuple_impl_specific;
      // auto w = tuple_transform(images_, internal::get_image_value_range ());
      // value_iterator begin_(tuple_transform(w, internal::get_image_value_iterator_begin ()));
      // value_iterator end_(tuple_transform(w, internal::get_image_value_iterator_end ()));
      // return value_range(begin_, end_);
    }

    const_value_range values() const
    {
      return const_value_range(images_);
      // using namespace boost::detail::tuple_impl_specific;
      // auto w = tuple_transform(images_, internal::get_image_value_range ());
      // const_value_iterator begin_(tuple_transform(w, internal::get_image_value_iterator_begin ()));
      // const_value_iterator end_(tuple_transform(w, internal::get_image_value_iterator_end ()));
      // return const_value_range(begin_, end_);
    }

    pixel_range pixels()
    {
      auto x = this->values();
      pixel_iterator begin_(std::begin(this->domain()), std::begin(x), *this);
      pixel_iterator end_(std::end(this->domain()), std::end(x), *this);
      return pixel_range(begin_, end_);
    }

    const_pixel_range pixels() const
    {
      auto x = this->values();
      const_pixel_iterator begin_(std::begin(this->domain()), std::begin(x),*this);
      const_pixel_iterator end_(std::end(this->domain()), std::end(x), *this);
      return const_pixel_range(begin_, end_);
    }

    template <typename dummy = void>
    typename std::enable_if<image_accessibility<E>::type::value, reference>::type
    operator() (const point_type& p)
    {
      return internal::tuple_transform(images_, internal::get_image_operator_call<point_type> (p));
    }

    template <typename dummy = void>
    typename std::enable_if<image_accessibility<E>::type::value, const_reference>::type
    operator() (const point_type& p) const
    {
      return internal::tuple_transform(images_, internal::get_image_operator_call<point_type> (p));
    }

  private:
    boost::tuple<Images...> images_;
  };




} // end of namespace mln


#endif //!MLN_CORE_IMAGE_ZIP_IMAGE_HPP
