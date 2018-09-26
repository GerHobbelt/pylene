#pragma once

#include <mln/core/rangev3/private/multidimensional_range_facade.hpp>
#include <mln/core/rangev3/private/multidimensional_range.hpp>
#include <range/v3/span.hpp>
#include <range/v3/view/reverse.hpp>
#include <array>


namespace mln::ranges
{
  template <class T, std::size_t Rank>
  class multi_span;

  template <class T, std::size_t Rank>
  class reversed_multi_span;

  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  /// Implement a multidimentional span with rows beings span (contiguous elements)
  template <class T, std::size_t Rank>
  class multi_span : public ::ranges::view_facade<multi_span<T, Rank>>, public multidimensional_range_base
  {
    friend ::ranges::range_access;

    class row_wise : public ::ranges::view_facade<row_wise>
    {
      friend ::ranges::range_access;

      struct cursor : details::forward_multidimensional_cursor_facade<Rank-1, cursor>
      {
        ::ranges::span<T> read() const { return {m_ptr.back(), static_cast<std::ptrdiff_t>(m_count.back())}; }
        bool __is_at_end(std::size_t k) const { return m_i[k] == m_count[k]; }
        void __next(std::size_t k)
        {
          m_i[k]++;
          m_ptr[k] += m_stride[k];
        }
        void __reset_to_begin(std::size_t k)
        {
          assert(k >= 1);
          m_ptr[k] = m_ptr[k-1];
          m_i[k] = 0;
        }

        using base = details::forward_multidimensional_cursor_facade<Rank-1, cursor>;
        using base::next;
        using base::equal;

        cursor() = default;
        cursor(T* pointer, std::array<std::size_t, Rank> count, std::array<std::ptrdiff_t, Rank> strides)
            : m_count(count), m_stride(strides)
        {
          m_ptr.fill(pointer);
          m_i.fill(0);
        }


      private:
        std::array<std::size_t, Rank>           m_count;
        std::array<std::ptrdiff_t, Rank>        m_stride;
        std::array<T*, Rank-1>                  m_ptr;
        std::array<std::size_t, Rank-1>         m_i;
      };

    public:
      row_wise() = default;
      row_wise(T* pointer, std::array<std::size_t, Rank> count, std::array<std::ptrdiff_t, Rank> strides)
        : m_ptr(pointer), m_count(count), m_stride(strides)
      {
      }

      cursor begin_cursor() const { return {m_ptr, m_count, m_stride}; }

    private:
      T* m_ptr;
      std::array<std::size_t, Rank>     m_count;
      std::array<std::ptrdiff_t, Rank>  m_stride;
    };

    struct cursor : details::forward_multidimensional_cursor_facade<Rank, cursor>
    {
      T& read() const { return *(m_ptr[Rank-1]); }
      bool __is_at_end(std::size_t k) const { return m_i[k] == m_count[k]; }
      void __next(std::size_t k)
      {
        m_i[k]++;
        m_ptr[k] += m_stride[k];
      }
      void __reset_to_begin(std::size_t k)
      {
        assert(k >= 1);
        m_i[k] = 0;
        m_ptr[k] = m_ptr[k - 1];
      }

      using base = details::forward_multidimensional_cursor_facade<Rank, cursor>;
      using base::next;
      using base::equal;

      cursor() = default;
      cursor(T* pointer,
             std::array<std::size_t, Rank>    count,
             std::array<std::ptrdiff_t, Rank> strides)
        : m_count(count), m_stride(strides)
      {
        m_ptr.fill(pointer);
        m_i.fill(0);
      }

    private:
      std::array<std::size_t, Rank>             m_count;
      std::array<std::ptrdiff_t, Rank>          m_stride;
      std::array<T*, Rank>                      m_ptr;
      std::array<std::size_t, Rank>             m_i;
    };

  public:
    multi_span() = default;
    multi_span(T* pointer,
               std::array<std::size_t, Rank>      count,
               std::array<std::ptrdiff_t, Rank>   strides)
      : m_ptr(pointer),  m_count(count), m_stride(strides)
    {
    }

    cursor begin_cursor() const { return {m_ptr, m_count, m_stride}; }
    row_wise rows() const { return {m_ptr, m_count, m_stride}; }
    reversed_multi_span<T, Rank> reversed() const { return {m_ptr, m_count, m_stride}; }

  private:
    T* m_ptr;
    std::array<std::size_t, Rank>      m_count;
    std::array<std::ptrdiff_t, Rank>   m_stride;
  };


  template <class T, std::size_t Rank>
  class reversed_multi_span : public ::ranges::view_facade<reversed_multi_span<T, Rank>>, public multidimensional_range_base
  {
    friend ::ranges::range_access;

    class row_wise : public ::ranges::view_facade<row_wise>
    {
      friend ::ranges::range_access;

      struct cursor : details::forward_multidimensional_cursor_facade<Rank-1, cursor>
      {
        ::ranges::reverse_view<::ranges::span<T>> read() const { return
            ::ranges::view::reverse(::ranges::make_span(m_ptr.back(), static_cast<std::ptrdiff_t>(m_count.back())));
        }

        bool __is_at_end(std::size_t k) const { return m_i[k] == m_count[k]; }
        void __next(std::size_t k)
        {
          m_i[k]++;
          m_ptr[k] -= m_stride[k];
        }
        void __reset_to_begin(std::size_t k)
        {
          assert(k >= 1);
          m_ptr[k] = m_ptr[k-1];
          m_i[k] = 0;
        }

        using base = details::forward_multidimensional_cursor_facade<Rank-1, cursor>;
        using base::next;
        using base::equal;

        cursor() = default;
        cursor(T* pointer, std::array<std::size_t, Rank> count, std::array<std::ptrdiff_t, Rank> strides)
            : m_count(count), m_stride(strides)
        {
          // Go to the last row
          for (std::size_t k = 0; k < Rank-1; ++k)
            pointer += (m_count[k] - 1) * m_stride[k];
          m_ptr.fill(pointer);
          m_i.fill(0);
        }


      private:
        std::array<std::size_t, Rank>           m_count;
        std::array<std::ptrdiff_t, Rank>        m_stride;
        std::array<T*, Rank-1>                  m_ptr;
        std::array<std::size_t, Rank-1>         m_i;
      };

    public:
      row_wise() = default;
      row_wise(T* pointer, std::array<std::size_t, Rank> count, std::array<std::ptrdiff_t, Rank> strides)
        : m_ptr(pointer), m_count(count), m_stride(strides)
      {
      }

      cursor begin_cursor() const { return {m_ptr, m_count, m_stride}; }

    private:
      T* m_ptr;
      std::array<std::size_t, Rank>     m_count;
      std::array<std::ptrdiff_t, Rank>  m_stride;
    };

    struct cursor : details::forward_multidimensional_cursor_facade<Rank, cursor>
    {
      T& read() const { return *(m_ptr.back()); }
      bool __is_at_end(std::size_t k) const { return m_i[k] == m_count[k]; }
      void __next(std::size_t k)
      {
        m_i[k]++;
        m_ptr[k] -= m_stride[k];
      }
      void __reset_to_begin(std::size_t k)
      {
        assert(k >= 1);
        m_i[k] = 0;
        m_ptr[k] = m_ptr[k - 1];
      }

      using base = details::forward_multidimensional_cursor_facade<Rank, cursor>;
      using base::next;
      using base::equal;

      cursor() = default;
      cursor(T* pointer,
             std::array<std::size_t, Rank>    count,
             std::array<std::ptrdiff_t, Rank> strides)
        : m_count(count), m_stride(strides)
      {
        // Goto last element
        for (std::size_t k = 0; k < Rank; ++k)
          pointer += (count[k] - 1) * strides[k];

        m_ptr.fill(pointer);
        m_i.fill(0);
      }

    private:
      std::array<std::size_t, Rank>             m_count;
      std::array<std::ptrdiff_t, Rank>          m_stride;
      std::array<T*, Rank>                      m_ptr;
      std::array<std::size_t, Rank>             m_i;
    };

  public:
    reversed_multi_span() = default;
    reversed_multi_span(T* pointer,
               std::array<std::size_t, Rank>      count,
               std::array<std::ptrdiff_t, Rank>   strides)
      : m_ptr(pointer),  m_count(count), m_stride(strides)
    {
    }

    cursor begin_cursor() const { return {m_ptr, m_count, m_stride}; }
    row_wise rows() const { return {m_ptr, m_count, m_stride}; }


  private:
    T* m_ptr;
    std::array<std::size_t, Rank>      m_count;
    std::array<std::ptrdiff_t, Rank>   m_stride;
  };
}
