#pragma once

#include <ano/core/concepts/structuring_element.hpp>
#include <ano/core/se/view/adaptor.hpp>

#include <range/v3/view/filter.hpp>

#include <utility>


namespace ano
{
  template <class Pred, class SE>
  class se_filter_view : public se_adaptor<SE>
  {
    using base_t = se_adaptor<SE>;

  public:
    se_filter_view(Pred pred, SE se)
      : base_t{std::move(se)}
      , m_pred{std::move(pred)}
    {
    }

    template <class P>
    requires(ano::is_a<P, ano::details::Pixel>::value) auto operator()(const P& pixel) const
    {
      return ::ranges::views::filter(details::sliding_pixel_range{pixel, static_cast<const SE*>(this)->offsets()},
                                     [this](auto pix) { return m_pred(pix.point()); });
    }

    template <class P>
    requires(!ano::is_a<P, ano::details::Pixel>::value) auto operator()(const P& point) const
    {
      return ::ranges::views::filter(::ranges::views::transform(static_cast<const SE*>(this)->offsets(),
                                                                [point](P offset) -> P { return point + offset; }),
                                     m_pred);
    }

  private:
    Pred m_pred;
  };

  namespace view
  {
    template <class Pred, class SE>
    se_filter_view<Pred, SE> se_filter(Pred&& pred, SE&& se)
    {
      return {std::forward<Pred>(pred), std::forward<SE>(se)};
    }
  } // namespace view
} // namespace ano
