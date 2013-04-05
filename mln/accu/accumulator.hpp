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
    template <typename A, typename ...>
    A& make_accumulator(Accumulator<A>& accu);

    template <typename F, typename T>
    typename F::template apply<T>::type
    make_accumulator(const FeatureSet<F>& feat, T = T());
    /// \}

    struct default_extractor
    {

      template <typename A>
      typename A::result_type
      operator() (const Accumulator<A>& accu) const
      {
	return accu.to_result();
      }
    };


    template <typename AccuLike, typename T, typename Extractor = default_extractor,
	      typename Enabler = void>
    struct result_of
    {
      static_assert( std::is_convertible<AccuLike, FeatureSet<AccuLike> >::value,
		     "sdfsd");
    };


    template <typename AccuLike, typename T, typename Extractor>
    struct result_of<AccuLike, T, Extractor,
		     typename std::enable_if< is_a<AccuLike, FeatureSet>::value >::type>
    {
    private:
      typedef typename AccuLike::template apply<T>::type Accu;

    public:
      typedef typename std::result_of<Extractor(Accu)>::type type;
    };

    template <typename AccuLike, typename T, typename Extractor>
    struct result_of<AccuLike, T, Extractor,
		     typename std::enable_if< is_a<AccuLike, Accumulator>::value >::type>
    {
      typedef typename std::result_of<Extractor(AccuLike)>::type type;
    };




    /// \}


    /*********************/
    /*** Implementation  */
    /*********************/

    template <typename A, typename ...>
    A& make_accumulator(Accumulator<A>& accu)
    {
      return exact(accu);
    }

    template <typename F, typename T>
    typename F::template apply<T>::type
    make_accumulator(const FeatureSet<F>&, T)
    {
      return typename F::template apply<T>::type ();
    }

  }

}

#endif // ! ACCUMULATOR_HPP
