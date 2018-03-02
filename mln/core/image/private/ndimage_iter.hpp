#pragma once

#include <mln/core/image/internal/nested_loop_iterator.hpp>

namespace mln
{

  namespace details
  {

    ///
    /// \tparam N Domain dimension
    /// \tparam V Value type
    /// \tparam E Exact image type
    template <int N, class V, class E>
    struct ndimage_value_range
    {
    private:
      using image_t = E;
      using pixel_t = ndimage_pixel_base<V, N>;
      using point_t = point<int, N>;

      friend struct ndimage_value_range<N, const V, const E>;

    public:
      using iterator =
          internal::nested_loop_iterator<internal::origin_point_visitor_forward<point_t>, internal::no_op_visitor,
                                         internal::strided_visitor<N, std::ptrdiff_t, 1>, pixel_t,
                                         internal::deref_return_value_policy>;

      using reverse_iterator =
          internal::nested_loop_iterator<internal::origin_point_visitor_backward<point_t>, internal::no_op_visitor,
                                         internal::strided_visitor<N, std::ptrdiff_t, -1>, pixel_t,
                                         internal::deref_return_value_policy>;

      using const_iterator = iterator;
      using const_reverse_iterator = reverse_iterator;

      ndimage_value_range(image_t* ima) : m_ima(ima) {}

      // interop
      ndimage_value_range(const ndimage_value_range<N, const V, const E>& other) : m_ima(other.m_ima) {}

      const_iterator iter() const
      {
        auto shp = m_ima->domain().shape();

        std::array<std::ptrdiff_t, N> delta_index_strides;
        delta_index_strides[N - 1] = 1;

        for (unsigned i = 0; i < N - 1; ++i)
          delta_index_strides[i] = m_ima->m_strides[i] - (m_ima->m_strides[i + 1] * shp[i + 1]);

        auto index_first = m_ima->index_of_point(m_ima->domain().pmin);

        return const_iterator(pixel_t(m_ima->m_ptr), internal::make_point_visitor_forward(shp),
                              internal::no_op_visitor(),
                              internal::strided_visitor<N, std::ptrdiff_t, 1>(index_first, delta_index_strides));
      }

      const_reverse_iterator riter() const
      {
        auto shp = m_ima->domain().shape();

        std::array<std::ptrdiff_t, N> delta_index_strides;
        delta_index_strides[N - 1] = -1;

        for (unsigned i = 0; i < N - 1; ++i)
          delta_index_strides[i] = -(m_ima->m_strides[i] - (m_ima->m_strides[i + 1] * shp[i + 1]));

        auto index_last = m_ima->index_of_point(m_ima->domain().pmax - 1);

        return const_reverse_iterator(
            pixel_t(m_ima->m_ptr), internal::make_point_visitor_backward(shp), internal::no_op_visitor(),
            internal::strided_visitor<N, std::ptrdiff_t, -1>(index_last, delta_index_strides));
      }

    private:
      image_t* m_ima;
    };

    ///
    /// \tparam N Domain dimension
    /// \tparam V Value type
    /// \tparam E Exact image type
    template <int N, class V, class E>
    struct ndimage_pixel_range
    {
    private:
      using image_t = E;
      using pixel_t = ndimage_pixel<N, V, E>;
      using point_t = point<int, N>;

      template <int M, class V2, class E2>
      friend struct ndimage_pixel_range;

    public:
      using iterator =
          internal::nested_loop_iterator<internal::domain_point_visitor_forward<point_t>, internal::no_op_visitor,
                                         internal::strided_visitor<N, std::ptrdiff_t, 1>, pixel_t,
                                         internal::deref_return_structure_policy>;

      using reverse_iterator =
          internal::nested_loop_iterator<internal::domain_point_visitor_backward<point_t>, internal::no_op_visitor,
                                         internal::strided_visitor<N, std::ptrdiff_t, -1>, pixel_t,
                                         internal::deref_return_structure_policy>;

      using const_iterator = iterator;
      using const_reverse_iterator = reverse_iterator;

      ndimage_pixel_range(E* ima) : m_ima(ima) {}

      // interop
      ndimage_pixel_range(const ndimage_pixel_range<N, const V, const E>& other) : m_ima(other.m_ima) {}

      const_iterator iter() const
      {
        auto shp = m_ima->domain().shape();

        std::array<std::ptrdiff_t, N> delta_index_strides;
        delta_index_strides[N - 1] = 1;

        for (unsigned i = 0; i < N - 1; ++i)
          delta_index_strides[i] = m_ima->m_strides[i] - (m_ima->m_strides[i + 1] * shp[i + 1]);

        auto index_first = m_ima->index_of_point(m_ima->domain().pmin);

        return const_iterator(pixel_t(m_ima, m_ima->m_ptr),
                              internal::make_point_visitor_forward(m_ima->domain().pmin, m_ima->domain().pmax),
                              internal::no_op_visitor(),
                              internal::strided_visitor<N, std::ptrdiff_t, 1>(index_first, delta_index_strides));
      }

      const_reverse_iterator riter() const
      {
        auto shp = m_ima->domain().shape();

        std::array<std::ptrdiff_t, N> delta_index_strides;
        delta_index_strides[N - 1] = -1;

        for (unsigned i = 0; i < N - 1; ++i)
          delta_index_strides[i] = -(m_ima->m_strides[i] - (m_ima->m_strides[i + 1] * shp[i + 1]));

        auto index_last = m_ima->index_of_point(m_ima->domain().pmax - 1);

        return const_reverse_iterator(pixel_t(m_ima, m_ima->m_ptr),
                                      internal::make_point_visitor_backward(m_ima->domain().pmin, m_ima->domain().pmax),
                                      internal::no_op_visitor(), internal::strided_visitor<N, std::ptrdiff_t, -1>(
                                                                     index_last, delta_index_strides));
      }

    private:
      image_t* m_ima;
    };

  }

}

