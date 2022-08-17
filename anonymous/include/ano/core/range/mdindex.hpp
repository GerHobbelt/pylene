#pragma once

#include <ano/core/assert.hpp>
#include <ano/core/range/private/mdrange_facade.hpp>
#include <ano/core/range/private/ndrange_facade.hpp>
#include <array>
#include <range/v3/view/span.hpp>

namespace ano::ranges
{

  template <std::size_t Rank, typename T = int>
  class mdindex;


  template <std::size_t Rank, typename T, class V, class E>
  class mdindex_facade : public ano::ranges::details::mdview_facade<E>
  {
  private:
    using value_t = V;

    struct cursor_base
    {
      static constexpr int rank = Rank;

      cursor_base() = default;
      cursor_base(const T* begin, const T* end, bool last_line)
        : m_begin{begin}
        , m_end{end}
      {
        for (int k = 0; k < rank; ++k)
          m_i[k] = last_line ? end[k] : begin[k];
      }


      bool equal(const cursor_base& other) const { return m_i == other.m_i; }

      value_t  m_i;
      const T* m_begin;
      const T* m_end;
    };

  public:
    struct cursor : cursor_base, ano::ranges::details::ndcursor_facade<cursor>
    {
      struct line_cursor
      {
        value_t read() const { return m_i; }
        void    next() { m_i[0]++; }
        bool    equal(line_cursor other) const { return m_i[0] == other.m_i[0]; }

        value_t m_i;
      };

      line_cursor begin_cursor() const { return {this->m_i}; }
      line_cursor end_cursor() const
      {
        auto tmp = this->m_i;
        tmp[0]   = this->m_end[0];
        return {tmp};
      }

      void __goto_end(int k) { this->m_i[k] = this->m_end[k]; }
      void __reset_to_begin(int k) { this->m_i[k] = this->m_begin[k]; }
      void __next(int k) { this->m_i[k]++; }
      bool __is_at_end(int k) const { return this->m_i[k] == this->m_end[k]; }

      using ano::ranges::details::ndcursor_facade<cursor>::equal;
      using cursor_base::cursor_base;
      using cursor_base::equal;
    };


    struct backward_cursor : cursor_base, ano::ranges::details::ndcursor_facade<backward_cursor>
    {

      struct line_cursor
      {
        value_t read() const
        {
          auto tmp = m_i;
          for (int k = 0; k < static_cast<int>(Rank); ++k)
            tmp[k]--;
          return tmp;
        }

        void next() { m_i[0]--; }
        bool equal(line_cursor other) const { return m_i[0] == other.m_i[0]; }

        value_t m_i;
      };


      line_cursor begin_cursor() const
      {
        auto tmp = this->m_i;
        tmp[0]   = this->m_end[0];
        return {tmp};
      }

      line_cursor end_cursor() const
      {
        auto tmp = this->m_i;
        tmp[0]   = this->m_begin[0];
        return {tmp};
      }

      void __goto_end(int k) { this->m_i[k] = this->m_begin[k]; }
      void __reset_to_begin(int k) { this->m_i[k] = this->m_end[k]; }
      void __next(int k) { this->m_i[k]--; }
      bool __is_at_end(int k) const { return this->m_i[k] == this->m_begin[k]; }

      using ano::ranges::details::ndcursor_facade<backward_cursor>::equal;
      using cursor_base::cursor_base;
      using cursor_base::equal;
    };


    ::ranges::default_sentinel_t end_cursor() const { return {}; }
    ::ranges::default_sentinel_t rend_cursor() const { return {}; }
  };


  template <std::size_t Rank, typename T>
  class mdindex : public mdindex_facade<Rank, T, std::array<T, Rank>, mdindex<Rank, T>>
  {
  private:
    using value_t = std::array<T, Rank>;
    using base    = mdindex_facade<Rank, T, value_t, mdindex<Rank, T>>;

  public:
    mdindex() = default;
    mdindex(value_t begin, value_t end)
      : m_begin{begin}
      , m_end{end}
    {
    }

    mdindex(value_t end)
      : m_end{end}
    {
    }

    typename base::cursor          begin_cursor() const { return {m_begin.data(), m_end.data(), false}; }
    typename base::backward_cursor rbegin_cursor() const { return {m_begin.data(), m_end.data(), true}; }


  private:
    value_t m_begin = {
        0,
    };
    value_t m_end;
  };

} // namespace ano::ranges
