#ifndef ACCUMULATOR_HPP
# define ACCUMULATOR_HPP

# include <mln/core/concept/accumulator.hpp>
# include <mln/accu/feature.hpp>
# include <mln/accu/composite_accumulator.hpp>

namespace mln
{

  namespace accu
  {

    namespace features
    {
    }

    namespace accumulators
    {
    }

    namespace extractor
    {
    }

    /// \defgroup freefun Free functions
    /// \{
    template <typename A>
    A& make_accumulator(Accumulator<A>& accu);

    template <typename T, typename F>
    typename F::template apply<T>::type
    make_accumulator(const features::FeatureSet<F>& feat);
    /// \}


    /*********************/
    /*** Implementation  */
    /*********************/

    template <typename A>
    A& make_accumulator(Accumulator<A>& accu)
    {
      return exact(accu);
    }

    template <typename T, typename F>
    typename features::FeatureSet<F>::template apply<T>::type
    make_accumulator(const features::FeatureSet<F>&)
    {
      return typename features::FeatureSet<F>::template apply<T>::type ();
    }

  }

}

#endif // ! ACCUMULATOR_HPP
