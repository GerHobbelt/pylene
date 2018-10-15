#pragma once

#include <mln/core/rangev3/private/multi_view_facade.hpp>
#include <range/v3/span.hpp>
#include <range/v3/view/reverse.hpp>

#include <mln/core/assert.hpp>

#include <array>

namespace mln::ranges
{
  template <class T, std::size_t Rank>
  class multi_span;

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  // Specialization for 1D span. Since 1D span are bidirectional
  // the reversed version is not required.
  template <class T>
  class multi_span<T, 1> : public ::ranges::span<T>
  {
  public:
    multi_span() = default;

    multi_span(T* pointer, std::array<std::size_t, 1> counts, [[maybe_unused]] std::array<std::ptrdiff_t, 1> stride)
        : ::ranges::span<T>(pointer, counts[0])
    {
      mln_assertion(stride[0] == 1);
    }
  };

  /// Implement a multidimentional span with rows beings span (contiguous elements)
  template <class T, std::size_t Rank>
  class multi_span : public details::multi_view_facade<Rank, multi_span<T, Rank>>
  {
  public:
    struct cursor
    {
      T& __read() const { return *(m_ptr.back()); };
      T& __rread() const { return *(m_ptr.back()); };
      ::ranges::span<T> __read_row() const { return {m_ptr[Rank - 2], static_cast<std::ptrdiff_t>(m_count.back())}; }

      ::ranges::reverse_view<::ranges::span<T>> __read_rrow() const
      {
        auto line = ::ranges::make_span(m_ptr[Rank - 2] - m_count.back() + 1, m_ptr[Rank - 2] + 1);
        return ::ranges::view::reverse(line);
      }

      bool __is_at_end(std::size_t k) const { return m_i[k] == m_count[k]; }
      bool __is_at_rend(std::size_t k) const { return m_i[k] == m_count[k]; }

      void __next(std::size_t k)
      {
        m_i[k]++;
        m_ptr[k] += m_stride[k];
      }

      void __rnext(std::size_t k)
      {
        m_i[k]++;
        m_ptr[k] -= m_stride[k];
      }

      void __reset_to_begin(std::size_t k)
      {
        assert(k >= 1);
        m_ptr[k] = m_ptr[k - 1];
        m_i[k] = 0;
      }

      void __reset_to_rbegin(std::size_t k)
      {
        assert(k >= 1);
        m_ptr[k] = m_ptr[k - 1];
        m_i[k] = 0;
      }

      cursor() = default;
      cursor(const multi_span& sp, bool forward = true) : m_count(sp.m_count), m_stride(sp.m_stride)
      {
        T* pointer = sp.m_ptr;
        if (!forward) // Go to the last element
        {
          for (std::size_t k = 0; k < Rank; ++k)
            pointer += (m_count[k] - 1) * m_stride[k];
        }
        m_ptr.fill(pointer);
        m_i.fill(0);
      }

    private:
      std::array<std::size_t, Rank> m_count;
      std::array<std::ptrdiff_t, Rank> m_stride;
      std::array<T*, Rank> m_ptr;
      std::array<std::size_t, Rank> m_i;
    };

  public:
    multi_span() = default;
    multi_span(T* pointer, std::array<std::size_t, Rank> count, std::array<std::ptrdiff_t, Rank> strides)
        : m_ptr(pointer), m_count(count), m_stride(strides)
    {
    }

  private:
    T* m_ptr;
    std::array<std::size_t, Rank> m_count;
    std::array<std::ptrdiff_t, Rank> m_stride;
  };

} // namespace mln::ranges
