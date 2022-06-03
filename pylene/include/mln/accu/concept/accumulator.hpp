#pragma once

#include <mln/core/concepts/object.hpp>
#include <concepts>

namespace mln
{
  namespace concepts
  {
    namespace details
    {
      template <class E>
      struct has_inner_apply {
        template <class T>
        using apply = typename E::template apply<T>;
      };
    }

    template <class A>
    concept Accumulator = requires(A accu, typename A::argument_type x)
    {
      accu.init();
      accu.take(x);
      accu.take(accu);
      { accu.to_result() } -> std::convertible_to<typename A::result_type>;
    };


    template <class F>
    concept FeatureSet = requires
    {
      typename F::features;
      { details::has_inner_apply<F>{} };
    };
  }


  /// \brief Concept for accumulator-like objects
  /// accumulator-like = accumulator | feature-set
  template <typename Acc>
  struct AccumulatorLike : Object<Acc>
  {
  protected:
    AccumulatorLike()                       = default;
    AccumulatorLike(const AccumulatorLike&) = default;
    AccumulatorLike& operator=(const AccumulatorLike&) = default;
  };

  /// \brief Concept for accumulator objects
  template <typename Acc>
  struct Accumulator : AccumulatorLike<Acc>
  {
    constexpr ~Accumulator() { static_assert(mln::concepts::Accumulator<Acc>); }
  };

  /// \brief Concept for feature-set objects
  template <typename E>
  struct FeatureSet : AccumulatorLike<E>
  {
    constexpr ~FeatureSet() { static_assert(mln::concepts::FeatureSet<E>); }
  };


} // namespace mln
