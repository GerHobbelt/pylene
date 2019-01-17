#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/image/private/where.hpp>
#include <range/v3/view_facade.hpp>
#include <type_traits>

namespace mln
{

  template <class I, class M>
  class mask_view : public image_adaptor<I>, public New_Image<mask_view<I, M>>
  {
    mutable M m_mask;

  public:
    /// Type definitions
    /// \{
    using typename mask_view::image_adaptor::point_type;
    using typename mask_view::image_adaptor::value_type;
    using typename mask_view::image_adaptor::new_pixel_type;
    using typename mask_view::image_adaptor::reference;
    using domain_type    = mln::ranges::where_t<M>;
    using concrete_type  = mask_view<concrete_t<I>, M>;

    template <class V>
    using ch_value_type  = mask_view<ch_value_t<I, V>, M>;
    /// \}


    /// Traits & Image Properties
    /// \{
    using accessible            = typename I::accessible;
    using indexable             = typename I::indexable;
    using extension_category    = mln::extension::none_extension_tag; // FIXME: should be improved
    /// \}


    // Concepts check
    static_assert(std::is_same_v<typename M::value_type, bool>,
                  "Image value type must be boolean.");

    static_assert(I::accessible::value,
                  "Image must be pw-accessible.");

  private:
    class value_range_t : public ::ranges::view_facade<value_range_t>
    {
      friend ::ranges::range_access;
      using ima_value_range_t = decltype(std::declval<I&>().new_values());
      using mask_value_range_t = decltype(std::declval<M&>().new_values());

      struct cursor
      {
        ::ranges::iterator_t<ima_value_range_t> m_ima_it;
        ::ranges::iterator_t<mask_value_range_t> m_mask_it;
        ::ranges::sentinel_t<mask_value_range_t> m_mask_end;

        image_reference_t<I> read() const { return *m_ima_it; }
        bool                 equal(const cursor& other) const { return m_mask_it == other.m_mask_it; }
        bool                 equal(::ranges::default_sentinel) const { return m_mask_it == m_mask_end; }
        void                 next()
        {
          do
          {
            ++m_ima_it;
            ++m_mask_it;
          } while (m_mask_it != m_mask_end && !(*m_mask_it));
        }
      };

      cursor begin_cursor()
      {
        cursor c = { ::ranges::begin(m_ima_values), ::ranges::begin(m_mask_values), ::ranges::end(m_mask_values) };
        if (!(*c.m_mask_it))
          c.next();
        return c;
      }

    public:
      value_range_t() = default;
      value_range_t(ima_value_range_t v, mask_value_range_t m)
        : m_ima_values {std::move(v)},
          m_mask_values {std::move(m)}
      {
      }

    private:
      ima_value_range_t  m_ima_values;
      mask_value_range_t m_mask_values;
    };

    class pixel_range_t : public ::ranges::view_facade<pixel_range_t>
    {
      friend ::ranges::range_access;
      using ima_pixel_range_t = decltype(std::declval<I&>().new_pixels());
      using mask_value_range_t = decltype(std::declval<M&>().new_values());

      struct cursor
      {
        ::ranges::iterator_t<ima_pixel_range_t> m_ima_it;
        ::ranges::iterator_t<mask_value_range_t> m_mask_it;
        ::ranges::sentinel_t<mask_value_range_t> m_mask_end;

        new_pixel_type       read() const { return { (*m_ima_it) }; }
        bool                 equal(const cursor& other) const { return m_mask_it == other.m_mask_it; }
        bool                 equal(::ranges::default_sentinel) const { return m_mask_it == m_mask_end; }
        void                 next()
        {
          do
          {
            ++m_ima_it;
            ++m_mask_it;
          } while (m_mask_it != m_mask_end && !(*m_mask_it));
        }
      };

      cursor begin_cursor()
      {
        cursor c = { ::ranges::begin(m_ima_pixels), ::ranges::begin(m_mask_values), ::ranges::end(m_mask_values) };
        if (!(*c.m_mask_it))
          c.next();
        return c;
      }

    public:
      pixel_range_t() = default;
      pixel_range_t(ima_pixel_range_t v, mask_value_range_t m)
        : m_ima_pixels {std::move(v)},
          m_mask_values {std::move(m)}
      {
      }

    private:
      ima_pixel_range_t  m_ima_pixels;
      mask_value_range_t m_mask_values;
    };

  public:

    mask_view(I ima, M mask)
      : mask_view::image_adaptor{std::move(ima)}
      , m_mask{std::move(mask)}
    {
      // FIXME: possibly assert that mask.domain() == ima.domain()
    }

    template <class I2, class D2>
    mask_view(const mask_view<I2, D2>& other, mln::init)
      : mask_view::image_adaptor{static_cast<I>(imchvalue<value_type>(other.base()))}
      , m_mask{other.m_mask}
    {
    }

    template <class I2, class D2>
    mask_view(const mask_view<I2, D2>& other, const value_type& v)
      : mask_view::image_adaptor{static_cast<I>(imchvalue<value_type>(other.base()).init(v))}
      , m_mask{other.m_mask}
    {
    }

    internal::initializer<concrete_type, mask_view> concretize() const
    {
      return { *this };
    }

    template <class U>
    internal::initializer<ch_value_type<U>, mask_view> ch_value() const
    {
      return { *this };
    }

    ranges::where_t<M> domain() const { return { m_mask }; }

    value_range_t new_values() { return {this->base().new_values(), m_mask.new_values()}; }

    pixel_range_t new_pixels() { return {this->base().new_pixels(), m_mask.new_values()}; }

    reference operator()(point_type p)
    {
      mln_precondition(m_mask(p));
      mln_precondition(this->base().domain().has(p));
      return this->base().at(p);
    }

    reference at(point_type p)
    {
      return this->base().at(p);
    }


    new_pixel_type new_pixel(point_type p)
    {
      mln_precondition(m_mask.has(p));
      mln_precondition(this->base().domain().has(p));
      return this->mask_view::image_adaptor::new_pixel_at(p);
    }

    new_pixel_type new_pixel_at(point_type p)
    {
      return this->mask_view::image_adaptor::new_pixel_at(p);
    }
  };


  namespace view
  {
    template <class I, class M>
    mask_view<I, M> mask(I ima, M mask)
    {
      static_assert(mln::is_a<I, New_Image>());
      static_assert(mln::is_a<M, New_Image>());

      // FIXME: make ima a view first ?
      return { std::move(ima), std::move(mask) };
    }

  } // namespace view

}