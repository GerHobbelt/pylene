#pragma once

#include <mln/accu/accumulator.hpp>
#include <mln/core/canvas/local_accumulation.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/extension/extension.hpp>
#include <mln/core/image/image.hpp>

#include <variant>


namespace mln
{
  template <class A, class I, class J, class SE, extension::BorderManagementMethod bmm>
  void accumulate_local(I f, const mln::details::StructuringElement<SE>& se, const AccumulatorLike<A>& accu,
                        extension::border_manager<bmm> bm, J g);

  template <class A, class I, class SE, extension::BorderManagementMethod bmm>
  image_ch_value_t<I, accu::result_of_t<A, image_value_t<I>>> //
  accumulate_local(I f, const mln::details::StructuringElement<SE>& se, const AccumulatorLike<A>& accu,
                   extension::border_manager<bmm> bm);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class A, class I, class J, class SE, extension::BorderManagementMethod bmm>
  void accumulate_local(I f, const mln::details::StructuringElement<SE>& se_, const AccumulatorLike<A>& accu_,
                        extension::border_manager<bmm> bm, J g)
  {
    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<J, mln::details::Image>());

    const SE& se   = static_cast<const SE&>(se_);
    auto      accu = accu::make_accumulator(mln::exact(accu_), image_value_t<I>());


    auto [managed_ima, managed_se] = bm.manage(f, se);

    std::visit(
        [&accu, &g](auto ima, auto s) {
          auto algo = canvas::make_LocalAccumulation(accu, s, ima, g);
          // canvas::LocalAccumulation algo(accu, se, *extended, g);
          algo.Execute();
        },
        managed_ima, managed_se);
  }

  template <class A, class I, class SE, extension::BorderManagementMethod bmm>
  image_ch_value_t<I, accu::result_of_t<A, image_value_t<I>>>
  accumulate_local(I f, const mln::details::StructuringElement<SE>& se, const AccumulatorLike<A>& accu,
                       extension::border_manager<bmm> bm)
  {
    using V = accu::result_of_t<A, image_value_t<I>>;

    image_ch_value_t<I, V> g = imchvalue<V>(f);
    accumulate_local(std::move(f), se, accu, bm, g);

    return g;
  }
} // namespace mln
