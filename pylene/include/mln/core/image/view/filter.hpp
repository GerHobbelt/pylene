#pragma once

#include <mln/core/domain/where.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/image/view/clip.hpp>

#include <mln/core/range/view/filter.hpp>
#include <mln/core/range/view/remove_if.hpp>


#include <range/v3/range/primitives.hpp>
#include <range/v3/utility/functional.hpp>

#include <type_traits>


namespace mln
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
    using domain_type = mln::ranges::where_t<I, F>;


    /// Traits & Image Properties
    /// \{
    using accessible = image_accessible_t<I>;
    using indexable  = image_indexable_t<I>;
    using view       = std::true_type;
    // May be too restrictive (might be extended by image)
    using extension_category = mln::extension::none_extension_tag;
    using concrete_type      = clip_view<image_concrete_t<I>, domain_type>;
    template <class V>
    using ch_value_type = clip_view<image_ch_value_t<I, V>, domain_type>;
    /// \}

    // Checks
    static_assert(mln::concepts::AccessibleImage<I>, "The image must be accessible.");

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
      return mln::clip_view{this->base(), domain()};
    }

    template <typename V>
    image_builder<ch_value_type<V>, clip_view<I, domain_type>> ch_value() const
    {
      return mln::clip_view{this->base(), domain()};
    }

    domain_type domain() const { return {this->base(), f}; }

    auto values() { return mln::ranges::view::filter(this->base().values(), f); }

    auto pixels()
    {
      auto g = [f_ = this->f](const auto& px) -> bool { return f_(px.val()); };
      return mln::ranges::view::filter(this->base().pixels(), g);
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
    reference operator()(point_type p) requires(accessible::value)
    {
      mln_precondition(domain().has(p));
      mln_precondition(std::invoke(f, this->base()(p)));
      return this->base()(p);
    }

    reference at(point_type p) requires(accessible::value) { return this->base().at(p); }

    pixel_type pixel(point_type p) requires(accessible::value)
    {
      mln_precondition(domain().has(p));
      mln_precondition(std::invoke(f, this->base().at(p)));
      return this->base().pixel(p);
    }

    pixel_type pixel_at(point_type p) requires(accessible::value) { return this->base().pixel_at(p); }
    /// \}


    /// IndexableAndAccessible-image related methods
    /// \{
    template <typename dummy = I>
    image_index_t<dummy> index_of_point(point_type p) const requires(indexable::value&& accessible::value)
    {
      return this->base().index_of_point(p);
    }

    template <typename dummy = I>
    point_type point_at_index(image_index_t<dummy> i) const requires(indexable::value&& accessible::value)
    {
      return this->base().point_at_index(i);
    }

    template <typename dummy = I>
    image_index_t<dummy> delta_index(point_type p) const requires(indexable::value&& accessible::value)
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
      static_assert(mln::is_a<I, mln::details::Image>());
      static_assert(std::is_invocable_r<bool, P, image_reference_t<I>>());

      return {std::move(ima), std::move(predicate)};
    }
  } // namespace view

} // namespace mln
