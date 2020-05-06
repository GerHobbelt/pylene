#pragma once

#include <mln/core/private/traits/pixel.hpp>
#include <mln/core/private/traits/image.hpp>


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
    pixel_adaptor(const pixel_adaptor&) = default;
    pixel_adaptor(pixel_adaptor&&)      = default;
    pixel_adaptor& operator=(const pixel_adaptor&) = delete;
    pixel_adaptor& operator=(pixel_adaptor&&) = delete;

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
    template <class I, class = void>
    struct image_adaptor_base_indexable
    {
    };

    template <class I>
    struct image_adaptor_base_indexable<I, std::enable_if_t<I::indexable::value>>
    {
      using index_type = image_index_t<I>;
    };

    template <class I, class = void>
    struct image_adaptor_base_with_extension
    {
    };

    template <class I>
    struct image_adaptor_base_with_extension<
        I, std::enable_if_t<not std::is_same_v<image_extension_category_t<I>, mln::extension::none_extension_tag>>>
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

    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, mln::details::Pixel<new_pixel_type>
    {
      using new_pixel_type::pixel_adaptor::pixel_adaptor;
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
    image_adaptor(const image_adaptor<I>&) = default;
    image_adaptor(image_adaptor<I>&&)      = default;
    image_adaptor<I>& operator=(const image_adaptor<I>&) = delete;
    image_adaptor<I>& operator=(image_adaptor<I>&&) = delete;

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


    /// IndexableAndAccessible-image related methods
    /// \{
    template <typename dummy = I>
    std::enable_if_t<(indexable::value && accessible::value), image_index_t<dummy>> index_of_point(point_type p) const
    {
      return m_ima.index_of_point(p);
    }

    template <typename dummy = I>
    point_type point_at_index(std::enable_if_t<(indexable::value && accessible::value), image_index_t<dummy>> i) const
    {
      return m_ima.point_at_index(i);
    }

    template <typename dummy = I>
    std::enable_if_t<(indexable::value && accessible::value), image_index_t<dummy>> delta_index(point_type p) const
    {
      return m_ima.delta_index(p);
    }
    /// \}


    /// Raw-image related methods
    /// \{
    template <typename dummy = I>
    decltype(std::declval<dummy>().data()) data() const
    {
      return m_ima.data();
    }

    template <typename dummy = I>
    decltype(std::declval<dummy>().data()) data()
    {
      return m_ima.data();
    }

    template <typename Ret = std::ptrdiff_t>
    std::enable_if_t<std::is_base_of_v<raw_image_tag, category_type>, Ret> stride(int dim) const
    {
      return m_ima.strides(dim);
    }
    /// \}


    /// WithExtension-image related methods
    /// \{
    template <typename dummy = I>
    std::enable_if_t<not std::is_same_v<extension_category, mln::extension::none_extension_tag>,
                     image_extension_t<dummy>>
        extension() const
    {
      return m_ima.extension();
    }
    template <typename dummy = I>
    std::enable_if_t<not std::is_same_v<extension_category, mln::extension::none_extension_tag>,
                     image_extension_t<dummy>>
        extension()
    {
      return m_ima.extension();
    }

    template <typename U = I>
    auto border() const -> decltype(std::declval<const U>().border())
    {
      return m_ima.border();
    }
    /// \}

  protected:
    I&       base() { return m_ima; }
    const I& base() const { return m_ima; }

  private:
    I m_ima;
  };

} // namespace mln
