#ifndef MLN_CORE_IMAGE_ZIP_IMAGE_VALUE_ITERATOR_HPP
# define MLN_CORE_IMAGE_ZIP_IMAGE_VALUE_ITERATOR_HPP

/// \FIXME Rename this file

# include <boost/tuple/tuple.hpp>
# include <boost/iterator/zip_iterator.hpp>

# include <mln/core/image_traits.hpp>
# include <mln/core/image/internal/nested_loop_iterator.hpp>
# include <mln/core/image/zip_image_pixel.hpp>

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

    zip_image_value_range(const ImageTuple& x)
    : w (internal::tuple_transform(x, internal::get_image_value_range ()))
    {
    }

    iterator begin() const { return iterator( internal::tuple_transform(w, internal::get_image_value_iterator_begin ()) ); }
    iterator end() const { return iterator( internal::tuple_transform(w, internal::get_image_value_iterator_end ()) ); }

  private:
    ValueRangeTuple w;
  };

  template <typename category, typename ImageTuple, typename ZipImage>
  struct zip_image_pixel_range
  {
  private:
    typedef typename std::remove_reference<typename boost::tuples::element<0, ImageTuple>::type>::type first_image_t;
    typedef typename first_image_t::domain_type					domain_t;
    typedef zip_image_value_range<category, ImageTuple>			ValueRange;
    typedef typename boost::range_iterator<domain_t>::type PointIterator;
    typedef typename ValueRange::iterator ValueIterator;

  public:
    typedef mln::pixel_iterator<PointIterator, ValueIterator, ZipImage> iterator;
    typedef mln::pixel_iterator<PointIterator, ValueIterator, ZipImage> const_iterator;


    zip_image_pixel_range(const ImageTuple& x, ZipImage& ima)
      : ima_ (ima),
        domain_ (boost::get<0>(x).domain()),
        rng_ (x)
    {
    }

    iterator begin() const { return iterator(std::begin(domain_), std::begin(rng_), ima_); }
    iterator end() const { return iterator(std::end(domain_), std::end(rng_), ima_); }

  private:
    ZipImage&  ima_;
    domain_t   domain_;
    ValueRange rng_;
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
  using zip_image_raw_value_iterator = internal::nested_loop_iterator
    <internal::origin_point_visitor<Point>,
     internal::strided_tuple_pointer_value_visitor<boost::tuples::length<PointerTuple>::value, Point>,
     zip_raw_pixel<PointerTuple, Point>,
     internal::deref_return_value_policy >;

  template <typename PointerTuple, typename Point, typename ZipImage>
  using zip_image_raw_pixel_iterator = internal::nested_loop_iterator
    <internal::domain_point_visitor<Point>,
     internal::strided_tuple_pointer_value_visitor<boost::tuples::length<PointerTuple>::value, Point>,
     zip_raw_pixel<PointerTuple, Point, ZipImage>,
     internal::deref_return_structure_policy >;



  template <typename ImageTuple>
  struct zip_image_value_range<raw_image_tag, ImageTuple>
  {
    typedef typename std::decay<typename boost::tuples::element<0, ImageTuple>::type>::type::point_type Point;
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_pointer>::type PointerTuple;
    typedef char* ptr_t;
    typedef zip_image_raw_value_iterator<PointerTuple, Point> iterator;
    typedef zip_image_raw_value_iterator<PointerTuple, Point> const_iterator;
    enum { nelems = boost::tuples::length<ImageTuple>::value };


    zip_image_value_range(const ImageTuple& x)
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

      begin_ = iterator(zip_raw_pixel<PointerTuple, Point> (Point (), ptr_arr),
			internal::make_point_visitor(shp),
			internal::strided_tuple_pointer_value_visitor<nelems, Point>(ptr_arr, strides_arr, shp));

      end_ = iterator(zip_raw_pixel<PointerTuple, Point> (pmax, end_ptr_arr),
		      internal::make_point_visitor(shp),
		      internal::strided_tuple_pointer_value_visitor<nelems, Point>(end_ptr_arr, strides_arr, shp));
    }

    iterator begin() const { return begin_; }
    iterator end() const { return end_; }

  private:
    template <size_t n>
    typename std::enable_if<(n == 0), void>::type
    init_(std::array<ptr_t, nelems>& x, std::array<const size_t*, nelems>& strides, const ImageTuple& imas)
    {
      x[0] = (ptr_t) (&  *(std::begin(boost::get<0>(imas).values())) );
      strides[0] = boost::get<0>(imas).strides();
    }

    template <size_t n>
    typename std::enable_if<(n > 0), void>::type
    init_(std::array<ptr_t, nelems>& x, std::array<const size_t*, nelems>& strides, const ImageTuple& imas)
    {
      x[n] = (ptr_t) (&  *(std::begin(boost::get<n>(imas).values())) );
      strides[n] = boost::get<n>(imas).strides();
      this->init_<n-1> (x, strides, imas);
    }


  private:
    iterator begin_, end_;
  };

  template <typename ImageTuple, typename ZipImage>
  struct zip_image_pixel_range<raw_image_tag, ImageTuple, ZipImage>
  {
    typedef typename std::decay<typename boost::tuples::element<0, ImageTuple>::type>::type::point_type Point;
    typedef typename internal::tuple_meta_transform<ImageTuple, image_meta_pointer>::type PointerTuple;
    typedef char* ptr_t;
    typedef zip_image_raw_pixel_iterator<PointerTuple, Point, ZipImage> iterator;
    typedef zip_image_raw_pixel_iterator<PointerTuple, Point, ZipImage> const_iterator;
    enum { nelems = boost::tuples::length<ImageTuple>::value };


    zip_image_pixel_range(const ImageTuple& x, ZipImage& ima)
    {
      auto first = boost::get<0>(x);
      Point pmax = first.domain().pmin;
      Point shp = first.domain().shape();
      pmax[0] = first.domain().pmax[0];

      std::array<ptr_t, nelems>         ptr_arr;
      std::array<ptr_t, nelems>         end_ptr_arr;
      std::array<const size_t*, nelems> strides_arr;

      this->init_<nelems-1>(ptr_arr, strides_arr, x);
      for (int i = 0; i < nelems; ++i)
	end_ptr_arr[i] = ptr_arr[i] + shp[0] * strides_arr[i][0];

      begin_ = iterator(zip_raw_pixel<PointerTuple, Point, ZipImage> (first.domain().pmin, ptr_arr, &ima),
			internal::make_point_visitor(first.domain().pmin, first.domain().pmax),
			internal::strided_tuple_pointer_value_visitor<nelems, Point>(ptr_arr, strides_arr, shp));

      end_ = iterator(zip_raw_pixel<PointerTuple, Point, ZipImage> (pmax, end_ptr_arr, &ima),
		      internal::make_point_visitor(first.domain().pmin, first.domain().pmax),
		      internal::strided_tuple_pointer_value_visitor<nelems, Point>(end_ptr_arr, strides_arr, shp));
    }

    iterator begin() const { return begin_; }
    iterator end() const { return end_; }

  private:
    template <size_t n>
    typename std::enable_if<(n == 0), void>::type
    init_(std::array<ptr_t, nelems>& x, std::array<const size_t*, nelems>& strides, const ImageTuple& imas)
    {
      x[0] = (ptr_t) (&  *(std::begin(boost::get<0>(imas).values())) );
      strides[0] = boost::get<0>(imas).strides();
    }

    template <size_t n>
    typename std::enable_if<(n > 0), void>::type
    init_(std::array<ptr_t, nelems>& x, std::array<const size_t*, nelems>& strides, const ImageTuple& imas)
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
