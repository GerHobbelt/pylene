#pragma once

#include <ano/core/domain/where.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/image/view/adaptor.hpp>
#include <ano/core/image/view/clip.hpp>

#include <ano/core/range/view/filter.hpp>
#include <ano/core/range/view/remove_if.hpp>


#include <range/v3/range/primitives.hpp>
#include <range/v3/utility/functional.hpp>

#include <type_traits>


namespace ano
{

  template <class I, class F>
  class filter_view : public image_adaptor<I>, public details::Image<filter_view<I, F>>
  {
    using fun_t = F;
    fun_t f;

  public:
    /// Type definitions
    /// \{
    using pixel_type = image_pixel_t<I>;
    using typename filter_view::image_adaptor::point_type;
    using typename filter_view::image_adaptor::reference;
    using typename filter_view::image_adaptor::value_type;
    using domain_type = ano::ranges::where_t<I, F>;


    /// Traits & Image Properties
    /// \{
    using accessible = image_accessible_t<I>;
    using indexable  = image_indexable_t<I>;
    using view       = std::true_type;
    // May be too restrictive (might be extended by image)
    using extension_category = ano::extension::none_extension_tag;
    using concrete_type      = clip_view<image_concrete_t<I>, domain_type>;
    template <class V>
    using ch_value_type = clip_view<image_ch_value_t<I, V>, domain_type>;
    /// \}

    // Checks
    static_assert(ano::concepts::AccessibleImage<I>, "The image must be accessible.");

  private:
    struct pix_filter_fn
    {
      fun_t f;

      template <class Pix>
      bool operator()(const Pix& px) const
      {
        return f(px.val());
      }
    };


  public:
    filter_view(I ima, F fun)
      : filter_view::image_adaptor{std::move(ima)}
      , f{std::move(fun)}
    {
    }

    image_builder<concrete_type, clip_view<I, domain_type>> concretize() const
    {
      return ano::clip_view{this->base(), domain()};
    }

    template <typename V>
    image_builder<ch_value_type<V>, clip_view<I, domain_type>> ch_value() const
    {
      return ano::clip_view{this->base(), domain()};
    }

    domain_type domain() const { return {this->base(), f}; }

    auto values() { return ano::ranges::view::filter(this->base().values(), f); }

    auto pixels()
    {
      auto g = [f_ = this->f](const auto& px) -> bool { return f_(px.val()); };
      return ano::ranges::view::filter(this->base().pixels(), g);
    }


    /// Indexable-image related methods
    /// \{
    template <typename dummy = I>
    reference operator[](image_index_t<dummy> i)
    {
      return this->base()[i];
    }
    /// \}


    /// Accessible-image related methods
    /// \{
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> operator()(point_type p)
    {
      ano_precondition(domain().has(p));
      ano_precondition(std::invoke(f, this->base()(p)));
      return this->base()(p);
    }
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> at(point_type p)
    {
      return this->base().at(p);
    }
    template <typename Ret = pixel_type>
    std::enable_if_t<accessible::value, Ret> pixel(point_type p)
    {
      ano_precondition(domain().has(p));
      ano_precondition(std::invoke(f, this->base().at(p)));
      return this->base().pixel(p);
    }
    template <typename Ret = pixel_type>
    std::enable_if_t<accessible::value, Ret> pixel_at(point_type p)
    {
      return this->base().pixel_at(p);
    }
    /// \}


    /// IndexableAndAccessible-image related methods
    /// \{
    template <typename dummy = I>
    std::enable_if_t<(indexable::value && accessible::value), image_index_t<dummy>> index_of_point(point_type p) const
    {
      return this->base().index_of_point(p);
    }

    template <typename dummy = I>
    point_type point_at_index(std::enable_if_t<(indexable::value && accessible::value), image_index_t<dummy>> i) const
    {
      return this->base().point_at_index(i);
    }

    template <typename dummy = I>
    std::enable_if_t<(indexable::value && accessible::value), image_index_t<dummy>> delta_index(point_type p) const
    {
      return this->base().delta_index(p);
    }
    /// \}


    /// Raw-image related methods
    /// \{
    auto data() const           = delete;
    auto data()                 = delete;
    auto strides(int dim) const = delete;
    /// \}
  };


  namespace view
  {
    template <class I, class P>
    filter_view<I, P> filter(I ima, P predicate)
    {
      static_assert(ano::is_a<I, ano::details::Image>());
      static_assert(std::is_invocable_r<bool, P, image_reference_t<I>>());

      return {std::move(ima), std::move(predicate)};
    }
  } // namespace view

} // namespace ano
