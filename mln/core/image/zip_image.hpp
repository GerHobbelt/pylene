#ifndef MLN_CORE_IMAGE_BASE_HPP
# warning "This file should be included only by image_base.hpp"
# include <mln/core/image_base.hpp>
#endif

#ifndef MLN_CORE_IMAGE_ZIP_IMAGE_HPP
# define MLN_CORE_IMAGE_ZIP_IMAGE_HPP

# include <boost/mpl/and.hpp>
# include <boost/tuple/tuple.hpp>

# include <mln/core/assert.hpp>
# include <mln/core/image_base.hpp>
# include <mln/core/iterator/pixel_iterator.hpp>

# include <mln/core/image/zip_image_value_iterator.hpp>

# include <boost/iterator/zip_iterator.hpp>
# include <boost/range/iterator_range.hpp>



namespace mln {

  template <typename... Images>
  struct zip_image;

  template <typename... I>
  zip_image<I...> imzip(I&&... images);

  // template <typename... I>
  // zip_image<I...> imzip(I&... images);

}

namespace boost
{
  // template <int N, typename... Images>
  // get(const zip_image<Images...>& ima);

  // template <int N, typename... Images>
  // get(zip_image<Images...>& ima);
}

/******************************************/
/****              Traits              ****/
/******************************************/

namespace mln
{

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
    typedef typename std::common_type<typename internal::zip_image_category<ImageTuple>::type,
				      bidirectional_image_tag>::type		 category;
    typedef typename internal::zip_image_accessibility<ImageTuple>::type accessible;
  };



/******************************************/
/****          Implementation          ****/
/******************************************/

  template <typename... I>
  zip_image<I...>
  imzip(I&&... images)
  {
    return zip_image<I...> (std::forward<I>(images)...);
  }

  // template <typename... I>
  // zip_image<I...>
  // imzip(I&... images)
  // {
  //   return zip_image<I...> (images...);
  // }



  // Some helpers
  namespace internal
  {

    template <typename Point>
    struct get_image_operator_call
    {
      get_image_operator_call(const Point& p) : p_ (p) {}

      template <class Image> struct apply { typedef typename Image::reference type; };
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

    struct constify
    {
      template <typename T> struct apply { typedef const typename std::remove_reference<T>::type& type; };
      //template <typename T> struct apply<T&> { typedef const T& type; };

      template <typename T>
      const T&
      operator() (T& x) const {return static_cast<const T&> (x); }
    };

    struct meta_add_reference
    {
      template <typename T> struct apply { typedef typename std::remove_reference<T>::type&  type; };

      template <typename T>
      typename std::remove_const<T>::type &
      operator() (T& x) const {return const_cast<typename std::remove_const<T>::type&>( x ); }
    };

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
  private:
    static_assert(sizeof...(Images) > 0, "Must have at least one image as parameter.");
    typedef zip_image<Images...> E;

    BOOST_CONCEPT_ASSERT((IterableImage<E>));


    typedef boost::tuple<Images...> ImageTuple;
    typedef typename internal::tuple_meta_transform<ImageTuple, internal::meta_add_reference>::type ImageTupleRef;
    typedef typename internal::tuple_meta_transform<ImageTuple, internal::constify>::type ConstImageTupleRef;
    typedef typename boost::tuples::element<0, ImageTuple>::type Image;
    typedef typename std::decay<Image>::type VImage;
    typedef boost::tuple<Images...> images_t;

  public:
    typedef ImageTuple image_tuple_t;

    typedef typename VImage::domain_type         domain_type;
    typedef typename VImage::point_type          point_type;
    typedef typename VImage::point_type          site_type;

    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_value>::type            value_type;
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_reference>::type        reference;
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_const_reference>::type  const_reference;
    typedef value_type*										   pointer;
    typedef const value_type*									   const_pointer;

    // typedef boost::zip_iterator<typename internal::tuple_meta_transform<ImageTuple, image_meta_value_iterator>::type>         value_iterator;
    // typedef boost::zip_iterator<typename internal::tuple_meta_transform<ImageTuple, image_meta_const_value_iterator>::type>   const_value_iterator;


    // typedef boost::iterator_range<value_iterator> value_range;
    // typedef boost::iterator_range<const_value_iterator> const_value_range;

    typedef zip_image_value_range<typename internal::zip_image_category<ImageTuple>::type, ImageTupleRef>		      value_range;
    typedef zip_image_value_range<typename internal::zip_image_category<ImageTuple>::type, ConstImageTupleRef>           const_value_range;
    typedef typename value_range::iterator							 value_iterator;
    typedef typename const_value_range::iterator						 const_value_iterator;

    typedef zip_image_pixel_range<typename internal::zip_image_category<ImageTuple>::type, ImageTupleRef, E>		 pixel_range;
    typedef zip_image_pixel_range<typename internal::zip_image_category<ImageTuple>::type, ConstImageTupleRef, const E>     const_pixel_range;
    typedef typename pixel_range::iterator							 pixel_iterator;
    typedef typename const_pixel_range::iterator						 const_pixel_iterator;
    typedef typename pixel_iterator::value_type                                                  pixel_type;
    typedef typename const_pixel_iterator::value_type                                            const_pixel_type;

    /// \brief Constructor
    /// \todo Check that all domain are equals
    /// \todo Add concept checking
    explicit zip_image(Images&&... imas) :
      images_ (std::forward<Images>(imas)...)
    {
    }

    zip_image(zip_image&& other)
    : images_ (std::move(other.images_))
    {
    }

    zip_image(const zip_image& other)
    : images_ (other.images_)
    {
    }

    zip_image(zip_image& other)
    : images_ (other.images_)
    {
    }

    // zip_image(const Images&... imas) :
    //   images_ (imas...)
    // {
    // }


    const domain_type& domain() const
    {
      return boost::get<0>(images_).domain();
    }

    value_range values()
    {
      return value_range(internal::tuple_transform(images_, internal::meta_add_reference ()));
      // using namespace boost::detail::tuple_impl_specific;
      // auto w = tuple_transform(images_, internal::get_image_value_range ());
      // value_iterator begin_(tuple_transform(w, internal::get_image_value_iterator_begin ()));
      // value_iterator end_(tuple_transform(w, internal::get_image_value_iterator_end ()));
      // return value_range(begin_, end_);
    }

    const_value_range values() const
    {
      return const_value_range(internal::tuple_transform(images_, internal::constify ()));
      // using namespace boost::detail::tuple_impl_specific;
      // auto w = tuple_transform(images_, internal::get_image_value_range ());
      // const_value_iterator begin_(tuple_transform(w, internal::get_image_value_iterator_begin ()));
      // const_value_iterator end_(tuple_transform(w, internal::get_image_value_iterator_end ()));
      // return const_value_range(begin_, end_);
    }

    pixel_range pixels()
    {
      return pixel_range(internal::tuple_transform(images_, internal::meta_add_reference ()), *this);
      // auto x = this->values();
      // pixel_iterator begin_(std::begin(this->domain()), std::begin(x), *this);
      // pixel_iterator end_(std::end(this->domain()), std::end(x), *this);
      // return pixel_range(begin_, end_);
    }

    const_pixel_range pixels() const
    {
      return const_pixel_range(internal::tuple_transform(images_, internal::constify ()), *this);
      // auto x = this->values();
      // const_pixel_iterator begin_(std::begin(this->domain()), std::begin(x),*this);
      // const_pixel_iterator end_(std::end(this->domain()), std::end(x), *this);
      // return const_pixel_range(begin_, end_);
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


    boost::tuple<Images...>& images()             { return images_; }
    const boost::tuple<Images...>& images() const { return images_; }

  private:
    boost::tuple<Images...> images_;
  };




} // end of namespace mln


#endif //!MLN_CORE_IMAGE_ZIP_IMAGE_HPP
