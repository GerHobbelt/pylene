#pragma once

#include <ano/core/concepts/structuring_element.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/se/private/se_facade.hpp>


namespace ano
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
    template <class SE, class = void>
    struct se_adaptor_dynamic
    {
    };
    template <class SE>
    struct se_adaptor_dynamic<
        SE, std::enable_if_t<std::is_convertible_v<typename SE::category, ano::dynamic_neighborhood_tag>>>
      : se_adaptor_base<SE>
    {
      int radial_extent() const { return this->base().radial_extent(); }
    };

    // Decomposable
    template <class SE, class = void>
    struct se_adaptor_decomposable
    {
    };
    template <class SE>
    struct se_adaptor_decomposable<SE, std::enable_if_t<SE::decomposable::value>> : se_adaptor_base<SE>
    {
      bool is_decomposable() const { return this->base().is_decomposable(); }
      auto decompose() const { return this->base().decompose(); }
    };

    // Separable
    template <class SE, class = void>
    struct se_adaptor_separable
    {
    };
    template <class SE>
    struct se_adaptor_separable<SE, std::enable_if_t<SE::separable::value>> : se_adaptor_base<SE>
    {
      bool is_separable() const { return this->base().is_separable(); }
      auto separate() const { return this->base().separate(); }
    };

    // Incrementable
    template <class SE, class = void>
    struct se_adaptor_incremental
    {
    };
    template <class SE>
    struct se_adaptor_incremental<SE, std::enable_if_t<SE::incrementable::value>> : se_adaptor_base<SE>
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

} // namespace ano
