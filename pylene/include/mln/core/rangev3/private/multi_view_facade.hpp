#pragma once

#include <mln/core/rangev3/private/multidimensional_range.hpp>
#include <mln/core/rangev3/private/multidimensional_range_facade.hpp>
#include <range/v3/view_facade.hpp>

namespace mln::ranges::details
{

  template <std::size_t Rank, typename R, bool forward = true>
  class multi_view_decorator;

  template <std::size_t Rank, typename R, bool forward = true>
  class multi_view_facade;


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <std::size_t Rank, typename R, bool forward>
  class multi_view_facade : public ::ranges::view_facade<multi_view_facade<Rank, R, forward>>,
                            public multidimensional_range_base
  {
    friend ::ranges::range_access;

    /***********************************************/
    /**** multi_view_facade::row_wise::row_wise ****/
    /***********************************************/
    class row_wise : public ::ranges::view_facade<row_wise>
    {
      friend ::ranges::range_access;

      /*********************************************/
      /**** multi_view_facade::row_wise::cursor ****/
      /*********************************************/
      class cursor : public forward_multidimensional_cursor_facade<Rank - 1, cursor>
      {
        friend ::ranges::range_access;
        using Cursor_ = typename R::cursor;
        using base    = forward_multidimensional_cursor_facade<Rank - 1, cursor>;


        using base::equal;
        using base::next;
        decltype(auto) read() const
        {
          if constexpr (forward)
            return m_cursor.__read_row();
          else
            return m_cursor.__read_rrow();
        }

      public:
        bool __is_at_end(std::size_t k) const
        {
          if constexpr (forward)
            return m_cursor.__is_at_end(k);
          else
            return m_cursor.__is_at_rend(k);
        }

        void __next(std::size_t k)
        {
          if constexpr (forward)
            m_cursor.__next(k);
          else
            m_cursor.__rnext(k);
        }

        void __reset_to_begin(std::size_t k)
        {
          if constexpr (forward)
            m_cursor.__reset_to_begin(k);
          else
            m_cursor.__reset_to_rbegin(k);
        }

        cursor() = default;
        cursor(Cursor_ cursor)
          : m_cursor(cursor)
        {
        }

      private:
        Cursor_ m_cursor;
      };

      cursor begin_cursor() const { return typename R::cursor(m_r, forward); }

    public:
      row_wise() = default;
      row_wise(R r)
        : m_r(std::move(r))
      {
      }

    private:
      R m_r;
    };

    /************************************/
    /**** multi_view_facade::cursor  ****/
    /************************************/
    class cursor : public forward_multidimensional_cursor_facade<Rank, cursor>
    {
      friend ::ranges::range_access;
      using base    = forward_multidimensional_cursor_facade<Rank, cursor>;
      using Cursor_ = typename R::cursor;

      using base::equal;
      using base::next;
      decltype(auto) read() const
      {
        if constexpr (forward)
          return m_cursor.__read();
        else
          return m_cursor.__rread();
      }


    public:
      bool __is_at_end(std::size_t k) const
      {
        if constexpr (forward)
          return m_cursor.__is_at_end(k);
        else
          return m_cursor.__is_at_rend(k);
      }

      void __next(std::size_t k)
      {
        if constexpr (forward)
          m_cursor.__next(k);
        else
          m_cursor.__rnext(k);
      }

      void __reset_to_begin(std::size_t k)
      {
        if constexpr (forward)
          m_cursor.__reset_to_begin(k);
        else
          m_cursor.__reset_to_rbegin(k);
      }

      cursor() = default;
      cursor(Cursor_ cursor)
        : m_cursor(cursor)
      {
      }

    private:
      Cursor_ m_cursor;
    };

    const R* exact() const { return reinterpret_cast<const R*>(this); }
    cursor   begin_cursor() const { return typename R::cursor(*exact(), forward); }


  public:
    row_wise                                rows() const { return row_wise(*exact()); }
    multi_view_decorator<Rank, R, !forward> reversed() const { return *exact(); }
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  // FIXME: how to make EBO while R inherits from multi_view_facade
  //
  template <std::size_t Rank, typename R, bool forward>
  class multi_view_decorator : public multi_view_facade<Rank, multi_view_decorator<Rank, R, forward>, forward>
  {
    friend ::ranges::range_access;
    using Cursor_ = typename R::cursor;

  public:
    struct cursor : public Cursor_
    {
      cursor() = default;
      cursor(const multi_view_decorator& x, bool fwd)
        : Cursor_(x.m_range, fwd)
      {
      }
    };

    multi_view_decorator() = default;
    multi_view_decorator(R rng)
      : m_range(rng)
    {
    }

  private:
    R m_range;
  };

} // namespace mln::ranges::details
