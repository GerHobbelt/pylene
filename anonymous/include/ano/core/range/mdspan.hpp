#pragma once

#include <ano/core/assert.hpp>
#include <ano/core/range/private/mdrange_facade.hpp>
#include <ano/core/range/private/ndrange_facade.hpp>
#include <array>
#include <range/v3/view/span.hpp>

namespace ano::ranges
{

  template <class T, std::size_t Rank>
  class mdspan;

  template <class T, std::size_t Rank>
  class mdspan : public ano::ranges::details::mdview_facade<mdspan<T, Rank>>
  {
  private:
    struct cursor_base
    {
      static constexpr int rank = Rank;

      cursor_base() = default;
      cursor_base(T* ptr, const std::size_t sizes[], const std::ptrdiff_t strides[], bool last_line)
        : m_data{ptr}
        , m_sizes{sizes}
        , m_strides{strides}
      {
        m_i[0] = 0;
        for (int k = 1; k < rank; ++k)
          m_i[k] = last_line ? sizes[k] - 1 : 0;
      }

      [[gnu::pure]] std::ptrdiff_t get_offset() const
      {
        std::ptrdiff_t x = 0;
        for (std::size_t k = 0; k < Rank; ++k)
          x += m_i[k] * m_strides[k];
        return x;
      }

      bool equal(const cursor_base& other) const { return m_i == other.m_i; }

      std::array<std::size_t, Rank> m_i;
      T*                            m_data;
      const std::size_t*            m_sizes;
      const std::ptrdiff_t*         m_strides;
    };

  public:
    struct cursor : cursor_base, ano::ranges::details::ndcursor_facade<cursor>
    {
      struct line_cursor
      {
        T&   read() const { return *m_ptr; }
        void next() { m_ptr++; }
        bool equal(line_cursor other) const { return m_ptr == other.m_ptr; }

        T* m_ptr;
      };

      line_cursor begin_cursor() const { return {this->m_data + this->get_offset()}; }
      line_cursor end_cursor() const { return {this->m_data + this->get_offset() + this->m_sizes[0]}; }

      void __reset_to_begin(int k) { this->m_i[k] = 0; }
      void __goto_end(int k) { this->m_i[k] = this->m_sizes[k]; }
      void __next(int k) { this->m_i[k]++; }
      bool __is_at_end(int k) const { return this->m_i[k] == this->m_sizes[k]; }


      using ano::ranges::details::ndcursor_facade<cursor>::equal;
      using cursor_base::cursor_base;
      using cursor_base::equal;
    };


    struct backward_cursor : cursor_base, ano::ranges::details::ndcursor_facade<backward_cursor>
    {
      struct line_cursor
      {
        T&   read() const { return *std::prev(m_ptr); }
        void next() { m_ptr--; }
        bool equal(line_cursor other) const { return m_ptr == other.m_ptr; }

        T* m_ptr;
      };

      line_cursor begin_cursor() const { return {this->m_data + this->get_offset() + this->m_sizes[0]}; }
      line_cursor end_cursor() const { return {this->m_data + this->get_offset()}; }

      void __reset_to_begin(int k) { this->m_i[k] = this->m_sizes[k] - 1; }
      void __goto_end(int k) { this->m_i[k] = static_cast<std::size_t>(-1); }
      void __next(int k) { this->m_i[k]--; }
      bool __is_at_end(int k) const { return this->m_i[k] == static_cast<std::size_t>(-1); }

      using ano::ranges::details::ndcursor_facade<backward_cursor>::equal;
      using cursor_base::cursor_base;
      using cursor_base::equal;
    };


    cursor                       begin_cursor() const { return {m_data, m_sizes, m_strides, false}; }
    ::ranges::default_sentinel_t end_cursor() const { return {}; }

    backward_cursor              rbegin_cursor() const { return {m_data, m_sizes, m_strides, true}; }
    ::ranges::default_sentinel_t rend_cursor() const { return {}; }


    mdspan() = default;
    mdspan(T* data, const std::size_t sizes[], const std::ptrdiff_t strides[])
      : m_data{data}
    {
      for (std::size_t i = 0; i < Rank; ++i)
      {
        m_sizes[i]   = sizes[i];
        m_strides[i] = strides[i];
      }
    }

    mdspan(T* data, std::array<std::size_t, Rank> sizes, std::array<std::ptrdiff_t, Rank> strides)
      : m_data{data}
    {
      for (std::size_t i = 0; i < Rank; ++i)
      {
        m_sizes[i]   = sizes[i];
        m_strides[i] = strides[i];
      }
    }

    mdspan(T* data, std::array<std::size_t, Rank> sizes)
      : m_data{data}
    {
      for (std::size_t i = 0; i < Rank; ++i)
        m_sizes[i] = sizes[i];

      m_strides[0] = 1;
      for (std::size_t i = 1; i < Rank; ++i)
        m_strides[i] = m_strides[i - 1] * m_sizes[i - 1];
    }

    T*             data() const noexcept { return m_data; }
    std::size_t    size(int dim) const noexcept { return m_sizes[dim]; }
    std::ptrdiff_t stride(int dim) const noexcept { return m_strides[dim]; }
    std::ptrdiff_t byte_stride(int dim) const noexcept { return m_strides[dim] * sizeof(T); }

    mdspan<std::byte, Rank> as_writable_bytes() noexcept
    {
      std::ptrdiff_t strides[Rank];
      for (std::size_t i = 0; i < Rank; ++i)
        strides[i] = this->byte_stride(i);
      return {reinterpret_cast<std::byte*>(m_data), m_sizes, strides};
    }

    mdspan<const std::byte, Rank> as_bytes() noexcept
    {
      std::ptrdiff_t strides[Rank];
      for (std::size_t i = 0; i < Rank; ++i)
        strides[i] = this->byte_stride(i);
      return {reinterpret_cast<const std::byte*>(m_data), m_sizes, strides};
    }


  private:
    T*             m_data;
    std::size_t    m_sizes[Rank];
    std::ptrdiff_t m_strides[Rank];
  };
} // namespace ano::ranges
