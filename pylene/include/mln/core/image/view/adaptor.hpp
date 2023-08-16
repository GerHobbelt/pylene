#pragma once

#include <mln/core/private/traits/image.hpp>
#include <mln/core/private/traits/pixel.hpp>

#include <concepts>
#include <utility>

namespace mln
{

  // Identity pixel adaptor
  template <class Pix>
  struct pixel_adaptor
  {
    using point_type = typename Pix::point_type;
    using value_type = typename Pix::value_type;
    using reference  = typename Pix::reference;

    decltype(auto) val() const { return m_pix.val(); }
    auto           point() const { return m_pix.point(); }
    void           shift(point_type p) { m_pix.shift(p); }

    pixel_adaptor(Pix px)
      : m_pix{std::move(px)}
    {
    }

    // no default constructor as Pix may not be default constructible
    pixel_adaptor(const pixel_adaptor&)            = default;
    pixel_adaptor(pixel_adaptor&&)                 = default;
    pixel_adaptor& operator=(const pixel_adaptor&) = delete;
    pixel_adaptor& operator=(pixel_adaptor&&)      = delete;

  protected:
    Pix&       base() { return m_pix; }
    const Pix& base() const { return m_pix; }

  private:
    Pix m_pix;
  };


  template <class LhsPix, class RhsPix,
            class = std::void_t<decltype(std::declval<pixel_adaptor<LhsPix>>().val() == std::declval<RhsPix>().val())>>
  bool operator==(const pixel_adaptor<LhsPix>& lhs, const RhsPix& rhs)
  {
    return lhs.val() == rhs.val() && lhs.point() == rhs.point();
  }

  template <class LhsPix, class RhsPix,
            class = std::void_t<decltype(std::declval<RhsPix>().val() == std::declval<pixel_adaptor<LhsPix>>().val())>>
  bool operator==(const LhsPix& lhs, const pixel_adaptor<RhsPix>& rhs)
  {
    return rhs == lhs;
  }

  template <class LhsPix, class RhsPix,
            class = std::void_t<decltype(std::declval<pixel_adaptor<LhsPix>>().val() != std::declval<RhsPix>().val())>>
  bool operator!=(const pixel_adaptor<LhsPix>& lhs, const RhsPix& rhs)
  {
    return !(lhs == rhs);
  }

  template <class LhsPix, class RhsPix,
            class = std::void_t<decltype(std::declval<RhsPix>().val() != std::declval<pixel_adaptor<LhsPix>>().val())>>
  bool operator!=(const LhsPix& lhs, const pixel_adaptor<RhsPix>& rhs)
  {
    return !(rhs == lhs);
  }


  namespace detail
  {
    template <class I>
    struct image_adaptor_base_indexable
    {
    };

    template <class I>
    requires I::indexable::value //
        struct image_adaptor_base_indexable<I>
    {
      using index_type = image_index_t<I>;
    };

    template <class I>
    struct image_adaptor_base_with_extension
    {
    };

    template <class I>
    requires(not std::same_as<image_extension_category_t<I>, mln::extension::none_extension_tag>) //
        struct image_adaptor_base_with_extension<I>
    {
      using extension_type = image_extension_t<I>;
    };
  } // namespace detail


  template <class I>
  struct image_adaptor : detail::image_adaptor_base_indexable<I>, detail::image_adaptor_base_with_extension<I>
  {
  public:
    /// Type definitions
    /// \{
    using reference   = image_reference_t<I>;
    using value_type  = image_value_t<I>;
    using point_type  = image_point_t<I>;
    using domain_type = image_domain_t<I>;
    /// \}

    struct pixel_type : pixel_adaptor<image_pixel_t<I>>, mln::details::Pixel<pixel_type>
    {
      using pixel_type::pixel_adaptor::pixel_adaptor;
    };

    /// Traits & Image Properties
    /// \{
    using accessible         = image_accessible_t<I>;
    using indexable          = image_indexable_t<I>;
    using view               = std::true_type;
    using extension_category = image_extension_category_t<I>;
    using category_type      = image_category_t<I>;
    using concrete_type      = image_concrete_t<I>;

    template <class Val>
    using ch_value_type = image_ch_value_t<I, Val>;
    /// \}


    /////////////////
    // Image proxy //
    /////////////////

    explicit image_adaptor(I ima)
      : m_ima(std::move(ima))
    {
    }

    // no default constructor as I may not be default constructible
    image_adaptor(const image_adaptor<I>&)               = default;
    image_adaptor(image_adaptor<I>&&)                    = default;
    image_adaptor<I>& operator=(const image_adaptor<I>&) = delete;
    image_adaptor<I>& operator=(image_adaptor<I>&&)      = delete;

    auto domain() const { return m_ima.domain(); }
    auto values() { return m_ima.values(); }
    auto pixels() { return m_ima.pixels(); }
    auto concretize() const { return m_ima.concretize(); }

    template <typename Val>
    auto ch_value() const
    {
      return m_ima.template ch_value<Val>();
    }


    /// Indexable-image related methods
    /// \{
    template <typename dummy = I>
    reference operator[](image_index_t<dummy> i)
    {
      return m_ima[i];
    }
    /// \}


    /// Accessible-image related methods
    /// \{
    reference operator()(point_type p) requires(accessible::value) { return m_ima(p); }

    reference at(point_type p) requires(accessible::value) { return m_ima.at(p); }

    pixel_type pixel(point_type p) requires(accessible::value) { return m_ima.pixel(p); }

    pixel_type pixel_at(point_type p) requires(accessible::value) { return m_ima.pixel_at(p); }
    /// \}


    /// IndexableAndAccessible-image related methods
    /// \{
    template <class dummy = I>
    image_index_t<dummy> index_of_point(point_type p) const requires(indexable::value&& accessible::value)

    {
      return m_ima.index_of_point(p);
    }
    template <class dummy = I>
    point_type point_at_index(image_index_t<dummy> i) const requires(indexable::value&& accessible::value)
    {
      return m_ima.point_at_index(i);
    }

    template <class dummy = I>
    image_index_t<dummy> delta_index(point_type p) const requires(indexable::value&& accessible::value)
    {
      return m_ima.delta_index(p);
    }
    /// \}


    /// Raw-image related methods
    /// \{
    template <class dummy = I>
    auto data() const
    {
      return m_ima.data();
    }

    template <class dummy = I>
    auto data()
    {
      return m_ima.data();
    }

    std::ptrdiff_t stride(int dim) const requires(std::derived_from<category_type, raw_image_tag>)
    {
      return m_ima.strides(dim);
    }
    /// \}


    /// WithExtension-image related methods
    /// \{
    template <class dummy = I>
    image_extension_t<dummy> extension() const
        requires(not std::same_as<extension_category, mln::extension::none_extension_tag>)
    {
      return m_ima.extension();
    }

    template <class dummy = I>
    image_extension_t<dummy>
    extension() requires(not std::same_as<extension_category, mln::extension::none_extension_tag>)
    {
      return m_ima.extension();
    }

    auto border() const { return m_ima.border(); }
    /// \}

  protected:
    I&       base() { return m_ima; }
    const I& base() const { return m_ima; }

  private:
    I m_ima;
  };

} // namespace mln
