#pragma once

#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/image/view/extended.hpp>
#include <mln/core/image/view/image_extended.hpp>
#include <mln/core/image/view/none_extended.hpp>
#include <mln/core/image/view/pattern_extended.hpp>
#include <mln/core/image/view/value_extended.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <range/v3/utility/common_type.hpp> // common_reference

#include <variant>


namespace mln
{
  template <class I>
  class extended_view;

  namespace view
  {
    template <class I, class U>
    extended_view<I> extended(I image, U adapted_image);

    template <class I>
    extended_view<I> extended(I image);
  }; // namespace view

  namespace detail
  {
    template <class... Ts>
    struct overloaded : Ts...
    {
      using Ts::operator()...;
    };
    template <class... Ts>
    overloaded(Ts...)->overloaded<Ts...>;

    template <typename V, typename I, bool is_finite>
    struct _is_finite_extension_size
    {
    };

    template <typename V, typename I>
    struct _is_finite_extension_size<V, I, true>
    {
      using is_finite = std::true_type;

      explicit _is_finite_extension_size(V* adapted_image)
        : m_adapted_image{adapted_image}
      {
      }

      std::size_t size() const
      {
        if (auto* ima = std::get_if<I>(m_adapted_image))
        {
          return ima->extension().size();
        }
        else
        {
          throw std::runtime_error("Calling size on an image that have an infinite extension");
        }
      }

    protected:
      V* m_adapted_image;
    };

    template <typename V, typename I>
    struct _is_finite_extension_size<V, I, false>
    {
      using is_finite = std::false_type;

      explicit _is_finite_extension_size(V* adapted_image)
        : m_adapted_image{adapted_image}
      {
      }

    protected:
      V* m_adapted_image;
    };
  } // namespace detail

  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I>
  class extended_view : public image_adaptor<I>, public experimental::Image<extended_view<I>>
  {
    using base_t = image_adaptor<I>;
    // can be the base image (se fit, no adaptation required)
    using adapted_image_t = std::variant<I, none_extended_view<I>, value_extended_view<I>, image_extended_view<I>,
                                         pattern_extended_view<I>>;

  public:
    using reference     = const image_value_t<I>&; // Restrict the image to be read-only
    using category_type = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using point_type    = image_point_t<I>;
    using typename image_adaptor<I>::domain_type;
    using extension_category = mln::extension::custom_extension_tag;


    // There isn't a good constexpr way yet to tell whether an extension is infinite when inside the variant
    struct extension_type : detail::_is_finite_extension_size<adapted_image_t, I, false>
    {
    private:
      using base_t = detail::_is_finite_extension_size<adapted_image_t, I, false>;

    public:
      using value_type        = image_value_t<I>;
      using point_type        = image_point_t<I>;
      using support_fill      = std::false_type;
      using support_mirror    = std::false_type;
      using support_periodize = std::false_type;
      using support_clamp     = std::false_type;
      using support_buffer    = std::false_type;

      explicit extension_type(adapted_image_t* adapted_image)
        : base_t{adapted_image}
      {
      }

      template <typename SE>
      bool fit(const SE& se) const
      {
        PYLENE_CONCEPT_TS_ASSERT(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

        return std::visit([&se](auto&& ima) { return ima.extension().fit(se); }, *m_adapted_image);
      }

      const value_type& value(const point_type& pnt) const
      {
        return std::visit([&pnt](auto&& ima) -> const value_type& { return ima.extension().value(pnt); },
                          *m_adapted_image);
      }
    };

    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, experimental::Pixel<new_pixel_type>
    {
      using reference = extended_view::reference;

      reference val() const
      {
        return std::visit(
            [pnt = this->base().point(), checked = this->m_checked](auto&& ima) -> reference {
              return checked ? ima.new_pixel(pnt).val() : ima.new_pixel_at(pnt).val();
            },
            *m_adapted_image);
      }

      new_pixel_type(image_pixel_t<I> px, adapted_image_t* adapted_ima, bool checked)
        : new_pixel_type::pixel_adaptor{std::move(px)}
        , m_adapted_image{adapted_ima}
        , m_checked{checked}
      {
      }

    private:
      adapted_image_t* m_adapted_image;
      bool             m_checked;
    };


  private:
    adapted_image_t m_adapted_image;
    extension_type  m_ext;

  public:
    template <class U>
    extended_view(I base_ima, U adapted_image)
      : base_t{std::move(base_ima)}
      , m_adapted_image{std::move(adapted_image)}
      , m_ext{&m_adapted_image}
    {
    }

    /// Accessible-image related methods
    /// \{
    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, reference> operator()(point_type p)
    {
      return std::visit([p](auto ima) -> reference { return ima(p); }, m_adapted_image);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, reference> at(point_type p)
    {
      return std::visit([p](auto ima) -> reference { return ima.at(p); }, m_adapted_image);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel(point_type p)
    {
      return {this->base().new_pixel(p), &m_adapted_image, true};
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel_at(point_type p)
    {
      return {this->base().new_pixel(p), &m_adapted_image, false};
    }
    /// \}

    auto new_pixels()
    {
      return ranges::view::transform(this->base().new_pixels(), [this](image_pixel_t<I> px) -> new_pixel_type {
        return {std::move(px), &this->m_adapted_image, false};
      });
    }

    const extension_type& extension() const { return m_ext; }
    extension_type&       extension() { return m_ext; }
  };


  namespace view
  {
    template <class I, class U>
    extended_view<I> extended(I image, U adapted_image)
    {
      return {std::move(image), std::move(adapted_image)};
    }

    template <class I>
    extended_view<I> extended(I image)
    {
      return extended(std::move(image), std::move(image));
    }
  }; // namespace view

} // namespace mln
