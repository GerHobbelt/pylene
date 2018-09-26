#pragma once

#include <mln/core/rangev3/private/multidimensional_range.hpp>
#include <mln/core/rangev3/private/multidimensional_range_facade.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/reverse.hpp>
#include <array>

namespace mln::ranges
{

  template <std::size_t Rank, typename T = int>
  struct multi_indices;

  template <std::size_t Rank, typename T = int>
  struct reversed_multi_indices;


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename T>
  struct multi_indices<1, T> : public ::ranges::view_facade<multi_indices<1, T>>
  {
  private:
    friend ::ranges::range_access;
    using value_t = std::array<T, 1>;

    struct cursor
    {
      value_t read() const { return {x}; }
      void next() { ++x; }
      void prev() { --x; }
      bool equal(const cursor& other) const { return x == other.x; }

      T x;
    };

  public:
    cursor begin_cursor() const { return cursor{m_from}; }
    cursor end_cursor() const { return cursor{m_to}; }

    multi_indices() = default;
    multi_indices(value_t from, value_t to) : m_from(from[0]), m_to(to[0]) {}
    multi_indices(value_t to) : m_to(to[0]) {}


    reversed_multi_indices<1, T> reversed() const { return {{m_from}, {m_to}}; }

  private:
    T m_from = 0;
    T m_to;
  };


  template <typename T>
  struct reversed_multi_indices<1, T> : public ::ranges::reverse_view<multi_indices<1, T>>
  {
  private:
    using adapted = multi_indices<1, T>;
    using base = ::ranges::reverse_view<adapted>;

  public:
    using value_type = std::array<T, 1>;
    reversed_multi_indices() = default;
    reversed_multi_indices(value_type from, value_type to) : base(adapted(from, to)) {}
    reversed_multi_indices(value_type to) : base(adapted(to)) {}
  };

  /******************************************/
  /****     generic multi_indices       *****/
  /******************************************/
  template <std::size_t Rank, typename T /* = int */>
  struct multi_indices :
    public multidimensional_range_base,
    public ::ranges::view_facade<multi_indices<Rank, T>>
  {
    static_assert(Rank > 1);
    using value_type = std::array<T, Rank>;

  private:

    /******************************************/
    /****     multi_indices::cursor       *****/
    /******************************************/
    friend ::ranges::range_access;
    struct cursor : mln::ranges::details::forward_multidimensional_cursor_facade<Rank, cursor>
    {
      using base = mln::ranges::details::forward_multidimensional_cursor_facade<Rank, cursor>;

      value_type read() const { return m_current; }
      bool __is_at_end(std::size_t k) const { return m_current[k] == m_to[k]; }
      void __next(std::size_t k) { m_current[k]++; }
      void __reset_to_begin(std::size_t k) { m_current[k] = m_from[k]; }

      cursor() = default;
      cursor(value_type from, value_type to) : m_from(from), m_to(to), m_current(from) {}

      using base::equal;
      using base::next;

    private:
      value_type m_from;
      value_type m_to;
      value_type m_current;
    };

    /******************************************/
    /****     multi_indices::row_size     *****/
    /******************************************/
    struct row_wise : public ::ranges::view_facade<row_wise>
    {
    public:
      using value_type = std::array<T, Rank>;

      row_wise() = default;
      row_wise(value_type from, value_type to) : m_from(from), m_to(to) {}

    private:

      /**************************************************/
      /****     multi_indices::row_size::cursor     *****/
      /**************************************************/
      friend ::ranges::range_access;
      struct cursor : mln::ranges::details::forward_multidimensional_cursor_facade<Rank - 1, cursor>
      {
        friend ::ranges::range_access;

      private:
        /*********************************************************/
        /****     multi_indices::row_size::cursor::row_t     *****/
        /*********************************************************/
        struct row_t : public ::ranges::view_facade<row_t>
        {
          friend ::ranges::range_access;
          using value_type = std::array<T, Rank>;

          row_t() = default;
          row_t(value_type from, std::size_t size) : m_x(from), m_size(size) {}

        private:
          value_type read() const { return m_x; }
          void next()
          {
            m_size--;
            m_x.back()++;
          }
          bool equal(::ranges::default_sentinel) const { return m_size == 0; }

        protected:
          value_type m_x;
          std::size_t m_size = 0;
        };
        /**************************************************************/
        /****   end of multi_indices::row_size::cursor::row_t     *****/
        /**************************************************************/

        using base = mln::ranges::details::forward_multidimensional_cursor_facade<Rank - 1, cursor>;

        row_t read() const { return {m_x, static_cast<std::size_t>(m_to[Rank - 1] - m_from[Rank - 1])}; }

      public:
        bool __is_at_end(std::size_t k) const { return m_x[k] == m_to[k]; }
        void __next(std::size_t k) { m_x[k]++; }
        void __reset_to_begin(std::size_t k) { m_x[k] = m_from[k]; }

        constexpr cursor() = default;
        constexpr cursor(value_type from, value_type to) : m_from(from), m_to(to), m_x(from) {}

        using base::equal;
        using base::next;

      private:
        value_type m_from;
        value_type m_to;
        value_type m_x;
      };
      /*******************************************************/
      /****   end of multi_indices::row_size::cursor     *****/
      /*******************************************************/

      cursor begin_cursor() const { return {m_from, m_to}; }

    private:
      value_type m_from;
      value_type m_to;
    };
    /*******************************************************/
    /****   end of multi_indices::row_size             *****/
    /*******************************************************/

    cursor begin_cursor() const { return cursor(m_from, m_to); }

  public:
    row_wise rows() const { return {m_from, m_to}; }
    reversed_multi_indices<Rank, T> reversed() const { return {m_from, m_to}; }

    constexpr multi_indices() = default;
    constexpr multi_indices(value_type from, value_type to) : m_from(from), m_to(to) {}
    constexpr multi_indices(value_type to) : m_from{{0}}, m_to(to) {}

  protected:
    value_type m_from;
    value_type m_to;
  };

  /************************************************/
  /****    end of multi_indices               *****/
  /************************************************/

  /************************************************/
  /****     reversed_multi_indices            *****/
  /************************************************/
  template <std::size_t Rank, typename T /* = int */>
  struct reversed_multi_indices :
    public multidimensional_range_base,
    public ::ranges::view_facade<reversed_multi_indices<Rank, T>>
  {
    static_assert(Rank > 1);
    using value_type = std::array<T, Rank>;

  private:
    friend ::ranges::range_access;
    /***************************************************/
    /****     reversed_multi_indices::cursor       *****/
    /***************************************************/
    struct cursor : mln::ranges::details::forward_multidimensional_cursor_facade<Rank, cursor>
    {
      using base =  mln::ranges::details::forward_multidimensional_cursor_facade<Rank, cursor>;

      value_type read() const
      {
        value_type v = m_current;
        for (std::size_t k = 0; k < Rank; ++k)
          --v[k];
        return v;
      }
      bool __is_at_end(std::size_t k) const { return m_current[k] == m_from[k]; }
      void __next(std::size_t k) { --m_current[k]; }
      void __reset_to_begin(std::size_t k) { m_current[k] = m_to[k]; }

      cursor() = default;
      cursor(value_type from, value_type to) : m_from(from), m_to(to), m_current(to) {}

      using base::equal;
      using base::next;

    private:
      value_type m_from;
      value_type m_to;
      value_type m_current;
    };

    /*****************************************************/
    /****     reversed_multi_indices::row_size       *****/
    /*****************************************************/
    struct row_wise : public ::ranges::view_facade<row_wise>
    {
    public:
      using value_type = std::array<T, Rank>;

      row_wise() = default;
      row_wise(value_type from, value_type to) : m_from(from), m_to(to) {}

    private:
      friend ::ranges::range_access;

      /*************************************************************/
      /****     reversed_multi_indices::row_size::cursor       *****/
      /*************************************************************/
      struct cursor : mln::ranges::details::forward_multidimensional_cursor_facade<Rank - 1, cursor>
      {
        friend ::ranges::range_access;

      private:
        struct row_t : public ::ranges::view_facade<row_t>
        {
          friend ::ranges::range_access;
          using value_type = std::array<T, Rank>;

          row_t() = default;
          row_t(value_type from, std::size_t size) : m_x(from), m_size(size) {}

        private:
          value_type read() const
          {
            value_type v = m_x;
            for (std::size_t k = 0; k < Rank; ++k)
              --v[k];
            return v;
          }
          void next()
          {
            --m_size;
            --m_x.back();
          }
          bool equal(::ranges::default_sentinel) const { return m_size == 0; }

        protected:
          value_type m_x;
          std::size_t m_size = 0;
        };

        using base = mln::ranges::details::forward_multidimensional_cursor_facade<Rank - 1, cursor>;

      public:
        row_t read() const { return {m_x, static_cast<std::size_t>(m_to[Rank - 1] - m_from[Rank - 1])}; }
        bool __is_at_end(std::size_t k) const { return m_x[k] == m_from[k]; }
        void __next(std::size_t k) { --m_x[k]; }
        void __reset_to_begin(std::size_t k) { m_x[k] = m_to[k]; }

        constexpr cursor() = default;
        constexpr cursor(value_type from, value_type to) : m_from(from), m_to(to), m_x(to) {}

        using base::equal;
        using base::next;

      private:
        value_type m_from;
        value_type m_to;
        value_type m_x;
      };

      cursor begin_cursor() const { return {m_from, m_to}; }

    private:
      value_type m_from;
      value_type m_to;
  };



    cursor begin_cursor() const { return cursor(m_from, m_to); }

  public:
    row_wise               rows() const { return {m_from, m_to}; }
    multi_indices<Rank, T> reversed() const { return {m_from, m_to}; }

    constexpr reversed_multi_indices() = default;
    constexpr reversed_multi_indices(value_type from, value_type to) : m_from(from), m_to(to) {}
    constexpr reversed_multi_indices(value_type to) : m_from{{0}}, m_to(to) {}
  protected:
    value_type m_from;
    value_type m_to;
  };

}
