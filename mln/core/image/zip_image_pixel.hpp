#ifndef MLN_CORE_IMAGE_ZIP_IMAGE_PIXEL_HPP
# define MLN_CORE_IMAGE_ZIP_IMAGE_PIXEL_HPP

# include <type_traits>
# include <algorithm>
# include <array>

# include <boost/tuple/tuple.hpp>

namespace mln {


  // The type of pixel if all images are raw.
  template <typename PointerTuple, typename Point, typename ZipImage = void>
  struct zip_raw_pixel;

  namespace internal
  {
    // The type of pixel when zip_pixel is unzipped
    template <size_t n, typename TuplePixel>
    struct unzip_pixel_proxy;
  }



/******************************************/
/****          Implementation          ****/
/******************************************/

  namespace internal
  {
    using namespace boost::detail::tuple_impl_specific;

    template <size_t nelems, size_t dim, bool forward>
    struct strided_tuple_pointer_value_visitor
    {
      typedef char* ptr_t;
      typedef std::array<ptr_t, nelems> arg_t;
      enum { ndim = dim };

      strided_tuple_pointer_value_visitor()
      {
      }

      strided_tuple_pointer_value_visitor(const std::array<ptr_t, nelems>&		start,
					  const std::array<const size_t*, nelems>&	strides)
	: start_ (start)
      {
	 for (unsigned i = 0; i < nelems; ++i)
	   std::copy(strides[i], strides[i] + ndim, strides_[i].begin());
      }

      void initialize(arg_t& x)
      {
	x = start_;
	for (unsigned i = 0; i < nelems; ++i)
	  stack_[i].fill(start_[i]);
      }

      template <size_t n>
      typename std::enable_if<(n < ndim-1)>::type
      init (arg_t& )
      {
	for (unsigned i = 0; i < nelems; ++i)
	  stack_[i][n] = stack_[i][n-1];
      }

      template <size_t n>
      typename std::enable_if<(n == ndim-1)>::type
      init (arg_t& x)
      {
	for (unsigned i = 0; i < nelems; ++i)
	  x[i] = (stack_[i][n] = stack_[i][n-1]);
      }

      template <size_t n>
      typename std::enable_if<(!forward and n < ndim-1)>::type
      next (arg_t&)
      {
	for (unsigned i = 0; i < nelems; ++i)
	  stack_[i][n] -= strides_[i][n];
      }

      template <size_t n>
      typename std::enable_if<(forward and n < ndim-1)>::type
      next (arg_t&)
      {
	for (unsigned i = 0; i < nelems; ++i)
	  stack_[i][n] += strides_[i][n];
      }

	template <size_t n>
	typename std::enable_if<(!forward and n == ndim-1)>::type
	next (arg_t& x)
      {
	for (unsigned i = 0; i < nelems; ++i)
	  x[i] = (stack_[i][n] -= strides_[i][n]);
      }

      template <size_t n>
      typename std::enable_if<(forward and n == ndim-1), void>::type
      next (arg_t& x)
      {
	for (unsigned i = 0; i < nelems; ++i)
	  x[i] = (stack_[i][n] += strides_[i][n]);
      }

    private:
      std::array<ptr_t, nelems>			     start_;
      std::array< std::array<size_t, ndim>, nelems > strides_;
      std::array< std::array<ptr_t,  ndim>, nelems > stack_;
    };

    struct ptr_2_ref
    {
      template <typename Ptr> struct apply;
      template <typename T> struct apply<T*> { typedef T& type; };

      template <typename T>
      T& operator () (T* x) { return (*x); }
    };

  }

  namespace internal
  {

    template <typename ZipImage>
    struct zip_raw_pixel_base
    {
      typedef ZipImage image_type;

      zip_raw_pixel_base(ZipImage* ima = NULL)
        : ima_ (ima)
      {
      }

      image_type& image() const
      {
        mln_precondition(ima_ != NULL);
        return *ima_;
      }

    protected:
      ZipImage* ima_;
    };

    template <>
    struct zip_raw_pixel_base<void>
    {
      zip_raw_pixel_base(void* = NULL)
      {
      }
    };

  }

  template <typename PointerTuple, typename Point, typename ZipImage>
  struct zip_raw_pixel : internal::zip_raw_pixel_base<ZipImage>
  {
    enum { nelems = boost::tuples::length<PointerTuple>::value };
    typedef char* ptr_t;
    typedef PointerTuple ptr_tuple_t;
    typedef typename internal::tuple_meta_transform<PointerTuple, internal::ptr_2_ref>::type ref_tuple_t;

    typedef Point point_type;
    typedef ref_tuple_t value_type;
    typedef ref_tuple_t reference;
    typedef internal::zip_raw_pixel_base<ZipImage> base;

    zip_raw_pixel()
    {
    };

    // construction from non-const pixel
    template <typename PointerTuple2, typename ZipImage2>
    zip_raw_pixel(const zip_raw_pixel<PointerTuple2, Point, ZipImage2>& other,
                  typename std::enable_if< std::is_convertible<ZipImage2*, ZipImage*>::value >::type* = NULL)
      : base (other.ima_),
        p_ (other.p_),
        x_ (other.x_)
    {
    };


    zip_raw_pixel(Point p, const std::array<ptr_t, nelems>& x, ZipImage* ima = nullptr)
      : base (ima),
        p_ (p),
        x_ (x)
    {
    }

    reference val() const
    {
      ptr_tuple_t x;
      this->val_<nelems-1>(x);
      return internal::tuple_transform(x, internal::ptr_2_ref());
    }

    const point_type& point() const { return p_; }


    bool
    equal(const zip_raw_pixel& other) const
    {
      return p_ == other.p_;
    }

    std::array<ptr_t, nelems>&		get_value()	{ return x_; }
    const std::array<ptr_t, nelems>&	get_value() const { return x_; }
    Point&				get_point() { return p_; }
    const Point&			get_point() const { return p_; }

  private:
    template <typename, typename, typename>
    friend struct zip_raw_pixel;

    template <size_t n>
    typename std::enable_if<(n == 0), void>::type
    val_(ptr_tuple_t& t) const
    {
      boost::get<0>(t) = reinterpret_cast<typename boost::tuples::element<0, PointerTuple>::type> ( x_[0] );
    }

    template <size_t n>
    typename std::enable_if<(n > 0), void>::type
    val_(ptr_tuple_t& t) const
    {
      boost::get<n>(t) = reinterpret_cast<typename boost::tuples::element<n, PointerTuple>::type> ( x_[n] );
      this->val_<n-1> (t);
    }

    ZipImage* ima_;
    Point p_;
    std::array<ptr_t, nelems> x_;
  };


  namespace internal
  {

    template <size_t n, typename TuplePixel>
    struct unzip_pixel_proxy
    {
    private:
      typedef typename std::decay<TuplePixel>::type pixel_t;

    public:
      typedef typename pixel_t::point_type                                  point_type;
      typedef typename boost::tuples::element<n, typename pixel_t::value_type>::type value_type;
      typedef typename boost::tuples::element<n, typename pixel_t::value_type>::type reference;
      typedef typename std::remove_reference<typename boost::tuples::element<n, typename pixel_t::image_type::image_tuple_t>::type>::type image_type;

      unzip_pixel_proxy(const pixel_t& pixel)
        : tuple_pix_ (pixel)
      {
      }

      reference val() const { return boost::get<n>(tuple_pix_.val()); }
      point_type point() const { return tuple_pix_.point(); }
      image_type& image() const { return boost::get<n>(tuple_pix_.image().images()); }

    private:
      const pixel_t& tuple_pix_;
    };

  }




} // end of namespace mln


#endif //!MLN_CORE_IMAGE_ZIP_IMAGE_PIXEL_HPP
