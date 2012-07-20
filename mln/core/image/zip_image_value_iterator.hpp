#ifndef MLN_CORE_IMAGE_ZIP_IMAGE_VALUE_ITERATOR_HPP
# define MLN_CORE_IMAGE_ZIP_IMAGE_VALUE_ITERATOR_HPP

# include <boost/tuple/tuple.hpp>
# include <boost/iterator/zip_iterator.hpp>

# include <mln/core/image_traits.hpp>
# include <mln/core/image/internal/nested_loop_iterator.hpp>
# include <iostream>

namespace mln
{

  template <typename category, typename ImageTuple>
  struct zip_image_value_range;

  template <typename ImageTuple>
  struct zip_image_value_range<raw_image_tag, ImageTuple>;


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace internal
  {
    using namespace boost::detail::tuple_impl_specific;


    struct get_image_value_range
    {
      template <class Image> struct apply;
      template <class Image> struct apply<Image&> { typedef typename Image::value_range type; };
      template <class Image> struct apply<const Image&> { typedef typename Image::const_value_range type; };

      template <typename Image>
      typename Image::const_value_range
      operator () (const Image& ima) const { return ima.values(); }

      template <typename Image>
      typename Image::value_range
      operator () (Image& ima) const { return ima.values(); }
    };


    struct get_image_value_iterator_begin
    {
      template <class VRange> struct apply { typedef typename VRange::iterator type; };

      template <typename VRange>
      typename VRange::iterator
      operator () (const VRange& values) const { return std::begin(values); }

    };

    struct get_image_value_iterator_end
    {
      template <class VRange> struct apply { typedef typename VRange::iterator type; };

      template <typename VRange>
      typename VRange::iterator
      operator () (const VRange& values) const { return std::end(values); }
    };

  } // end of namespace internal


  template <typename category, typename ImageTuple>
  struct zip_image_value_range
  {
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_value_range>::type ValueRangeTuple;
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_value_iterator>::type IteratorTuple;
    typedef boost::zip_iterator<IteratorTuple> iterator;
    typedef boost::zip_iterator<IteratorTuple> const_iterator;

    zip_image_value_range(ImageTuple& x)
    : w (internal::tuple_transform(x, internal::get_image_value_range ()))
    {
    }

    iterator begin() const { return iterator( internal::tuple_transform(w, internal::get_image_value_iterator_begin ()) ); }
    iterator end() const { return iterator( internal::tuple_transform(w, internal::get_image_value_iterator_end ()) ); }

  private:
    ValueRangeTuple w;
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


  namespace internal
  {
    template <size_t nelems, typename Point>
    struct strided_tuple_pointer_value_visitor
    {
      typedef char* ptr_t;
      typedef std::array<ptr_t, nelems> arg_t;
      enum { ndim = Point::ndim };

      strided_tuple_pointer_value_visitor()
      {
      }

      strided_tuple_pointer_value_visitor(const std::array<ptr_t, nelems>& start,
					  const std::array<const size_t*, nelems>& strides,
					  Point shape)
	: shp_ (shape)
      {
	 for (unsigned i = 0; i < nelems; ++i)
	   {
	     std::copy(strides[i], strides[i] + ndim, strides_arr_[i].begin());
	     stack_arr_[i].fill(start[i]);
	   }

      }

      template <size_t n>
      typename std::enable_if< (n > 0 and n < ndim-1),void>::type
      fwd_init (arg_t& )
      {
	for (unsigned i = 0; i < nelems; ++i)
	  stack_arr_[i][n] = stack_arr_[i][n-1];
      }

      template <size_t n>
      typename std::enable_if< (n > 0 and n < ndim-1),void>::type
      bwd_init (arg_t& )
      {
	for (unsigned i = 0; i < nelems; ++i)
	  stack_arr_[i][n] = stack_arr_[i][n-1] + (strides_arr_[i][n] * (shp_[n] - 1));
      }

      template <size_t n>
      typename std::enable_if<(n > 0 and n == ndim-1), void>::type
      fwd_init(arg_t& x)
      {
	for (unsigned i = 0; i < nelems; ++i)
	  x[i] = (stack_arr_[i][n] = stack_arr_[i][n-1]);
      }

      template <size_t n>
      typename std::enable_if<(n > 0 and n == ndim-1), void>::type
      bwd_init(arg_t& x)
      {
	for (unsigned i = 0; i < nelems; ++i)
	  x[i] = (stack_arr_[i][ndim-1] = (stack_arr_[i][ndim-2] + strides_arr_[i][ndim-1] * (shp_[ndim-1] - 1)));
      }

       template <size_t n>
       typename std::enable_if<(n < ndim-1), void>::type
       fwd_inc (arg_t&)
       {
	 for (unsigned i = 0; i < nelems; ++i)
	   stack_arr_[i][n] += strides_arr_[i][n];
       }

       template <size_t n>
       typename std::enable_if<(n == ndim-1), void>::type
       fwd_inc (arg_t& x)
       {
	 for (unsigned i = 0; i < nelems; ++i)
	   x[i] = (stack_arr_[i][n] += strides_arr_[i][n]);
       }

       template <size_t n>
       typename std::enable_if<(n < ndim-1), void>::type
       bwd_inc (arg_t&)
       {
	 for (unsigned i = 0; i < nelems; ++i)
	   stack_arr_[i][n] -= strides_arr_[i][n];
       }

       template <size_t n>
       typename std::enable_if<(n == ndim-1), void>::type
       bwd_inc (arg_t& x)
       {
	 for (unsigned i = 0; i < nelems; ++i)
	   x[i] = (stack_arr_[i][n] -= strides_arr_[i][n]);
       }

    private:
      Point shp_;
      std::array< std::array<size_t, ndim>, nelems > strides_arr_;
      std::array< std::array<ptr_t,  ndim>, nelems > stack_arr_;
    };

    struct ptr_2_ref
    {
      template <typename Ptr> struct apply;
      template <typename T> struct apply<T*> { typedef T& type; };

      template <typename T>
      T& operator () (T* x) { return (*x); }
    };

    template <typename PointerTuple, typename Point>
    struct zip_raw_pixel
    {
      enum { nelems = boost::tuples::length<PointerTuple>::value };
      typedef char* ptr_t;
      typedef PointerTuple ptr_tuple_t;
      typedef typename internal::tuple_meta_transform<PointerTuple, ptr_2_ref>::type ref_tuple_t;

      typedef Point point_type;
      typedef ref_tuple_t value_type;
      typedef ref_tuple_t reference;

      zip_raw_pixel()
      {
      };

      zip_raw_pixel(Point p)
      : p_(p)
      {
      }

      zip_raw_pixel(Point p, const std::array<ptr_t, nelems>& x)
	: p_ (p),
	  x_ (x)
      {
      }

      ref_tuple_t val() const
      {
	ptr_tuple_t x;
	this->val_<nelems-1>(x);
	return tuple_transform(x, ptr_2_ref());
      }

      bool
      equal(const zip_raw_pixel& other) const
      {
	return p_ == other.p_;
      }


      const Point& point() const
      {
	return p_;
      }

      std::array<ptr_t, nelems>&	get_value() { return x_; }
      Point&				get_point() { return p_; }

    private:
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

      Point p_;
      std::array<ptr_t, nelems> x_;
    };

  }

  template <typename PointerTuple, typename Point>
  using zip_image_raw_value_iterator = internal::nested_loop_iterator
			       <internal::origin_point_visitor<Point>,
				internal::strided_tuple_pointer_value_visitor<boost::tuples::length<PointerTuple>::value, Point>,
				internal::zip_raw_pixel<PointerTuple, Point>,
				internal::deref_return_value_policy >;


  template <typename ImageTuple>
  struct zip_image_value_range<raw_image_tag, ImageTuple>
  {
    typedef typename std::decay<typename boost::tuples::element<0, ImageTuple>::type>::type::point_type Point;
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_pointer>::type PointerTuple;
    typedef char* ptr_t;
    typedef zip_image_raw_value_iterator<PointerTuple, Point> iterator;
    typedef zip_image_raw_value_iterator<PointerTuple, Point> const_iterator;
    enum { nelems = boost::tuples::length<ImageTuple>::value };


    zip_image_value_range(ImageTuple& x)
    {
      auto ima = boost::get<0>(x);
      Point pmax = Point ();
      Point shp = ima.domain().shape();
      pmax[0] = shp[0];

      std::array<ptr_t, nelems>         ptr_arr;
      std::array<ptr_t, nelems>         end_ptr_arr;
      std::array<const size_t*, nelems> strides_arr;

      this->init_<nelems-1>(ptr_arr, strides_arr, x);
      for (int i = 0; i < nelems; ++i)
	end_ptr_arr[i] = ptr_arr[i] + shp[0] * strides_arr[i][0];

      begin_ = iterator(internal::zip_raw_pixel<PointerTuple, Point> (Point (), ptr_arr),
			internal::make_point_visitor(shp),
			internal::strided_tuple_pointer_value_visitor<nelems, Point>(ptr_arr, strides_arr, shp));

      end_ = iterator(internal::zip_raw_pixel<PointerTuple, Point> (pmax, end_ptr_arr),
		      internal::make_point_visitor(shp),
		      internal::strided_tuple_pointer_value_visitor<nelems, Point>(end_ptr_arr, strides_arr, shp));
    }

    iterator begin() const { return begin_; }
    iterator end() const { return end_; }

  private:
    template <size_t n>
    typename std::enable_if<(n == 0), void>::type
    init_(std::array<ptr_t, nelems>& x, std::array<const size_t*, nelems>& strides, ImageTuple& imas)
    {
      x[0] = (ptr_t) (&  *(std::begin(boost::get<0>(imas).values())) );
      strides[0] = boost::get<0>(imas).strides();
    }

    template <size_t n>
    typename std::enable_if<(n > 0), void>::type
    init_(std::array<ptr_t, nelems>& x, std::array<const size_t*, nelems>& strides, ImageTuple& imas)
    {
      x[n] = (ptr_t) (&  *(std::begin(boost::get<n>(imas).values())) );
      strides[n] = boost::get<n>(imas).strides();
      this->init_<n-1> (x, strides, imas);
    }


  private:
    iterator begin_, end_;
  };


} // end of namespace mln



#endif //!MLN_CORE_IMAGE_ZIP_IMAGE_VALUE_ITERATOR_HPP
