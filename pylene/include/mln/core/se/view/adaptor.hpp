#pragma once

#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/se/private/se_facade.hpp>

#include <concepts>

namespace mln
{
  namespace detail
  {
    template <class SE>
    struct se_adaptor_base
    {
      virtual SE&       base()       = 0;
      virtual const SE& base() const = 0;
    };

    // Dynamic
    template <class SE>
    requires std::convertible_to<typename SE::category, mln::dynamic_neighborhood_tag> //
    struct se_adaptor_dynamic : se_adaptor_base<SE>
    {
      int radial_extent() const { return this->base().radial_extent(); }
    };

    // Decomposable
    template <class SE>
    requires SE::decomposable::value //
        struct se_adaptor_decomposable : se_adaptor_base<SE>
    {
      bool is_decomposable() const { return this->base().is_decomposable(); }
      auto decompose() const { return this->base().decompose(); }
    };

    // Separable
    template <class SE>
    requires SE::separable::value //
        struct se_adaptor_separable : se_adaptor_base<SE>
    {
      bool is_separable() const { return this->base().is_separable(); }
      auto separate() const { return this->base().separate(); }
    };

    // Incrementable
    template <class SE>
    requires SE::incrementable::value //
        struct se_adaptor_incremental : se_adaptor_base<SE>
    {
      auto inc() const { return this->base().inc(); }
      auto dec() const { return this->base().dec(); }
    };
  }; // namespace detail


  template <class SE>
  struct se_adaptor : detail::se_adaptor_dynamic<SE>,
                      detail::se_adaptor_decomposable<SE>,
                      detail::se_adaptor_incremental<SE>,
                      detail::se_adaptor_base<SE>,
                      se_facade<se_adaptor<SE>>
  {
    using category     = typename SE::category;
    using incremental  = typename SE::incremental;
    using decomposable = typename SE::decomposable;
    using separable    = typename SE::separable;

    explicit se_adaptor(SE se)
      : m_se{std::move(se)}
    {
    }

    auto offsets() const { return this->base().offsets(); }

  protected:
    SE&       base() override final { return m_se; }
    const SE& base() const override final { return m_se; }

  private:
    SE m_se;
  };

} // namespace mln
