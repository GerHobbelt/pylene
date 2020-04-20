#pragma once

#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/se/view/adaptor.hpp>

#include <range/v3/view/filter.hpp>

#include <utility>


namespace mln
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
    requires(mln::is_a<P, mln::details::Pixel>::value) auto operator()(const P& pixel) const
    {
      return ::ranges::view::filter(details::sliding_pixel_range{pixel, static_cast<const SE*>(this)->offsets()},
                                    [this](auto pix) { return m_pred(pix.point()); });
    }

    template <class P>
    requires(!mln::is_a<P, mln::details::Pixel>::value) auto operator()(const P& point) const
    {
      return ::ranges::view::filter(::ranges::view::transform(static_cast<const SE*>(this)->offsets(),
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
} // namespace mln
