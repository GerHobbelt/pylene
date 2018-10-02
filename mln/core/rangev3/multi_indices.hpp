#pragma once

#include <mln/core/rangev3/private/multidimensional_range.hpp>
#include <mln/core/rangev3/private/multi_view_facade.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/reverse.hpp>
#include <array>

namespace mln::ranges
{

  template <std::size_t Rank, typename T = int>
  class multi_indices;

  namespace details
  {
    template <std::size_t Rank, class E>
    class multi_indices_facade;
  }

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {
    template <class E>
    class multi_indices_facade<1, E> : public ::ranges::view_facade<E>
    {
      friend ::ranges::range_access;

      struct cursor
      {
        using value_t = decltype(E().__from());
        using T = typename value_t::value_type;

        value_t read() const { return {x}; }
        void next() { ++x; }
        void prev() { --x; }
        bool equal(const cursor& other) const { return x == other.x; }

        T x;
      };

      auto __from() const { return static_cast<const E*>(this)->__from(); }
      auto __to() const { return static_cast<const E*>(this)->__to(); }

    public:
      cursor begin_cursor() const { return cursor{__from()[0]}; }
      cursor end_cursor() const { return cursor{__to()[0]}; }

    };


    /******************************************/
    /****     generic multi_indices       *****/
    /******************************************/

    /// Implement a multidimentional indices with rows beings indices (contiguous elements)
    template <std::size_t Rank, class E>
    class multi_indices_facade : public details::multi_view_facade<Rank, E>
    {

    public:
      /*********************************************************/
      /****     multi_indices::row_size::cursor::row_t     *****/
      /*********************************************************/
      struct row_t : public ::ranges::view_facade<row_t>
      {
        friend ::ranges::range_access;
        using value_t = decltype(E().__from());

        row_t() = default;
        row_t(value_t from, std::size_t size) : m_x(from), m_size(size) {}

      private:
        struct cursor
        {
          value_t read() const { return m_x; }
          void next() { m_x.back()++; }
          void prev() { m_x.back()--; }
          bool equal(const cursor& other) const { return m_x.back() == other.m_x.back(); }

          value_t m_x;
        };

        cursor begin_cursor() const { return {m_x}; }
        cursor end_cursor() const
        {
          auto x = m_x;
          x.back() += m_size;
          return {x};
        }

      protected:
        value_t m_x;
        std::size_t m_size = 0;
      };
      /**************************************************************/
      /****   end of multi_indices::row_size::cursor::row_t     *****/
      /**************************************************************/

      struct cursor
      {
        using value_t = decltype(E().__from());

        value_t __read() const { return m_x; }
        row_t __read_row() const { return row_t(m_x, static_cast<std::size_t>(m_to.back() - m_from.back())); }

        value_t __rread() const
        {
          auto x = m_x;
          for (std::size_t k = 0; k < Rank; ++k)
            x[k]--;
          return x;
        }

        ::ranges::reverse_view<row_t> __read_rrow() const
        {
          auto x = m_x;
          for (std::size_t k = 0; k < Rank - 1; ++k)
            x[k]--;
          x.back() = m_from.back();
          return ::ranges::view::reverse(row_t(x, static_cast<std::size_t>(m_to.back() - m_from.back())));
        }

        bool __is_at_end(std::size_t k) const { return m_x[k] == m_to[k]; }
        bool __is_at_rend(std::size_t k) const { return m_x[k] == m_from[k]; }
        void __next(std::size_t k) { m_x[k]++; }
        void __rnext(std::size_t k) { m_x[k]--; }
        void __reset_to_begin(std::size_t k) { m_x[k] = m_from[k]; }
        void __reset_to_rbegin(std::size_t k) { m_x[k] = m_to[k]; }

        cursor() = default;
        cursor(const multi_indices_facade& sp, bool forward = true) : m_from(sp.__from()), m_to(sp.__to())
        {
          m_x = forward ? m_from : m_to;
        }

      private:
        value_t m_from;
        value_t m_to;
        value_t m_x;
      };


    private:
      auto __from() const { return static_cast<const E*>(this)->__from(); }
      auto __to() const { return static_cast<const E*>(this)->__to(); }
    };
  }

  template <std::size_t Rank, typename T>
  class multi_indices : public details::multi_indices_facade<Rank, multi_indices<Rank, T>>
  {
    using value_t = std::array<T, Rank>;

  public:
    multi_indices() = default;
    multi_indices(value_t from, value_t to) : m_from(from), m_to(to) {}
    multi_indices(value_t to) : m_to(to) {}


    value_t __from() const { return m_from; }
    value_t __to() const { return m_to; }

  private:
    value_t m_from = {0, };
    value_t m_to;
  };

}
