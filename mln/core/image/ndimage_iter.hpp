#ifndef MLN_NDIMAGE_RANGE_HH
# define MLN_NDIMAGE_RANGE_HH

# include <mln/core/image/internal/nested_loop_iterator.hpp>
# include <mln/core/image/ndimage_pixel.hpp>
# include <mln/core/stride_utils.hpp>

namespace mln
{

  template <typename Image, typename Value>
  struct ndimage_value_range
  {
  private:
    enum { ndim = Image::point_type::ndim };
    typedef typename exact_type<Image>::type E;
    typedef ndimage_pixel<Value, Image::ndim, E> pixel_t;

    template <typename J, typename V>
    friend struct ndimage_value_range;

    using P = point<int, Image::ndim>;
  public:

    typedef internal::nested_loop_iterator<
    internal::origin_point_visitor_forward<P>,
    internal::strided_offset_visitor<ndim, sizeof(Value)>,
    internal::no_op_visitor,
    pixel_t, internal::deref_return_value_policy> iterator;

  typedef internal::nested_loop_iterator<
    internal::origin_point_visitor_backward<P>,
    internal::strided_offset_visitor<ndim, -(int)sizeof(Value)>,
    internal::no_op_visitor,
    pixel_t, internal::deref_return_value_policy> reverse_iterator;

    typedef iterator const_iterator;
    typedef reverse_iterator const_reverse_iterator;


    ndimage_value_range(Image& ima)
    : ima_(&ima)
    {
    }

    // interop
    ndimage_value_range(const typename Image::value_range& other)
      : ima_(other.ima_)
    {
    }


    const_iterator iter() const
    {
      P shp = ima_->domain().shape();

      std::array<std::ptrdiff_t, ndim> delta_byte_strides;
      delta_byte_strides[ndim-1]  = ima_->strides_[ndim-1];

      for (unsigned i = 0; i < ndim-1; ++i)
        delta_byte_strides[i] = ima_->strides_[i] - ima_->strides_[i+1] * shp[i+1];

      return const_iterator(pixel_t(nullptr, ima_->ptr_),
			    internal::make_point_visitor_forward(shp),
			    internal::strided_offset_visitor<ndim, sizeof(Value)>(0, delta_byte_strides),
                            internal::no_op_visitor ()
			    );
    }

    const_reverse_iterator riter() const
    {
      P shp = ima_->domain().shape();

      std::array<std::ptrdiff_t, ndim> delta_byte_strides;
      delta_byte_strides[ndim-1]  = -static_cast<std::ptrdiff_t>(ima_->strides_[ndim-1]);

      for (unsigned i = 0; i < ndim-1; ++i)
        delta_byte_strides[i] = -static_cast<std::ptrdiff_t>(ima_->strides_[i] - ima_->strides_[i+1] * shp[i+1]);

      return const_reverse_iterator(pixel_t(nullptr, ima_->last_),
				    internal::make_point_visitor_backward(shp),
				    internal::strided_offset_visitor<ndim, -(int)sizeof(Value)>(0, delta_byte_strides),
                                    internal::no_op_visitor ()
				    );
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

    template <typename I, typename V>
    friend struct ndimage_pixel_range;

    using P = point<int, Image::ndim>;

  public:
    typedef internal::nested_loop_iterator<
    internal::domain_point_visitor_forward<P>,
    internal::strided_offset_visitor<ndim, sizeof(Value)>,
    internal::strided_index_visitor<ndim>,
    pixel_t, internal::deref_return_structure_policy> iterator;

    typedef iterator const_iterator;

  typedef internal::nested_loop_iterator<
    internal::domain_point_visitor_backward<P>,
    internal::strided_offset_visitor<ndim, -(int)sizeof(Value)>,
    internal::strided_index_visitor<ndim>,
    pixel_t, internal::deref_return_structure_policy> reverse_iterator;

    typedef reverse_iterator const_reverse_iterator;


    ndimage_pixel_range(Image& ima)
    : ima_(&ima)
    {
    }

    ndimage_pixel_range(const typename Image::pixel_range& other)
      : ima_(other.ima_)
    {
    }

    const_iterator iter() const
    {
      P pmin = ima_->domain().pmin;
      P pmax = ima_->domain().pmax;
      P shp = pmax - pmin;

      std::array<std::ptrdiff_t, ndim> delta_byte_strides;
      std::array<std::ptrdiff_t, ndim> delta_index_strides;

      delta_byte_strides[ndim-1]  = ima_->strides_[ndim-1];
      delta_index_strides[ndim-1]  = ima_->m_index_strides[ndim-1];

      for (unsigned i = 0; i < ndim-1; ++i) {
        delta_byte_strides[i] = ima_->strides_[i] - ima_->strides_[i+1] * shp[i+1];
        delta_index_strides[i] = ima_->m_index_strides[i] - ima_->m_index_strides[i+1] * shp[i+1];
      }

      return const_iterator(pixel_t(exact(ima_), ima_->ptr_),
                            internal::make_point_visitor_forward(pmin, pmax),
                            internal::strided_offset_visitor<ndim, sizeof(Value)>(0, delta_byte_strides),
                            internal::strided_index_visitor<ndim>(ima_->m_index_first, delta_index_strides)
                            );
    }

    const_reverse_iterator riter() const
    {
      P pmin = ima_->domain().pmin;
      P pmax = ima_->domain().pmax;
      P shp = pmax - pmin;

      std::array<std::ptrdiff_t, ndim> delta_byte_strides;
      std::array<std::ptrdiff_t, ndim> delta_index_strides;
      delta_byte_strides[ndim-1]  = -static_cast<std::ptrdiff_t>(ima_->strides_[ndim-1]);
      delta_index_strides[ndim-1]  = -static_cast<std::ptrdiff_t>(ima_->m_index_strides[ndim-1]);

      for (unsigned i = 0; i < ndim-1; ++i) {
        delta_byte_strides[i] = -static_cast<std::ptrdiff_t>(ima_->strides_[i] - ima_->strides_[i+1] * shp[i+1]);
        delta_index_strides[i] = -static_cast<std::ptrdiff_t>(ima_->m_index_strides[i] - ima_->m_index_strides[i+1] * shp[i+1]);
      }

      return reverse_iterator(pixel_t(exact(ima_), ima_->last_),
			      internal::make_point_visitor_backward(pmin, pmax),
			      internal::strided_offset_visitor<ndim, -(int)sizeof(Value)>(0, delta_byte_strides),
			      internal::strided_index_visitor<ndim>(ima_->m_index_last, delta_index_strides)
			      );
    }

  private:
    Image* ima_;
  };
}
#endif
