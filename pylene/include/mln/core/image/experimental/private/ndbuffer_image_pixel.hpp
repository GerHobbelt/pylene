#pragma once

#include <mln/core/concepts/pixel.hpp>
#include <mln/core/experimental/point.hpp>
#include <mln/core/range/private/mdrange_facade.hpp>
#include <mln/core/range/private/ndrange_facade.hpp>
#include <mln/core/utils/ptroffset.hpp>

namespace mln::experimental::details
{

  /// This file defines:
  ///
  /// class __ndbuffer_image<T,N>::pixel_type
  /// class __ndbuffer_image<T,N>::const_pixel_type
  /// class __ndbuffer_image<T,N>::pixel_range
  /// class __ndbuffer_image<T,N>::const_pixel_range

  template <class T, int N>
  struct ndpix : mln::experimental::Pixel<ndpix<T, N>>
  {
    // Note that using "int" instead of ptrdiff_t prevent clang vectorization
    using point_type = ndpoint<N, std::ptrdiff_t>;
    using value_type = std::remove_const_t<T>;
    using reference  = T&;

    constexpr reference  val() const noexcept { return m_lineptr[m_point[0]]; }
    constexpr point_type point() const noexcept { return m_point; }

    template <class P>
    void shift(const P& dp)
    {
      if constexpr (N == 2)
      {
        this->m_lineptr = ptr_offset(this->m_lineptr, m_info->m_axes[1].byte_stride * dp[1]);
        this->m_point[0] += dp[0];
        this->m_point[1] += dp[1];
      }
      else
      {
        std::ptrdiff_t offset = 0;
        for (int k = 1; k < N; ++k)
          offset += m_info->m_axes[k].byte_stride * dp[k];
        this->m_lineptr = ptr_offset(this->m_lineptr, offset);
        this->m_point += dp;
      }
    }

    void __update_lineptr() noexcept
    {
      std::ptrdiff_t offset = 0;
      for (int k = 1; k < N; ++k)
        offset += this->m_point[k] * this->m_info->m_axes[k].byte_stride;
      this->m_lineptr = reinterpret_cast<T*>(ptr_offset(this->m_info->m_buffer, offset));
    }


    bool equal(const ndpix& other) const noexcept { return this->m_point[0] == other.m_point[0]; }
    constexpr ndpix read() const noexcept { return *this; }

  public:
    const mln::details::ndbuffer_image_info_t* m_info;
    T*                                         m_lineptr;
    point_type                                 m_point;
  };


  template <class T, int N>
  class ndpix_range : public mln::ranges::details::mdview_facade<ndpix_range<T, N>>
  {
  public:
    struct cursor : mln::ranges::details::ndcursor_facade<cursor>
    {
      static constexpr int rank = N;

      struct line_cursor : ndpix<T, N>
      {
        using ndpix<T,N>::read;
        using ndpix<T,N>::equal;
        using value_type = ndpix<T,N>;
        using reference = value_type;

        void next() noexcept { this->m_point[0]++; }
      };

      line_cursor begin_cursor() const
      {
        auto px = m_pix;
        px.__update_lineptr();
        return {px};
      }

      line_cursor end_cursor() const
      {
        auto tmp = *this;
        tmp.__goto_end(0);
        tmp.m_pix.__update_lineptr();
        return {tmp.m_pix};
      }


      void __goto_end(int k) { m_pix.m_point[k] = m_pix.m_info->m_axes[k].domain_end; }
      bool __is_at_end(int k) const { return m_pix.m_point[k] == m_pix.m_info->m_axes[k].domain_end; }
      void __reset_to_begin(int k) { m_pix.m_point[k] = m_pix.m_info->m_axes[k].domain_begin; }
      void __next(int k) { m_pix.m_point[k]++; }
      bool equal(const cursor& other) const { return m_pix.m_point == other.m_pix.m_point; }
      using mln::ranges::details::ndcursor_facade<cursor>::equal;

      ndpix<T, N> m_pix;
    };

    struct backward_cursor : mln::ranges::details::ndcursor_facade<backward_cursor>
    {
      static constexpr int rank = N;

      struct line_cursor : ndpix<T, N>
      {
        using ndpix<T,N>::read;
        using ndpix<T,N>::equal;
        using value_type = ndpix<T,N>;
        using reference = value_type;

        void next() noexcept { this->m_point[0]--; }

      };

      line_cursor begin_cursor() const
      {
        auto pix = m_pix;
        pix.__update_lineptr();
        return {pix};
      }

      line_cursor end_cursor() const
      {
        auto tmp = *this;
        tmp.__goto_end(0);
        tmp.m_pix.__update_lineptr();
        return {tmp.m_pix};
      }

      void __reset_to_begin(int k) { m_pix.m_point[k] = m_pix.m_info->m_axes[k].domain_end - 1; }
      void __goto_end(int k) { m_pix.m_point[k] = m_pix.m_info->m_axes[k].domain_begin - 1; }
      void __next(int k) { m_pix.m_point[k]--; }
      bool __is_at_end(int k) const { return m_pix.m_point[k] < m_pix.m_info->m_axes[k].domain_begin; }
      bool equal(const backward_cursor& other) const { return m_pix.m_point == other.m_pix.m_point; }

      using mln::ranges::details::ndcursor_facade<backward_cursor>::equal;
      ndpix<T, N> m_pix;
    };


    cursor begin_cursor() const
    {
      ndpix<T, N> pix;
      pix.m_info = &m_info;
      pix.m_lineptr = nullptr;
      for (int i = 0; i < N; ++i)
        pix.m_point[i] = this->m_info.m_axes[i].domain_begin;
      return {{}, pix};
    }

    backward_cursor rbegin_cursor() const
    {
      ndpix<T, N> pix;
      pix.m_info = &m_info;
      pix.m_lineptr = nullptr;
      for (int i = 0; i < N; ++i)
        pix.m_point[i] = this->m_info.m_axes[i].domain_end - 1;
      return {{}, pix};
    }

    ::ranges::default_sentinel_t end_cursor() const { return {}; }
    ::ranges::default_sentinel_t rend_cursor() const { return {}; }

    ndpix_range() = default;

    ndpix_range(const mln::details::ndbuffer_image_info_t* info)
      : m_info{*info}
    {
    }

  private:
    mln::details::ndbuffer_image_info_t m_info;
};


} // namespace mln::details
