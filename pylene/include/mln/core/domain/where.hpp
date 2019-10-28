#pragma once

#include <mln/core/image/image.hpp>

#include <mln/core/rangev3/private/mdrange_facade.hpp>

#include <range/v3/view/facade.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/functional/identity.hpp>

namespace mln::ranges
{

  namespace details
  {
    template <class Cur, class Pred>
    requires MDCursor<Cur>
    struct where_mdcursor : mdcursor_facade<where_mdcursor<Cur, Pred>>
    {
      using I = ::ranges::detail::begin_cursor_t<Cur>;
      using S = ::ranges::detail::end_cursor_t<Cur>;

      struct cursor
      {
        auto read() const { return ::ranges::range_access::read(m_pixel_cursor).point(); }
        bool equal(::ranges::default_sentinel_t) const
        {
          return ::ranges::range_access::equal(m_pixel_cursor, m_pixel_sentinel);
        }

        bool equal(const cursor& other) const
        {
          return ::ranges::range_access::equal(m_pixel_cursor, other.m_pixel_cursor);
        }

        void next()
        {
          do
          {
            ::ranges::range_access::next(m_pixel_cursor);
          } while (!::ranges::range_access::equal(m_pixel_cursor, m_pixel_sentinel) &&
                   !m_pred(::ranges::range_access::read(m_pixel_cursor).val()));
        }

        I                                 m_pixel_cursor;
        S                                 m_pixel_sentinel;
        ::ranges::semiregular_box_t<Pred> m_pred;
      };

      cursor begin_cursor() const
      {
        auto b = ::ranges::range_access::begin_cursor(m_cur);
        auto e = ::ranges::range_access::end_cursor(m_cur);
        while (!::ranges::range_access::equal(b, e))
        {
          if (std::invoke(m_pred, ::ranges::range_access::read(b).val()))
            break;
          ::ranges::range_access::next(b);
        }
        return {b, e, m_pred};
      }

      ::ranges::default_sentinel_t end_cursor() const { return {}; }


      void next() { return ::ranges::range_access::next(m_cur); }
      bool equal(const where_mdcursor& other) const { return ::ranges::range_access::equal(m_cur, other.m_cur); }
      bool equal(::ranges::default_sentinel_t) const { return ::ranges::range_access::equal(m_cur, ::ranges::default_sentinel); }


      Cur                               m_cur;
      ::ranges::semiregular_box_t<Pred> m_pred;
    };

    template <class Rng, class Pred>
    struct where_mdview;


    template <MDRange Rng, class Pred>
    struct where_mdview<Rng, Pred> : mdview_facade<where_mdview<Rng, Pred>>
    {
      using cursor = where_mdcursor<::ranges::detail::begin_cursor_t<Rng>, Pred>;

      cursor begin_cursor() const { return {{}, ::ranges::range_access::begin_cursor(m_rng), m_pred}; }
      ::ranges::default_sentinel_t end_cursor() const { return {}; }

      where_mdview() = default;
      where_mdview(Rng rng, Pred pred)
        : m_rng{std::move(rng)}
        , m_pred{std::move(pred)}
      {
      }

    protected:
      Rng                               m_rng;
      ::ranges::semiregular_box_t<Pred> m_pred;
    };

    template <::ranges::cpp20::range Rng, class Pred>
    struct where_mdview<Rng, Pred> : public ::ranges::view_facade<where_mdview<Rng, Pred>>
    {
      using R = ::ranges::views::all_t<Rng>;
      using base = ::ranges::view_facade<where_mdview<Rng, Pred>>;
      friend ::ranges::range_access;

      struct cursor
      {
        auto read() const { return m_pixel_it->point(); }
        void next()
        {
          do
          {
            ++m_pixel_it;
          } while (m_pixel_it != m_end && !m_pred(m_pixel_it->val()));
        }

        //void equal(const cursor& other) const { return m_pixel_it == other.m_pixel_it; }
        bool equal(::ranges::default_sentinel_t) const { return m_pixel_it == m_end; }

        ::ranges::iterator_t<R>           m_pixel_it;
        ::ranges::sentinel_t<R>           m_end;
        ::ranges::semiregular_box_t<Pred> m_pred;
      };

      cursor begin_cursor()
      {
        return {::ranges::begin(m_rng), ::ranges::end(m_rng), m_pred};
      }

      where_mdview() = default;
      where_mdview(Rng& rng, Pred pred)
        : m_rng{::ranges::views::all(rng)}
        , m_pred{std::move(pred)}
      {
      }

      using base::begin;
      using base::end;

    protected:
      R                                 m_rng;
      ::ranges::semiregular_box_t<Pred> m_pred;
    };

    template <class I>
    using __image_pixel_range_t = decltype(std::declval<I>().new_pixels());

  } // namespace details

  template <class I, class Pred>
  class where_t : public details::where_mdview<details::__image_pixel_range_t<I>, Pred>
  {
    static_assert(mln::is_a<I, experimental::Image>());
    static_assert(::ranges::cpp20::predicate<Pred, image_value_t<I>>);

    using pixel_range_type = details::__image_pixel_range_t<I>;


    mutable ::ranges::semiregular_box_t<I> m_ima; // mutable required because has/empty required to be const.

  public:
    where_t(I ima, Pred pred)
      : m_ima{std::move(ima)}
    {
      this->m_rng = base().new_pixels();
      this->m_pred = std::move(pred);
    }

    where_t() = default;

    bool          has(image_point_t<I> p) const { return base().domain().has(p) && this->m_pred(base()(p)); }
    bool          empty() const { return base().domain().empty(); }
    constexpr int dim() const { return base().domain().dim(); }

  private:
    I& base() const { return m_ima; }
  };

} // namespace mln::ranges


namespace mln::experimental
{

  template <class I>
  mln::ranges::where_t<I, ::ranges::identity> where(const experimental::Image<I>& ima)
  {
    return {static_cast<const I&>(ima), ::ranges::identity{}};
  }

} // namespace mln::experimental
