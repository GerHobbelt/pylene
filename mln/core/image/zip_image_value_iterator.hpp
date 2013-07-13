#ifndef MLN_CORE_IMAGE_ZIP_IMAGE_VALUE_ITERATOR_HPP
# define MLN_CORE_IMAGE_ZIP_IMAGE_VALUE_ITERATOR_HPP

/// \FIXME Rename this file

# include <boost/tuple/tuple.hpp>
# include <mln/core/iterator/zip_iterator.hpp>

# include <mln/core/image_traits.hpp>
# include <mln/core/image/internal/nested_loop_iterator.hpp>
# include <mln/core/image/zip_image_pixel.hpp>
# include <mln/core/range/range_traits.hpp>
# include <mln/core/stride_utils.hpp>

namespace mln
{

  template <typename category, typename ImageTuple>
  struct zip_image_value_range;

  template <typename ImageTuple>
  struct zip_image_value_range<raw_image_tag, ImageTuple>;

  template <typename category, typename ImageTuple, typename ZipImage>
  struct zip_image_pixel_range;

  template <typename ImageTuple, typename ZipImage>
  struct zip_image_pixel_range<raw_image_tag, ImageTuple, ZipImage>;

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace internal
  {
    using namespace boost::detail::tuple_impl_specific;


    struct get_image_value_range
    {
      template <class Image>
      struct apply { typedef typename image_value_range<typename std::remove_reference<Image>::type>::type type; };

      template <typename Image>
      typename Image::const_value_range
      operator () (const Image& ima) const { return ima.values(); }

      template <typename Image>
      typename Image::value_range
      operator () (Image& ima) const { return ima.values(); }
    };


    struct get_iterator
    {
      template <class Range> struct apply
      {
	typedef typename range_iterator< typename std::remove_reference<Range>::type >::type type;
      };

      template <typename Range>
      typename Range::const_iterator
      operator () (const Range& values) const { return values.iter(); }

      template <typename Range>
      typename Range::iterator
      operator () (Range& values) const { return values.iter(); }
    };

  } // end of namespace internal


  template <typename category, typename ImageTuple>
  struct zip_image_value_range
  {
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_value_range>::type	 ValueRangeTuple;
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_value_iterator>::type IteratorTuple;
    typedef zip_iterator<IteratorTuple> iterator;
    typedef iterator const_iterator;
    //typedef zip_iterator<const IteratorTuple> const_iterator;

    zip_image_value_range(const ImageTuple& x)
    : w (internal::tuple_transform(x, internal::get_image_value_range ()))
    {
    }

    iterator iter() const
    {
      return iterator(internal::tuple_transform(w, internal::get_iterator ()));
    }

    // const_iterator iter() const
    // {
    //   return const_iterator(internal::tuple_transform(w, internal::get_iterator ()));
    // }

  private:
    ValueRangeTuple w;
  };

  template <typename category, typename ImageTuple, typename ZipImage>
  struct zip_image_pixel_range
  {
  private:
    typedef typename std::remove_reference<typename boost::tuples::element<0, ImageTuple>::type>::type first_image_t;
    typedef typename first_image_t::domain_type			        Domain;
    typedef zip_image_value_range<category, ImageTuple>			ValueRange;
    typedef typename Domain::iterator                                   PointIterator;
    typedef typename ValueRange::iterator				ValueIterator;
    typedef typename ValueRange::const_iterator				ConstValueIterator;

  public:
    typedef mln::pixel_iterator<PointIterator, ValueIterator, ZipImage>			iterator;
    typedef mln::pixel_iterator<PointIterator, ConstValueIterator, const ZipImage>	const_iterator;


    zip_image_pixel_range(const ImageTuple& x, ZipImage& ima)
      : ima_ (ima),
        domain_ (boost::get<0>(x).domain()),
        rng_ (x)
    {
    }

    iterator iter() { return iterator(domain_.iter(), rng_.iter(), ima_); }
    const_iterator iter() const { return const_iterator(domain_.iter(), rng_.iter(), ima_); }

  private:
    ZipImage&     ima_;
    const Domain  domain_;
    ValueRange    rng_;
  };





  // template <typename category, typename ImageTuple>
  // struct zip_image_value_range<category, const ImageTuple>
  // {
  //   typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_const_value_range>::type ValueRangeTuple;
  //   typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_const_value_iterator>::type IteratorTuple;
  //   typedef boost::zip_iterator<IteratorTuple> iterator;
  //   typedef boost::zip_iterator<IteratorTuple> const_iterator;


  //   zip_image_value_range(const ImageTuple& x)
  //   : w (internal::tuple_transform(x, internal::get_image_value_range ()))
  //   {
  //   }

  //   iterator begin() const { return iterator( internal::tuple_transform(w, internal::get_image_value_iterator_begin ()) ); }
  //   iterator end() const { return iterator( internal::tuple_transform(w, internal::get_image_value_iterator_end ()) ); }

  // private:
  //   ValueRangeTuple w;
  // };

  namespace internal
  {

    // struct get_pointer_begin
    // {
    //   template <class Image> struct apply { typedef typename Image::pointer type; };
    //   template <class Image> struct apply<const Image> { typedef typename Image::const_pointer type; };

    //   template <typename Image>
    //   typename Image::pointer
    //   operator() (Image& ima) const { return &(* std::begin(ima.values())); }

    //   template <typename Image>
    //   typename Image::const_pointer
    //   operator() (Image& ima) const { return &(* std::begin(ima.values())); }
    // };

    // struct get_pointer_end
    // {
    //   template <class Image> struct apply { typedef typename Image::pointer type; };
    //   template <class Image> struct apply<const Image> { typedef typename Image::const_pointer type; };

    //   template <typename Image>
    //   typename Image::pointer
    //   operator() (Image& ima) const { return &(* std::end(ima.values())); }

    //   template <typename Image>
    //   typename Image::const_pointer
    //   operator() (const Image& ima) const { return &(* std::end(ima.values())); }
    // };


  };


  template <typename PointerTuple, typename Point>
  using zip_image_raw_value_iterator_forward = internal::nested_loop_iterator
    <internal::origin_point_visitor_forward<Point>,
     internal::strided_tuple_pointer_value_visitor<boost::tuples::length<PointerTuple>::value, Point::ndim>,
     internal::no_op_visitor,
     zip_raw_pixel<PointerTuple, Point>,
     internal::deref_return_value_policy >;

  template <typename PointerTuple, typename Point, typename ZipImage>
  using zip_image_raw_pixel_iterator_forward = internal::nested_loop_iterator
    <internal::domain_point_visitor_forward<Point>,
     internal::strided_tuple_pointer_value_visitor<boost::tuples::length<PointerTuple>::value, Point::ndim>,
     internal::no_op_visitor,
     zip_raw_pixel<PointerTuple, Point, ZipImage>,
     internal::deref_return_structure_policy >;

  template <typename PointerTuple, typename Point>
  using zip_image_raw_value_iterator_backward = internal::nested_loop_iterator
    <internal::origin_point_visitor_backward<Point>,
     internal::strided_tuple_pointer_value_visitor<boost::tuples::length<PointerTuple>::value, Point::ndim>,
     internal::no_op_visitor,
     zip_raw_pixel<PointerTuple, Point>,
     internal::deref_return_value_policy >;

  template <typename PointerTuple, typename Point, typename ZipImage>
  using zip_image_raw_pixel_iterator_backward = internal::nested_loop_iterator
    <internal::domain_point_visitor_backward<Point>,
     internal::strided_tuple_pointer_value_visitor<boost::tuples::length<PointerTuple>::value, Point::ndim>,
     internal::no_op_visitor,
     zip_raw_pixel<PointerTuple, Point, ZipImage>,
     internal::deref_return_structure_policy >;


  template <typename ImageTuple>
  struct zip_image_value_range<raw_image_tag, ImageTuple>
  {
    typedef typename std::decay<typename boost::tuples::element<0, ImageTuple>::type>::type::point_type Point;
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_pointer>::type PointerTuple;
    typedef typename internal::tuple_meta_transform<const ImageTuple, image_meta_pointer>::type ConstPointerTuple;

    typedef char* ptr_t;
    typedef zip_image_raw_value_iterator_forward<PointerTuple, Point> iterator;
    typedef zip_image_raw_value_iterator_forward<ConstPointerTuple, Point> const_iterator;
    typedef zip_image_raw_value_iterator_backward<PointerTuple, Point> reverse_iterator;
    typedef zip_image_raw_value_iterator_backward<ConstPointerTuple, Point> const_reverse_iterator;
    typedef zip_raw_pixel<PointerTuple, Point> pixel_t;
    typedef zip_raw_pixel<ConstPointerTuple, Point> const_pixel_t;

    enum { nelems = boost::tuples::length<ImageTuple>::value };
    enum { ndim = Point::ndim };

    zip_image_value_range(const ImageTuple& x)
    : imas_ (x)
    {
    }


    iterator iter() const
    {
      auto& ima = boost::get<0>(imas_);
      auto pmin = ima.domain().pmin;
      auto pmax = ima.domain().pmax;

      std::array<ptr_t, nelems>		ptr_arr;
      std::array< std::array<std::ptrdiff_t, ndim> , nelems> delta_strides_arr;
      this->init_forward<nelems-1>(ptr_arr, delta_strides_arr);

      return iterator(zip_raw_pixel<PointerTuple, Point> (Point (), ptr_arr),
		      internal::make_point_visitor_forward((Point) (pmax - pmin) ),
		      internal::strided_tuple_pointer_value_visitor<nelems, ndim>(ptr_arr, delta_strides_arr),
		      internal::no_op_visitor ());
    }

    reverse_iterator riter() const
    {
      auto& ima = boost::get<0>(imas_);
      auto pmin = ima.domain().pmin;
      auto pmax = ima.domain().pmax;

      std::array<ptr_t, nelems>		ptr_arr;
      std::array< std::array<std::ptrdiff_t, ndim> , nelems> delta_strides_arr;
      this->init_backward<nelems-1>(ptr_arr, delta_strides_arr);

      return reverse_iterator(zip_raw_pixel<PointerTuple, Point> (Point (), ptr_arr),
			      internal::make_point_visitor_backward((Point) (pmax - pmin)),
			      internal::strided_tuple_pointer_value_visitor<nelems, ndim>(ptr_arr, delta_strides_arr),
			      internal::no_op_visitor ());
    }

  private:
    template <size_t n>
    typename std::enable_if< (n > 0) >::type
    init_forward(std::array<ptr_t, nelems>& x,
		 std::array<std::array<std::ptrdiff_t, ndim>, nelems>& delta_strides) const
    {
      auto& f = boost::get<n>(imas_);
      auto shp = f.domain().shape();
      x[n] = (ptr_t) &(f(f.domain().pmin));
      compute_delta_strides<ndim>(f.strides(), &shp[0], & delta_strides[n][0]);
      if (n > 0)
	this->init_forward<n-1>(x, delta_strides);
    }

    template <size_t n>
    typename std::enable_if< (n == 0) >::type
    init_forward(std::array<ptr_t, nelems>& x,
		 std::array<std::array<std::ptrdiff_t, ndim>, nelems>& delta_strides) const
    {
      auto& f = boost::get<n>(imas_);
      auto shp = f.domain().shape();
      x[n] = (ptr_t) &(f(f.domain().pmin));
      compute_delta_strides<ndim>(f.strides(), &shp[0], & delta_strides[n][0]);
    }



    template <size_t n>
    typename std::enable_if< (n > 0) >::type
    init_backward(std::array<ptr_t, nelems>& x,
		  std::array<std::array<std::ptrdiff_t, ndim>, nelems>& delta_strides) const
    {
      auto& f = boost::get<n>(imas_);
      auto shp = f.domain().shape();
      x[n] = (ptr_t) &(f(f.domain().pmax));
      compute_negative_delta_strides<ndim>(f.strides(), &shp[0], & delta_strides[n][0]);
      if (n > 0)
	this->init_backward<n-1>(x, delta_strides);
    }

    template <size_t n>
    typename std::enable_if< (n == 0) >::type
    init_backward(std::array<ptr_t, nelems>& x,
		  std::array<std::array<std::ptrdiff_t, ndim>, nelems>& delta_strides) const
    {
      auto& f = boost::get<n>(imas_);
      auto shp = f.domain().shape();
      x[n] = (ptr_t) &(f(f.domain().pmax));
      compute_negative_delta_strides<ndim>(f.strides(), &shp[0], & delta_strides[n][0]);
    }

  private:
    ImageTuple imas_;
  };

  template <typename ImageTuple, typename ZipImage>
  struct zip_image_pixel_range<raw_image_tag, ImageTuple, ZipImage>
  {
    typedef typename std::decay<typename boost::tuples::element<0, ImageTuple>::type>::type::point_type Point;
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_pointer>::type PointerTuple;
    typedef typename internal::tuple_meta_transform<const ImageTuple, image_meta_pointer>::type ConstPointerTuple;

    typedef char* ptr_t;
    typedef zip_image_raw_pixel_iterator_forward<PointerTuple, Point, ZipImage> iterator;
    typedef zip_image_raw_pixel_iterator_backward<PointerTuple, Point, ZipImage> reverse_iterator;
    typedef iterator const_iterator;
    typedef reverse_iterator const_reverse_iterator;

    typedef zip_raw_pixel<PointerTuple, Point, ZipImage> pixel_t;
    typedef zip_raw_pixel<ConstPointerTuple, Point, ZipImage> const_pixel_t;

    enum { nelems = boost::tuples::length<ImageTuple>::value };
    enum { ndim = Point::ndim };

    zip_image_pixel_range(const ImageTuple& x, ZipImage& zip)
      : imas_ (x), zip_ (&zip)
    {
    }

    iterator iter() const
    {
      auto& ima = boost::get<0>(imas_);
      auto pmin = ima.domain().pmin;
      auto pmax = ima.domain().pmax;

      std::array<ptr_t, nelems>		ptr_arr;
      std::array< std::array<std::ptrdiff_t, ndim> , nelems> delta_strides_arr;
      this->init_forward<nelems-1>(ptr_arr, delta_strides_arr);

      return iterator(zip_raw_pixel<PointerTuple, Point, ZipImage> (Point (), ptr_arr, zip_),
		      internal::make_point_visitor_forward(pmin, pmax),
		      internal::strided_tuple_pointer_value_visitor<nelems, ndim>(ptr_arr, delta_strides_arr),
		      internal::no_op_visitor ());
    }

    reverse_iterator riter() const
    {
      auto& ima = boost::get<0>(imas_);
      auto pmin = ima.domain().pmin;
      auto pmax = ima.domain().pmax;

      std::array<ptr_t, nelems>		ptr_arr;
      std::array< std::array<std::ptrdiff_t, ndim> , nelems> delta_strides_arr;
      this->init_backward<nelems-1>(ptr_arr, delta_strides_arr);

      return reverse_iterator(zip_raw_pixel<PointerTuple, Point, ZipImage> (Point (), ptr_arr, zip_),
			      internal::make_point_visitor_backward(pmin, pmax),
			      internal::strided_tuple_pointer_value_visitor<nelems, ndim>(ptr_arr, delta_strides_arr),
			      internal::no_op_visitor ());
    }

  private:
    template <size_t n>
    typename std::enable_if< (n > 0) >::type
    init_forward(std::array<ptr_t, nelems>& x,
		 std::array<std::array<std::ptrdiff_t, ndim>, nelems>& delta_strides) const
    {
      auto& f = boost::get<n>(imas_);
      auto shp = f.domain().shape();
      x[n] = (ptr_t) &(f(f.domain().pmin));
      compute_delta_strides<ndim>(f.strides(), &shp[0], & delta_strides[n][0]);
      if (n > 0)
	this->init_forward<n-1>(x, delta_strides);
    }

    template <size_t n>
    typename std::enable_if< (n == 0) >::type
    init_forward(std::array<ptr_t, nelems>& x,
		 std::array<std::array<std::ptrdiff_t, ndim>, nelems>& delta_strides) const
    {
      auto& f = boost::get<n>(imas_);
      auto shp = f.domain().shape();
      x[n] = (ptr_t) &(f(f.domain().pmin));
      compute_delta_strides<ndim>(f.strides(), &shp[0], & delta_strides[n][0]);
    }



    template <size_t n>
    typename std::enable_if< (n > 0) >::type
    init_backward(std::array<ptr_t, nelems>& x,
		  std::array<std::array<std::ptrdiff_t, ndim>, nelems>& delta_strides) const
    {
      auto& f = boost::get<n>(imas_);
      auto shp = f.domain().shape();
      x[n] = (ptr_t) &(f(f.domain().pmax));
      compute_negative_delta_strides<ndim>(f.strides(), &shp[0], & delta_strides[n][0]);
      if (n > 0)
	this->init_backward<n-1>(x, delta_strides);
    }

    template <size_t n>
    typename std::enable_if< (n == 0) >::type
    init_backward(std::array<ptr_t, nelems>& x,
		  std::array<std::array<std::ptrdiff_t, ndim>, nelems>& delta_strides) const
    {
      auto& f = boost::get<n>(imas_);
      auto shp = f.domain().shape();
      x[n] = (ptr_t) &(f(f.domain().pmax));
      compute_negative_delta_strides<ndim>(f.strides(), &shp[0], & delta_strides[n][0]);
    }

  private:
    ImageTuple imas_;
    ZipImage*   zip_;
  };

} // end of namespace mln



#endif //!MLN_CORE_IMAGE_ZIP_IMAGE_VALUE_ITERATOR_HPP
