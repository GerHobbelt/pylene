#pragma once

#include <mln/core/concept/pixel.hpp>
#include <mln/core/rangev3/private/multi_view_facade.hpp>
#include <mln/core/rangev3/view/reverse.hpp>
#include <mln/core/point.hpp>

namespace mln::experimental::details
{

  /// This file defines:
  ///
  /// class __ndbuffer_image<T,N>::pixel_type
  /// class __ndbuffer_image<T,N>::const_pixel_type
  /// class __ndbuffer_image<T,N>::pixel_range
  /// class __ndbuffer_image<T,N>::const_pixel_range

  template <class T, int N>
  struct ndpix : Pixel<ndpix<T, N>>
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
        this->m_lineptr += m_info->m_axes[1].stride * dp[1];
        this->m_point[0] += dp[0];
        this->m_point[1] += dp[1];
      }
      else
      {
        for (int k = 1; k < N; ++k)
          this->m_lineptr += m_info->m_axes[k].stride * dp[k];

        for (int k = 0; k < N; ++k)
          this->m_point[k] += dp[k];
      }
    }

  private:
    friend ::ranges::range_access;
    // Do not return by const& because reverse iterator does not support stashing
    // (It creates a copy and return *it)
    constexpr ndpix read() const noexcept { return *this; }

    void next() noexcept { this->m_point[0]++; }
    void prev() noexcept { this->m_point[0]--; }
    bool equal(const ndpix& other) const noexcept { return this->m_point[0] == other.m_point[0]; }

  public:
    const mln::details::ndbuffer_image_info_t* m_info;
    T*                                         m_lineptr;
    point_type                                 m_point;
  };


  template <class T, int N>
  class ndpix_range : public ranges::details::multi_view_facade<N, ndpix_range<T, N>>
  {
  public:
    struct row_t : public ndpix<T, N>, ::ranges::view_facade<row_t>
    {
      friend ::ranges::range_access;
    private:
      struct cursor_t : ndpix<T, N>
      {
        using value_type = ndpix<T, N>;
        using reference  = ndpix<T,N>;
      };

      cursor_t begin_cursor() const noexcept
      {
        cursor_t tmp   = {*this};
        tmp.m_point[0] = this->m_info->m_axes[0].domain_begin;
        return tmp;
      }
      cursor_t end_cursor() const noexcept
      {
        cursor_t tmp   = {*this};
        tmp.m_point[0] = this->m_info->m_axes[0].domain_end;
        return tmp;
      }

    public:
      row_t() = default;
      row_t(const ndpix<T, N>& px)
        : ndpix<T, N>{px}
      {
      }
    };

  public:
    struct cursor : ndpix<T, N>
    {
      using value_type = ndpix<T, N>;
      using reference  = const ndpix<T, N>&;

      const ndpix<T, N>&            __read() const noexcept { return *this; }
      const ndpix<T, N>&            __rread() const noexcept { return *this; }
      row_t                         __read_row() const noexcept { return row_t(*this); }
      ::ranges::reverse_view<row_t> __read_rrow() const noexcept { return ::ranges::view::reverse(row_t(*this)); }

      void __next(std::size_t k) noexcept
      {
        this->m_point[N - 1 - k]++;
        if (k + 2 == N)
          __update_lineptr();
      }
      void __rnext(std::size_t k) noexcept
      {
        this->m_point[N - 1 - k]--;
        if (k + 2 == N)
          __update_lineptr();
      }

      void __reset_to_begin(std::size_t k) noexcept
      {
        this->m_point[N - 1 - k] = this->m_info->m_axes[N - 1 - k].domain_begin;
        if (k + 2 == N)
          __update_lineptr();
      }

      void __reset_to_rbegin(std::size_t k) noexcept
      {
        this->m_point[N - 1 - k] = this->m_info->m_axes[N - 1 - k].domain_end - 1;
        if (k + 2 == N)
          __update_lineptr();
      }

      bool __is_at_end(std::size_t k) const noexcept
      {
        return this->m_point[N - 1 - k] == this->m_info->m_axes[N - 1 - k].domain_end;
      }
      bool __is_at_rend(std::size_t k) const noexcept
      {
        return this->m_point[N - 1 - k] == this->m_info->m_axes[N - 1 - k].domain_begin - 1;
      }

      bool __equal(const cursor& other) const noexcept
      {
        return this->m_lineptr == other.m_lineptr && this->m_point[0] == other.m_point[0];
      }

      cursor() = default;
      cursor(const ndpix_range& r, bool forward = true) noexcept
      {
        this->m_info = &r.m_info;
        for (int i = 0; i < N; ++i)
          this->m_point[i] = forward ? this->m_info->m_axes[i].domain_begin : (this->m_info->m_axes[i].domain_end - 1);
        this->__update_lineptr();
      }

    private:
      void __update_lineptr() noexcept
      {
        this->m_lineptr = reinterpret_cast<T*>(this->m_info->m_buffer);
        for (std::size_t k = 1; k < N; ++k)
          this->m_lineptr += this->m_point[k] * this->m_info->m_axes[k].stride;
      }
    };

    ndpix_range() = default;

    ndpix_range(const mln::details::ndbuffer_image_info_t* info)
      : m_info{*info}
    {
    }

    mln::details::ndbuffer_image_info_t m_info;
  };


} // namespace mln::details
