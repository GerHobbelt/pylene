#pragma once

#include <ano/accu/accumulator.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/range/rows.hpp>

namespace ano
{

  template <typename InputImage, class AccuLike, class Extractor = accu::default_extractor>
  typename accu::result_of<AccuLike, image_value_t<InputImage>, Extractor>::type
  accumulate(InputImage input, const AccumulatorLike<AccuLike>& accu, const Extractor& ex = Extractor());

  template <typename InputImage, class V, class BinaryOperator>
  std::enable_if_t<!is_a<V, AccumulatorLike>::value, V> accumulate(InputImage input, V init, BinaryOperator op);

  /*********************/
  /*** Implementation  */
  /*********************/

  template <typename InputImage, class AccuLike, class Extractor>
  typename accu::result_of<AccuLike, image_value_t<InputImage>, Extractor>::type
  accumulate(InputImage input, const AccumulatorLike<AccuLike>& accu, const Extractor& ex)
  {
    static_assert(ano::is_a<InputImage, ano::details::Image>());

    auto   a    = accu::make_accumulator(exact(accu), image_value_t<InputImage>());
    auto&& vals = input.values();
    for (auto row : ano::ranges::rows(vals))
      for (auto&& v : row)
        a.take(v);
    return ex(a);
  }

  template <typename InputImage, class V, class BinaryOperator>
  std::enable_if_t<!is_a<V, AccumulatorLike>::value, V> accumulate(InputImage input, V init, BinaryOperator op)
  {
    static_assert(ano::is_a<InputImage, ano::details::Image>());

    auto&& vals = input.values();
    for (auto row : ano::ranges::rows(vals))
      for (auto&& v : row)
        init = op(init, v);

    return init;
  }
} // namespace ano
