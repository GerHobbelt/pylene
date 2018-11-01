#pragma once

#include <mln/core/rangev3/private/multi_view_facade.hpp>
#include <mln/core/concept/pixel.hpp>
#include <array>

// Note: because of the CRTP, we do not want this to be inner class of ndimage<T, N, E>
// (E is a non-dependant template parameter).
// This is going to change as soon as we have better ndimage class hierarchy
namespace mln::details
{


  template <class T, std::size_t N>
  struct ndimage_info
  {
    using point_type = mln::point<std::ptrdiff_t, N>;
    std::array<std::ptrdiff_t, N> stride;   // Strides
    point_type from;                        // Start domain point
    point_type to;                          // Past-the-end domain point
    T* buffer;                              // Pointer to (0,0)
  };

  template <class T, std::size_t N>
  struct ndpix_base : Pixel<ndpix_base<T, N>>
  {
    using point_type = typename ndimage_info<T, N>::point_type;
    using site_type = point_type;
    using value_type = std::remove_const_t<T>;
    using reference = T&;

    T& val()            const { return m_lineptr[m_point[N-1]]; }
    point_type point()  const { return m_point; }


    T*                               m_lineptr;
    point_type                       m_point;
  };

  template <class T, std::size_t N>
  struct ndpix : ndpix_base<T, N>
  {
    using typename ndpix_base<T, N>::point_type;

    // Overwrite value_type/reference for cursor
    using value_type = ndpix;
    using reference = ndpix;

    void advance(const point_type& dp)
    {
      for (std::size_t k = 0; k < N - 1; ++k)
        this->m_lineptr += m_info->stride[k] * dp[k];
      this->m_point += dp;
    }

    const ndimage_info<T, N>*        m_info;

  private:
    friend ::ranges::range_access;
    // Do not return by const& because reverse iterator does not support stashing
    // (It creates a copy and return *it)
    ndpix read() const { return *this; }

    void next() { this->m_point[N-1]++; }
    void prev() { this->m_point[N-1]--; }
    bool equal(const ndpix& other) const { return this->m_point[N-1] == other.m_point[N-1]; }
  };

  template <class T, std::size_t N>
  struct ndpixel : ndpix_base<T, N>
  {
    using typename ndpix_base<T, N>::point_type;

    void advance(const point_type& dp)
    {
      for (std::size_t k = 0; k < N - 1; ++k)
        this->m_lineptr += m_info.stride[k] * dp[k];
      this->m_point += dp;
    }

  public:
    ndimage_info<T, N>        m_info;
  };



  template <class T, std::size_t N>
  class ndpix_range : public ranges::details::multi_view_facade<N, ndpix_range<T, N>>
  {
    using point_type = mln::point<std::ptrdiff_t, N>;

  public:
    struct row_t : public ndpix<T, N>, ::ranges::view_facade<row_t>
    {
      friend ::ranges::range_access;
    private:
      using cursor_t = ndpix<T, N>;

      cursor_t begin_cursor() const
      {
        cursor_t tmp = *this;
        tmp.m_point[N-1] = this->m_info->from[N-1];
        return tmp;
      }
      cursor_t end_cursor() const
      {
        cursor_t tmp = *this;
        tmp.m_point[N-1] = this->m_info->to[N-1];
        return tmp;
      }

    public:
      row_t() = default;
      row_t(const ndpix<T, N>& px) : ndpix<T,N>(px) {}
    };

  public:
    struct cursor : ndpix<T, N>
    {
      using value_type = ndpix<T, N>;
      using reference = const ndpix<T, N>&;

      const ndpix<T,N>& __read() const { return *this; }
      const ndpix<T,N>& __rread() const { return *this; }
      row_t __read_row() const { return row_t(*this); }
      ::ranges::reverse_view<row_t> __read_rrow() const { return ::ranges::view::reverse(row_t(*this)); }

      void __next(std::size_t k)
      {
        this->m_point[k]++;
        if (k + 2 == N)
          __update_lineptr();
      }
      void __rnext(std::size_t k)
      {
        this->m_point[k]--;
        if (k + 2 == N)
          __update_lineptr();
      }

      void __reset_to_begin(std::size_t k)
      {
        this->m_point[k] = this->m_info->from[k];
        if (k + 2 == N)
          __update_lineptr();
      }

      void __reset_to_rbegin(std::size_t k)
      {
        this->m_point[k] = this->m_info->to[k] - 1;
        if (k + 2 == N)
          __update_lineptr();
      }

      bool __is_at_end(std::size_t k) const { return this->m_point[k] == this->m_info->to[k]; }
      bool __is_at_rend(std::size_t k) const { return this->m_point[k] == this->m_info->from[k] - 1; }

      cursor() = default;
      cursor(const ndpix_range& r, bool forward = true)
      {
        this->m_info = &r.m_info;
        this->m_point = forward ? this->m_info->from : (this->m_info->to - 1);
        this->__update_lineptr();
      }

    private:
      void __update_lineptr()
      {
        this->m_lineptr = this->m_info->buffer;
        for (std::size_t k = 0; k < N - 1; ++k)
          this->m_lineptr += this->m_point[k] * this->m_info->stride[k];
      }
    };

    ndpix_range() = default;

    ndpix_range(const std::array<std::ptrdiff_t, N>& strides,
                const point_type& from,
                const point_type& to,
                T* ptr)
        : m_info{strides, from, to, ptr}
    {
    }

  private:
    ndimage_info<T, N> m_info;
  };

}
