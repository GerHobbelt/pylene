#pragma once

#include <mln/core/image/image.hpp>
#include <range/v3/view_facade.hpp>

namespace mln
{
  namespace ranges
  {

    template <class I>
    struct where_t : public ::ranges::view_facade<where_t<I>>
    {
      static_assert(mln::is_a<I, experimental::Image>());
      static_assert(std::is_convertible_v<image_value_t<I>, bool>, "Image value type must be Boolean.");

    private:
      friend ::ranges::range_access;
      using pixel_range_type = decltype(std::declval<I&>().new_pixels());

      mutable I        m_ima;
      pixel_range_type m_pixels;

      struct cursor
      {
        ::ranges::iterator_t<pixel_range_type> m_it;
        ::ranges::sentinel_t<pixel_range_type> m_end;


        image_point_t<I> read() const { return (*m_it).point(); }
        bool             equal(::ranges::default_sentinel) const { return m_it == m_end; }
        bool             equal(const cursor& other) const { return m_it == other.m_it; }

        void next()
        {
          do
          {
            ++m_it;
          } while (m_it != m_end && !(*m_it).val());
        }
      };

      cursor begin_cursor()
      {
        cursor c = {::ranges::begin(m_pixels), ::ranges::end(m_pixels)};
        if (!(*c.m_it).val())
          c.next();
        return c;
      }

    public:
      where_t(I ima)
        : m_ima(std::move(ima))
        , m_pixels(m_ima.new_pixels())
      {
      }

      where_t() = default;

      bool has(image_point_t<I> p) const { return m_ima(p); }
      bool empty() const { return m_ima.domain().empty(); }
    };

  } // namespace ranges


  namespace experimental
  {

    template <class I>
    mln::ranges::where_t<I> where(const experimental::Image<I>& ima)
    {
      return {static_cast<const I&>(ima)};
    }

  }
} // namespace mln
