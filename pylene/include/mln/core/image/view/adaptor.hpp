#pragma once

#include <mln/core/concept/new/values.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/private/image_traits.hpp>
#include <mln/core/image/private/pixel_traits.hpp>

namespace concepts = mln::concepts;

namespace mln
{

  // Identity pixel adaptor
  template <class Pixel>
  struct pixel_adaptor
  {
    using point_type              = typename Pixel::point_type;
    using site_type[[deprecated]] = point_type;
    using value_type              = typename Pixel::value_type;
    using reference               = typename Pixel::reference;

    decltype(auto) val() const { return m_pix.val(); }
    auto           point() const { return m_pix.point(); }
    void           advance(point_type p) { m_pix.advance(p); }

    pixel_adaptor() = default;
    pixel_adaptor(Pixel px)
      : m_pix{std::move(px)}
    {
    }

  protected:
    Pixel&       base() { return m_pix; }
    const Pixel& base() const { return m_pix; }

  private:
    Pixel m_pix;
    /*
      public:
        template <class LhsPix, class PixRhs>
        friend bool operator==(const pixel_adaptor<LhsPix>& lhs, const pixel_adaptor<PixRhs>& rhs)
        {
          return lhs.base() == rhs.base();
        }

        template <class Pix>
        friend bool operator!=(const pixel_adaptor<Pix>& lhs, const pixel_adaptor<Pix>& rhs)
        {
          return !(lhs == rhs);
        }
    */
  };


  namespace detail
  {
    template <class I, class = void>
    struct image_adaptor_base_indexable
    {
    };

    template <class I>
    struct image_adaptor_base_indexable<I, std::enable_if_t<I::indexable::value>>
    {
      using size_type[[deprecated]]       = image_index_t<I>;
      using index_type                    = size_type;
      using difference_type[[deprecated]] = typename I::difference_type;
    };

    template <class I, class = void>
    struct image_adaptor_base_extended
    {
    };

    template <class I>
    struct image_adaptor_base_extended<
        I, std::enable_if_t<not std::is_same_v<image_extension_category_t<I>, mln::extension::none_extension_tag>>>
    {
      using extension_type = image_extension_t<I>;
    };
  } // namespace detail


  template <class I>
  struct image_adaptor : detail::image_adaptor_base_indexable<I>, detail::image_adaptor_base_extended<I>
  {
  public:
    /// Type definitions
    /// \{
    using reference      = image_reference_t<I>;
    using value_type     = image_value_t<I>;
    using point_type     = image_point_t<I>;
    using domain_type    = image_domain_t<I>;
    using new_pixel_type = pixel_adaptor<image_pixel_t<I>>;
    /// \}

    /// Traits & Image Properties
    /// \{
    using accessible         = image_accessible_t<I>;
    using indexable          = image_indexable_t<I>;
    using extension_category = image_extension_category_t<I>;
    using category_type      = image_category_t<I>;
    using concrete_type      = image_concrete_t<I>;

#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <concepts::Value Val>
#else
    template <typename Val>
#endif
    using ch_value_type = image_ch_value_t<I, Val>;
    /// \}


    /////////////////
    // Image proxy //
    /////////////////

    image_adaptor() = default;
    image_adaptor(I ima)
      : m_ima(std::move(ima))
    {
    }

    auto domain() const { return m_ima.domain(); }
    auto new_values() { return m_ima.new_values(); }
    auto new_pixels() { return m_ima.new_pixels(); }
    auto concretize() const { return m_ima.concretize(); }

#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <concepts::Value Val>
#else
    template <typename Val>
#endif
    auto ch_value() const
    {
      return m_ima.template ch_value<Val>();
    }


    /// Indexable-image related methods
    /// \{
    template <typename dummy = I>
    std::enable_if_t<indexable::value, reference> operator[](image_index_t<dummy> i)
    {
      return m_ima[i];
    }

    template <typename dummy = I>
    std::enable_if_t<indexable::value, image_index_t<dummy>> index_of_point(point_type p) const
    {
      return m_ima.index_of_point(p);
    }

    template <typename dummy = I>
    std::enable_if_t<indexable::value, point_type> point_at_index(image_index_t<dummy> i) const
    {
      return m_ima.point_at_index(i);
    }

    template <typename dummy = I>
    std::enable_if_t<indexable::value, image_index_t<dummy>> delta_index(point_type p) const
    {
      return m_ima.delta_index(p);
    }
    /// \}


    /// Accessible-image related methods
    /// \{
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> operator()(point_type p)
    {
      return m_ima(p);
    }

    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> at(point_type p)
    {
      return m_ima.at(p);
    }

    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel(point_type p)
    {
      return m_ima.new_pixel(p);
    }

    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel_at(point_type p)
    {
      return m_ima.new_pixel_at(p);
    }
    /// \}

    /// Raw-image related methods
    /// \{
    template <typename Ret = decltype(std::declval<I>().data())>
    std::enable_if_t<std::is_base_of_v<raw_image_tag, category_type>, Ret> data() const
    {
      return m_ima.data();
    }

    template <typename Ret = decltype(std::declval<I>().data())>
    std::enable_if_t<std::is_base_of_v<raw_image_tag, category_type>, Ret> data()
    {
      return m_ima.data();
    }

    template <typename Ret = std::ptrdiff_t>
    std::enable_if_t<std::is_base_of_v<raw_image_tag, category_type>, Ret> strides(int dim) const
    {
      return m_ima.strides(dim);
    }
    /// \}

    /// Extended-image related methods
    /// \{
    template <typename dummy = I>
    std::enable_if_t<not std::is_same_v<extension_category, mln::extension::none_extension_tag>,
                     image_extension_t<dummy>>
        extension() const
    {
      return m_ima.extension();
    }
    /// \}

  protected:
    I&       base() { return m_ima; }
    const I& base() const { return m_ima; }

  private:
    I m_ima;
  };

} // namespace mln
