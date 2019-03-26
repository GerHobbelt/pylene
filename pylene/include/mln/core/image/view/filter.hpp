#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/image/view/clip.hpp>
#include <mln/core/rangev3/view/filter.hpp>
#include <mln/core/rangev3/view/remove_if.hpp>

#include <range/v3/empty.hpp>
#include <range/v3/utility/functional.hpp>

#include <type_traits>


namespace mln
{

  template <class I, class F>
  class filter_view : public image_adaptor<I>, public experimental::Image<filter_view<I, F>>
  {
    using fun_t = F;
    fun_t f;

  public:
    /// Type definitions
    /// \{
    using new_pixel_type = image_pixel_t<I>;
    using typename filter_view::image_adaptor::point_type;
    using typename filter_view::image_adaptor::reference;
    using typename filter_view::image_adaptor::value_type;

    class domain_type : ::ranges::view_base
    {
      using fun_t = ::ranges::composed<F, std::reference_wrapper<I>>; // f o I::operator()
      using dom_t = decltype(std::declval<I*>()->domain());
      using rng_t = mln::ranges::remove_if_view<::ranges::view::all_t<dom_t>, ::ranges::logical_negate<fun_t>>;

      fun_t         m_fun;
      dom_t         m_dom;
      mutable rng_t m_rng; // domain can be a range, so non-const

      static_assert(::ranges::ForwardRange<rng_t>());
      static_assert(::ranges::View<::ranges::view::all_t<dom_t>>());

    public:
      using value_type = ::ranges::range_value_t<rng_t>;
      using reference  = ::ranges::range_reference_t<rng_t>;

      domain_type(I* ima, F f)
        : m_fun(std::move(f), std::ref(*ima))
        , m_dom(ima->domain())
        , m_rng(mln::ranges::view::filter(::ranges::view::all(m_dom), m_fun))
      {
      }

      auto begin() const { return ::ranges::begin(m_rng); }
      auto end() const { return ::ranges::end(m_rng); }

      bool has(point_type p) const { return m_dom.has(p) && m_fun(p); }
      bool empty() const { return ::ranges::empty(m_rng); }
    };
    /// \}


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
    PYLENE_CONCEPT_TS_ASSERT(mln::concepts::AccessibleImage<I>, "The image must be accessible.");

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

    internal::initializer<concrete_type, clip_view<I, domain_type>> concretize() const
    {
      return mln::clip_view{this->base(), domain()};
    }

    template <typename V>
    internal::initializer<ch_value_type<V>, clip_view<I, domain_type>> ch_value() const
    {
      return mln::clip_view{this->base(), domain()};
    }

    domain_type domain() const { return {const_cast<I*>(&this->base()), this->f}; }

    auto new_values() { return mln::ranges::view::filter(this->base().new_values(), f); }

    auto new_pixels()
    {
      auto g = [f_ = this->f](const auto& px) -> bool { return f_(px.val()); };
      return mln::ranges::view::filter(this->base().new_pixels(), g);
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
      mln_precondition(domain().has(p));
      mln_precondition(std::invoke(f, this->base()(p)));
      return this->base()(p);
    }
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> at(point_type p)
    {
      return this->base().at(p);
    }
    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel(point_type p)
    {
      mln_precondition(domain().has(p));
      mln_precondition(std::invoke(f, this->base().at(p)));
      return this->base().new_pixel(p);
    }
    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel_at(point_type p)
    {
      return this->base().new_pixel_at(p);
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
      static_assert(mln::is_a<I, experimental::Image>());
      static_assert(std::is_invocable_r<bool, P, image_reference_t<I>>());

      return {std::move(ima), std::move(predicate)};
    }
  } // namespace view

} // namespace mln
