#pragma once

#include <mln/accu/accumulator.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>

namespace mln
{

  template <typename I, class AccuLike, class Extractor = accu::default_extractor>
  [[deprecated]]
  typename accu::result_of<AccuLike, mln_value(I), Extractor>::type
  accumulate(const Image<I>& input, const AccumulatorLike<AccuLike>& accu, const Extractor& ex = Extractor());

  template <typename I, class BinaryOperator, class V>
  [[deprecated]]
  typename std::enable_if<!is_a<BinaryOperator, AccumulatorLike>::value, V>::type
  accumulate(const Image<I>& input, const BinaryOperator& op, V init);

  namespace experimental
  {
    template <typename InputImage, class AccuLike, class Extractor = accu::default_extractor>
    typename accu::result_of<AccuLike, image_value_t<InputImage>, Extractor>::type
    accumulate(InputImage input, const AccumulatorLike<AccuLike>& accu, const Extractor& ex = Extractor());

    template <typename InputImage, class V, class BinaryOperator>
    std::enable_if_t<!is_a<V, AccumulatorLike>::value, V>
    accumulate(InputImage input, V init, BinaryOperator op);
  }

  /*********************/
  /*** Implementation  */
  /*********************/

  template <typename I, class AccuLike, class Extractor>
  typename accu::result_of<AccuLike, mln_value(I), Extractor>::type
  accumulate(const Image<I>& input, const AccumulatorLike<AccuLike>& accu_, const Extractor& ex)
  {
    const I& ima = exact(input);
    auto     a   = accu::make_accumulator(exact(accu_), mln_value(I)());

    mln_foreach (const auto& v, ima.values())
      a.take(v);

    return ex(a);
  }

  template <typename I, class BinaryOperator, class V>
  typename std::enable_if<!is_a<BinaryOperator, AccumulatorLike>::value, V>::type
  accumulate(const Image<I>& input, const BinaryOperator& op, V init)
  {
    const I& ima = exact(input);

    mln_foreach (const auto& v, ima.values())
      init = op(init, v);

    return init;
  }

  namespace experimental
  {
    template <typename InputImage, class AccuLike, class Extractor>
    typename accu::result_of<AccuLike, image_value_t<InputImage>, Extractor>::type
    accumulate(InputImage input, const AccumulatorLike<AccuLike>& accu, const Extractor& ex)
    {
      static_assert(mln::is_a<InputImage, Image>());

      auto a = accu::make_accumulator(exact(accu), image_value_t<InputImage>());
      for (auto row : mln::ranges::rows(input.new_values()))
        for (auto&& v : row)
          a.take(v);
      return ex(a);
    }

    template <typename InputImage, class V, class BinaryOperator>
    std::enable_if_t<!is_a<V, AccumulatorLike>::value, V>
    accumulate(InputImage input, V init, BinaryOperator op)
    {
      static_assert(mln::is_a<InputImage, Image>());

      for (auto row : mln::ranges::rows(input.new_values()))
        for (auto&& v : row)
          init = op(init, v);

      return init;
    }
  } // namespace experimental
} // namespace mln
