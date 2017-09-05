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
    using iterator =
        internal::nested_loop_iterator<internal::origin_point_visitor_forward<P>,
                                       internal::no_op_visitor,
                                       internal::strided_visitor<ndim, int, 1>,
                                       pixel_t, internal::deref_return_value_policy>;

    using reverse_iterator =
        internal::nested_loop_iterator<internal::origin_point_visitor_backward<P>,
                                       internal::no_op_visitor,
                                       internal::strided_visitor<ndim, int, -1>,
                                       pixel_t, internal::deref_return_value_policy>;

    using const_iterator = iterator;
    using const_reverse_iterator = reverse_iterator;

    ndimage_value_range(Image& ima) : ima_(&ima) {}

    // interop
    ndimage_value_range(const typename Image::value_range& other) : ima_(other.ima_) {}

    const_iterator iter() const
    {
      P shp = ima_->domain().shape();

      std::array<int, ndim> delta_index_strides;
      delta_index_strides[ndim - 1] = 1;

      for (unsigned i = 0; i < ndim - 1; ++i)
        delta_index_strides[i] = ima_->m_index_strides[i] - ima_->m_index_strides[i + 1] * shp[i + 1];

      return const_iterator(pixel_t(nullptr, ima_->m_ptr_origin),
                            internal::make_point_visitor_forward(shp),
                            internal::no_op_visitor(),
                            internal::strided_visitor<ndim, int, 1>(ima_->m_index_first, delta_index_strides));
    }

    const_reverse_iterator riter() const
    {
      P shp = ima_->domain().shape();

      std::array<int, ndim> delta_index_strides;
      delta_index_strides[ndim - 1] = -1;

      for (unsigned i = 0; i < ndim - 1; ++i)
        delta_index_strides[i] = -static_cast<int>(ima_->m_index_strides[i] - ima_->m_index_strides[i + 1] * shp[i + 1]);

      return const_reverse_iterator(pixel_t(nullptr, ima_->m_ptr_origin),
                                    internal::make_point_visitor_backward(shp),
                                    internal::no_op_visitor(),
                                    internal::strided_visitor<ndim, int, -1>(ima_->m_index_last, delta_index_strides));
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
    using iterator = internal::nested_loop_iterator<internal::domain_point_visitor_forward<P>,
                                                    internal::no_op_visitor,
                                                    internal::strided_visitor<ndim, int, 1>,
                                                    pixel_t,
                                                    internal::deref_return_structure_policy>;

    using reverse_iterator =
        internal::nested_loop_iterator<internal::domain_point_visitor_backward<P>,
                                       internal::no_op_visitor,
                                       internal::strided_visitor<ndim, int, 1>,
                                       pixel_t,
                                       internal::deref_return_structure_policy>;
    using const_iterator = iterator;
    using const_reverse_iterator = reverse_iterator;

    ndimage_pixel_range(Image& ima) : ima_(&ima) {}

    ndimage_pixel_range(const typename Image::pixel_range& other) : ima_(other.ima_) {}

    const_iterator iter() const
    {
      P pmin = ima_->domain().pmin;
      P pmax = ima_->domain().pmax;
      P shp = pmax - pmin;

      std::array<int, ndim> delta_index_strides;
      delta_index_strides[ndim - 1] = 1;

      for (unsigned i = 0; i < ndim - 1; ++i)
        delta_index_strides[i] = ima_->m_index_strides[i] - ima_->m_index_strides[i + 1] * shp[i + 1];

      return const_iterator(pixel_t(exact(ima_), ima_->m_ptr_origin),
                            internal::make_point_visitor_forward(pmin, pmax),
                            internal::no_op_visitor(),
                            internal::strided_visitor<ndim, int, 1>(ima_->m_index_first, delta_index_strides));
    }

    const_reverse_iterator riter() const
    {
      P pmin = ima_->domain().pmin;
      P pmax = ima_->domain().pmax;
      P shp = pmax - pmin;

      std::array<int, ndim> delta_index_strides;
      delta_index_strides[ndim - 1] = -11;

      for (unsigned i = 0; i < ndim - 1; ++i)
        delta_index_strides[i] = -static_cast<int>(ima_->m_index_strides[i] - ima_->m_index_strides[i + 1] * shp[i + 1]);

      return reverse_iterator(pixel_t(exact(ima_), ima_->m_ptr_origin),
			      internal::make_point_visitor_backward(pmin, pmax),
                              internal::no_op_visitor(),
			      internal::strided_visitor<ndim, int, -1>(ima_->m_index_last, delta_index_strides));
    }

  private:
    Image* ima_;
  };
}
#endif
