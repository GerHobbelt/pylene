#pragma once

#include <ano/accu/accumulator.hpp>
#include <ano/core/assert.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/range/foreach.hpp>
#include <ano/core/range/view/zip.hpp>

#include <algorithm>
#include <type_traits>


namespace ano::labeling
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
    static_assert(ano::is_a<I, ano::details::Image>());

    using R = typename accu::result_of<AccuLike, image_point_t<I>>::type;

    auto acc = accu::make_accumulator(exact(accu), image_point_t<I>());
    acc.init();

    std::vector<decltype(acc)> vec_acc(nlabel + 1, acc);
    std::vector<R>             results(nlabel + 1);


    {
      ano_foreach (auto px, lbl_input.pixels())
      {
        int lbl = px.val();
        assert(0 <= lbl && lbl <= nlabel);
        vec_acc[lbl].take(px.point());
      }
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
    static_assert(ano::is_a<I, ano::details::Image>());
    static_assert(ano::is_a<J, ano::details::Image>());

    using R = typename accu::result_of<AccuLike, image_value_t<J>>::type;

    auto acc = accu::make_accumulator(exact(accu), image_value_t<J>());
    acc.init();

    std::vector<decltype(acc)> vec_acc(nlabel + 1, acc);
    std::vector<R>             results(nlabel + 1);

    {
      auto zz = ano::ranges::view::zip(lbl_input.values(), values.values());
      ano_foreach ((auto [lbl, v]), zz)
      {
        assert(0 <= lbl && lbl <= nlabel);
        vec_acc[lbl].take(v);
      }
    }

    std::transform(std::begin(vec_acc), std::end(vec_acc), std::begin(results),
                   [](auto&& acc) { return acc.to_result(); });
    return results;
  }
} // namespace ano::labeling
