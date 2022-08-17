#pragma once

#include <ano/accu/accumulator.hpp>
#include <ano/core/canvas/local_accumulation.hpp>
#include <ano/core/concepts/structuring_element.hpp>
#include <ano/core/extension/border_management.hpp>
#include <ano/core/extension/extension.hpp>
#include <ano/core/image/image.hpp>

#include <variant>


namespace ano
{
  template <class A, class I, class J, class SE, extension::BorderManagementMethod bmm>
  void accumulate_local(I f, const ano::details::StructuringElement<SE>& se, const AccumulatorLike<A>& accu,
                        extension::border_manager<bmm> bm, J g);

  template <class A, class I, class SE, extension::BorderManagementMethod bmm>
  image_ch_value_t<I, accu::result_of_t<A, image_value_t<I>>> //
  accumulate_local(I f, const ano::details::StructuringElement<SE>& se, const AccumulatorLike<A>& accu,
                   extension::border_manager<bmm> bm);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class A, class I, class J, class SE, extension::BorderManagementMethod bmm>
  void accumulate_local(I f, const ano::details::StructuringElement<SE>& se_, const AccumulatorLike<A>& accu_,
                        extension::border_manager<bmm> bm, J g)
  {
    static_assert(ano::is_a<I, ano::details::Image>());
    static_assert(ano::is_a<J, ano::details::Image>());

    const SE& se   = static_cast<const SE&>(se_);
    auto      accu = accu::make_accumulator(ano::exact(accu_), image_value_t<I>());


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
  accumulate_local(I f, const ano::details::StructuringElement<SE>& se, const AccumulatorLike<A>& accu,
                   extension::border_manager<bmm> bm)
  {
    using V = accu::result_of_t<A, image_value_t<I>>;

    image_ch_value_t<I, V> g = imchvalue<V>(f);
    accumulate_local(std::move(f), se, accu, bm, g);

    return g;
  }
} // namespace ano
