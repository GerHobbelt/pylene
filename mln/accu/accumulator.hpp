#ifndef ACCUMULATOR_HPP
# define ACCUMULATOR_HPP

# include <mln/core/concept/accumulator.hpp>

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

    template <typename F, typename T>
    typename F::template apply<T>::type
    make_accumulator(const Feature<F>& feat);
    /// \}


    /*********************/
    /*** Implementation  */
    /*********************/

    template <typename A>
    A& make_accumulator(Accumulator<A>& accu)
    {
      return exact(accu);
    }

    template <typename F, typename T>
    typename F::template apply<T>::type
    make_accumulator(const Feature<F>& feat)
    {

    }


  }

}

#endif // ! ACCUMULATOR_HPP
