#pragma once

#include <mln/accu/accumulator.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <mln/core/rangev3/foreach.hpp>

#include <algorithm>
#include <type_traits>


namespace mln::labeling
{

  /// Compute a geometric feature (on points) on regions defined by labels.
  template <typename I, typename AccuLike>
  std::vector<typename accu::result_of<AccuLike, image_point_t<I>>::type>
  accumulate(I lbl_input, int nlabel, const AccumulatorLike<AccuLike>& accu);


  /// Compute a colorimetric feature (on value) on regions defined by labels.
  template <typename I, typename J, typename AccuLike>
  std::vector<typename accu::result_of<AccuLike, image_value_t<J>>::type>
  accumulate_on(I lbl_input, J values, int nlabel, const AccumulatorLike<AccuLike>& accu);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  /// Compute a geometric feature (on points) on regions defined by labels.
  template <typename I, typename AccuLike>
  std::vector<typename accu::result_of<AccuLike, image_point_t<I>>::type>
  accumulate(I lbl_input, int nlabel, const AccumulatorLike<AccuLike>& accu)
  {
    static_assert(mln::is_a<I, mln::experimental::Image>());

    using R = typename accu::result_of<AccuLike, image_point_t<I>>::type;

    auto acc = accu::make_accumulator(exact(accu), image_point_t<I>());
    acc.init();

    std::vector<decltype(acc)> vec_acc(nlabel + 1, acc);
    std::vector<R>             results(nlabel + 1);


    {
      mln_foreach_new (auto px, lbl_input.new_pixels())
        vec_acc[px.val()].take(px.point());
    }

    std::transform(std::begin(vec_acc), std::end(vec_acc), std::begin(results),
                   [](auto&& acc) { return acc.to_result(); });

    return results;
  }

  /// Compute a geometric feature (on points) on regions defined by labels.
  template <typename I, typename J, typename AccuLike>
  std::vector<typename accu::result_of<AccuLike, image_value_t<J>>::type>
  accumulate_on(I lbl_input, J values, int nlabel, const AccumulatorLike<AccuLike>& accu)
  {
    static_assert(mln::is_a<I, mln::experimental::Image>());
    static_assert(mln::is_a<J, mln::experimental::Image>());

    using R = typename accu::result_of<AccuLike, image_value_t<J>>::type;

    auto acc = accu::make_accumulator(exact(accu), image_value_t<J>());
    acc.init();

    std::vector<decltype(acc)> vec_acc(nlabel + 1, acc);
    std::vector<R>             results(nlabel + 1);

    {
      auto zz = mln::ranges::view::zip(lbl_input.new_values(), values.new_values());
      mln_foreach_new ((auto [lbl, v]), zz)
        vec_acc[lbl].take(v);
    }

    std::transform(std::begin(vec_acc), std::end(vec_acc), std::begin(results),
                   [](auto&& acc) { return acc.to_result(); });
    return results;
  }
} // namespace mln::labeling
