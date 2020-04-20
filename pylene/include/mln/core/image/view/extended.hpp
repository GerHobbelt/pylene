#pragma once

#include <mln/core/private/traits/extension.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/image/view/clamp_extended.hpp>
#include <mln/core/image/view/extended.hpp>
#include <mln/core/image/view/image_extended.hpp>
#include <mln/core/image/view/mirror_extended.hpp>
#include <mln/core/image/view/none_extended.hpp>
#include <mln/core/image/view/periodize_extended.hpp>
#include <mln/core/image/view/value_extended.hpp>
#include <mln/core/range/view/transform.hpp>


#include <range/v3/utility/common_type.hpp> // common_reference
#include <concepts/type_traits.hpp>

#include <optional>
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


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I>
  class extended_view : public image_adaptor<I>, public mln::details::Image<extended_view<I>>
  {
    using base_t = image_adaptor<I>;
    // can be the base image (se fit, no adaptation required)
    using adapted_image_t = std::variant<I, clamp_extended_view<I>, image_extended_view<I>, mirror_extended_view<I>,
                                         none_extended_view<I>, periodize_extended_view<I>, value_extended_view<I>>;

  public:
    using value_type = image_value_t<I>;
    using reference  = const value_type; // Restrict the image to be read-only by copy, should not be const and be
                                         // checked on pixel concept (but issue with proxy)
    using category_type = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using point_type    = image_point_t<I>;
    using typename image_adaptor<I>::domain_type;
    using extension_category = mln::extension::custom_extension_tag;


    // There isn't a good constexpr way yet to tell whether an extension is infinite when inside the variant
    struct extension_type
    {
      using value_type          = image_value_t<I>;
      using point_type          = image_point_t<I>;
      using support_fill        = std::true_type;
      using support_mirror      = std::true_type;
      using support_periodize   = std::true_type;
      using support_clamp       = std::true_type;
      using support_extend_with = std::true_type;

      explicit extension_type(adapted_image_t* adapted_image_ptr)
        : m_adapted_image_ptr{adapted_image_ptr}
      {
      }

      template <typename SE>
      constexpr bool fit(const SE& se) const
      {
        static_assert(concepts::StructuringElement<SE>, "SE is not a valid Structuring Element!");

        return std::visit([&se](auto&& ima) { return ima.extension().fit(se); }, *m_adapted_image_ptr);
      }

      constexpr int extent() const
      {
        return std::visit([](auto&& ima) { return ima.extension().extent(); }, *m_adapted_image_ptr);
      }

      void fill(const value_type& v)
      {
        std::visit(
            [v](auto&& ima) {
              if constexpr (image_extension_t<::concepts::remove_cvref_t<decltype(ima)>>::support_fill::value)
              {
                auto ext = ima.extension();
                if (ext.is_fill_supported())
                {
                  ext.fill(v);
                }
              }
            },
            *m_adapted_image_ptr);
      }
      void mirror(std::size_t padding = 0)
      {
        std::visit(
            [padding](auto&& ima) {
              if constexpr (image_extension_t<::concepts::remove_cvref_t<decltype(ima)>>::support_mirror::value)
              {
                auto ext = ima.extension();
                if (ext.is_mirror_supported())
                {
                  ext.mirror(padding);
                }
              }
            },
            *m_adapted_image_ptr);
      }
      void periodize()
      {
        std::visit(
            [](auto&& ima) {
              if constexpr (image_extension_t<::concepts::remove_cvref_t<decltype(ima)>>::support_periodize::value)
              {
                auto ext = ima.extension();
                if (ext.is_periodize_supported())
                {
                  ext.periodize();
                }
              }
            },
            *m_adapted_image_ptr);
      }
      void clamp()
      {
        std::visit(
            [](auto&& ima) {
              if constexpr (image_extension_t<::concepts::remove_cvref_t<decltype(ima)>>::support_clamp::value)
              {
                auto ext = ima.extension();
                if (ext.is_clamp_supported())
                {
                  ext.clamp();
                }
              }
            },
            *m_adapted_image_ptr);
      }
      template <typename U>
      void extend_with(U&& u, point_type offset = {})
      {
        std::visit(
            [u_ = std::forward<U>(u), offset](auto&& ima) {
              if constexpr (image_extension_t<::concepts::remove_cvref_t<decltype(ima)>>::support_extend_with::value)
              {
                auto ext = ima.extension();
                if (ext.is_extend_with_supported())
                {
                  ext.extend_with(u_, offset);
                }
              }
            },
            *m_adapted_image_ptr);
      }
      bool is_fill_supported() const
      {
        return std::visit([](auto&& ima) { return ima.extension().is_fill_supported(); }, *m_adapted_image_ptr);
      }
      bool is_mirror_supported() const
      {
        return std::visit([](auto&& ima) { return ima.extension().is_mirror_supported(); }, *m_adapted_image_ptr);
      }
      bool is_periodize_supported() const
      {
        return std::visit([](auto&& ima) { return ima.extension().is_periodize_supported(); }, *m_adapted_image_ptr);
      }
      bool is_clamp_supported() const
      {
        return std::visit([](auto&& ima) { return ima.extension().is_clamp_supported(); }, *m_adapted_image_ptr);
      }
      bool is_extend_with_supported() const
      {
        return std::visit([](auto&& ima) { return ima.extension().is_extend_with_supported(); }, *m_adapted_image_ptr);
      }

    private:
      const adapted_image_t* m_adapted_image_ptr; // mutable
    };

    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, mln::details::Pixel<new_pixel_type>
    {
      using reference = extended_view::reference;

      reference val() const { return (*m_ima_ptr)(this->base().point()); }

      new_pixel_type(image_pixel_t<I> px, extended_view<I>* ima_ptr)
        : new_pixel_type::pixel_adaptor{std::move(px)}
        , m_ima_ptr{ima_ptr}
      {
      }

    private:
      extended_view<I>* m_ima_ptr;
    };


  private:
    mutable adapted_image_t m_adapted_image;

  public:
    template <class U>
    extended_view(I base_ima, U adapted_image)
      : base_t{std::move(base_ima)}
      , m_adapted_image{std::move(adapted_image)}
    {
    }

    /// Accessible-image related methods
    /// \{
    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, reference> operator()(point_type p)
    {
      return std::visit([p](auto&& ima) -> reference { return ima(p); }, m_adapted_image);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, reference> at(point_type p)
    {
      return std::visit([p](auto&& ima) -> reference { return ima.at(p); }, m_adapted_image);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel(point_type p)
    {
      return {this->base().new_pixel(p), this};
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel_at(point_type p)
    {
      return {this->base().new_pixel(p), this};
    }
    /// \}

    auto pixels()
    {
      return ranges::view::transform(this->base().pixels(), [this](image_pixel_t<I> px) -> new_pixel_type {
        return {std::move(px), this};
      });
    }

    extension_type extension() const { return extension_type{&m_adapted_image}; }
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
