#ifndef MLN_NDIMAGE_RANGE_HH
# define MLN_NDIMAGE_RANGE_HH

# include <mln/core/image/internal/nested_loop_iterator.hpp>
# include <mln/core/image/ndimage_pixel.hpp>


namespace mln
{

  template <typename Image, typename Value>
  struct ndimage_value_range
  {
  private:
    enum { ndim = Image::point_type::ndim };
    typedef typename exact_type<Image>::type E;
    typedef ndimage_pixel<Value, Image::ndim, E> pixel_t;

  public:
  typedef internal::nested_loop_iterator<
    internal::origin_point_visitor_forward< typename Image::point_type >,
    internal::strided_pointer_value_visitor<ndim, true>,
    pixel_t, internal::deref_return_value_policy> iterator;


  typedef internal::nested_loop_iterator<
    internal::origin_point_visitor_backward< typename Image::point_type >,
    internal::strided_pointer_value_visitor<ndim, false>,
    pixel_t, internal::deref_return_value_policy> reverse_iterator;

    ndimage_value_range(Image& ima)
    : ima_(&ima)
    {
    }

    iterator iter() const
    {
      typename Image::point_type pmin = ima_->domain().pmin;
      typename Image::point_type pmax = ima_->domain().pmax;
      return iterator(pixel_t(exact(ima_)),
		      internal::make_point_visitor_forward(pmax - pmin),
		      internal::strided_pointer_value_visitor<ndim, true>(ima_->ptr_, ima_->strides()));
    }

    reverse_iterator riter() const
    {
      typename Image::point_type pmin = ima_->domain().pmin;
      typename Image::point_type pmax = ima_->domain().pmax;
      return iterator(pixel_t(exact(ima_)),
		      internal::make_point_visitor_backward(pmax - pmin),
		      internal::strided_pointer_value_visitor<ndim, false>(ima_->last_, ima_->strides()));
    }

  private:
    Image* ima_;
  };


  template <typename Image, typename Value>
  struct ndimage_pixel_range
  {
  private:
    enum { ndim = Image::point_type::ndim };
    typedef typename exact_type<Image>::type E;
    typedef ndimage_pixel<Value, Image::ndim, E> pixel_t;

  public:
  typedef internal::nested_loop_iterator<
    internal::domain_point_visitor_forward< typename Image::point_type >,
    internal::strided_pointer_value_visitor<ndim, true>,
    pixel_t, internal::deref_return_structure_policy> iterator;


  typedef internal::nested_loop_iterator<
    internal::domain_point_visitor_backward< typename Image::point_type >,
    internal::strided_pointer_value_visitor<ndim, false>,
    pixel_t, internal::deref_return_structure_policy> reverse_iterator;

    ndimage_pixel_range(Image& ima)
    : ima_(&ima)
    {
    }

    iterator iter() const
    {
      typename Image::point_type pmin = ima_->domain().pmin;
      typename Image::point_type pmax = ima_->domain().pmax;
      return iterator(pixel_t(exact(ima_)),
		      internal::make_point_visitor_forward(pmin, pmax),
		      internal::strided_pointer_value_visitor<ndim, true>(ima_->ptr_, ima_->strides()));
    }

    reverse_iterator riter() const
    {
      typename Image::point_type pmin = ima_->domain().pmin;
      typename Image::point_type pmax = ima_->domain().pmax;
      return iterator(pixel_t(exact(ima_)),
		      internal::make_point_visitor_backward(pmin, pmax),
		      internal::strided_pointer_value_visitor<ndim, false>(ima_->last_, ima_->strides()));
    }

  private:
    Image* ima_;
  };
}
#endif
