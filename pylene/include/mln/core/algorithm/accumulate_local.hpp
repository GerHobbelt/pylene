#pragma once


#include <mln/accu/accumulator.hpp>
#include <mln/core/canvas/local_accumulation.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/extension/extension.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/se/se.hpp>


namespace mln
{


  template <class A, class I, class J, class SE, extension::BorderManagementMethod bmm>
  void accumulate_local(I f, const experimental::StructuringElement<SE>& se, const AccumulatorLike<A>& accu,
                        extension::border_manager<bmm> bm, J g);

  template <class A, class I, class SE, extension::BorderManagementMethod bmm>
  ch_value_t<I, accu::result_of_t<A, image_value_t<I>>>
      accumulate_local(I f, const experimental::StructuringElement<SE>& se, const AccumulatorLike<A>& accu,
                       extension::border_manager<bmm> bm);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class A, class I, class J, class SE, extension::BorderManagementMethod bmm>
  void accumulate_local(I f, const experimental::StructuringElement<SE>& se_, const AccumulatorLike<A>& accu_,
                        extension::border_manager<bmm> bm, J g)
  {
    static_assert(mln::is_a<I, experimental::Image>());
    static_assert(mln::is_a<J, experimental::Image>());

    const SE& se   = static_cast<const SE&>(se_);
    auto      accu = accu::make_accumulator(mln::exact(accu_), image_value_t<I>());


    auto extended = bm.manage(f, se);

    auto algo = canvas::make_LocalAccumulation(accu, se, extended, g);
    // canvas::LocalAccumulation algo(accu, se, *extended, g);
    algo.Execute();
  }

  template <class A, class I, class SE, extension::BorderManagementMethod bmm>
  image_ch_value_t<I, accu::result_of_t<A, image_value_t<I>>>
      accumulate_local(I f, const experimental::StructuringElement<SE>& se, const AccumulatorLike<A>& accu,
                       extension::border_manager<bmm> bm)
  {
    using V = accu::result_of_t<A, image_value_t<I>>;

    image_ch_value_t<I, V> g = imchvalue<V>(f);
    accumulate_local(std::move(f), se, accu, bm, g);

    return g;
  }

} // namespace mln
