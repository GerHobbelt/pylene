#ifndef MLN_CORE_IMAGE_ZIP_IMAGE_PIXEL_HPP
# define MLN_CORE_IMAGE_ZIP_IMAGE_PIXEL_HPP

# include <type_traits>
# include <algorithm>
# include <array>

# include <boost/tuple/tuple.hpp>
# include <mln/core/image/internal/nested_loop_iterator.hpp>

namespace mln {


  // The type of pixel if all images are raw.
  template <typename PointerTuple, typename Point, typename ZipImage = void>
  struct zip_raw_pixel;



/******************************************/
/****          Implementation          ****/
/******************************************/

  namespace internal
  {
    using namespace boost::detail::tuple_impl_specific;

    template <size_t nelems, size_t dim>
    struct strided_tuple_pointer_value_visitor
    {
      typedef char* ptr_t;
      typedef std::array<ptr_t, nelems> arg_t;
      enum { ndim = dim };

      strided_tuple_pointer_value_visitor()
      {
      }

      strided_tuple_pointer_value_visitor(const std::array<ptr_t, nelems>&				start,
					  const std::array< std::array<ptrdiff_t, ndim>, nelems>&	delta_strides)
	: m_start (start), m_delta_strides (delta_strides)
      {
      }

      void initialize(arg_t& x)
      {
	x = m_start;
      }

      template <size_t n>
      void
      next (arg_t& x)
      {
	for (unsigned i = 0; i < nelems; ++i)
	  x[i] += m_delta_strides[i][n];
      }

    private:
      std::array<ptr_t, nelems>			        m_start;
      std::array< std::array<ptrdiff_t, ndim>, nelems > m_delta_strides;
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
  struct zip_raw_pixel :
    internal::zip_raw_pixel_base<ZipImage>,
    internal::nested_loop_pixel_structure_base
  {
    enum { nelems = boost::tuples::length<PointerTuple>::value };
    typedef char* ptr_t;
    typedef PointerTuple ptr_tuple_t;
    typedef typename internal::tuple_meta_transform<PointerTuple, internal::ptr_2_ref>::type ref_tuple_t;

    typedef Point point_type;
    typedef Point site_type;
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

    point_type point() const { return p_; }
    point_type site() const { return p_; }


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


} // end of namespace mln


#endif //!MLN_CORE_IMAGE_ZIP_IMAGE_PIXEL_HPP
