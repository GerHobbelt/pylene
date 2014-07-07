#ifndef ACCUMULATOR_HPP
# define ACCUMULATOR_HPP

# include <mln/core/concept/accumulator.hpp>
# include <mln/accu/feature.hpp>
# include <mln/accu/composite_accumulator.hpp>

/// \defgroup accu Accumulator framework
/// \{

/// \file
/// \brief Header file for the accumulator framework

namespace mln
{

  namespace accu
  {

    /// \brief The namespace for accumulator features.
    namespace features
    {
    }

    /// \brief The namespace for accumulator object themselves.
    namespace accumulators
    {
    }

    /// \brief The namespace for feature extractor.
    namespace extractor
    {
    }

#   ifdef MLN_DOXYGEN
    /// \brief Make a concrete accumulator from an accumulator-like object \p accu.
    /// If \p accu is a feature-set, it is bound to the argument type of \p arg, otherwise
    /// \tparam A the accumulator-like type.
    /// \tparam T the value type to accumulate
    /// \param accu the accumulator-like object
    /// \param arg a value prototype to accumulate
    ///
    /// \code
    /// accu::features::max<> myfeat;
    /// auto myaccu = accu::make_accumulator(myfeat, int());
    /// myaccu.init();
    /// myaccu.take(15); myaccu.take(5);
    /// int m = myaccu.to_result() // 15
    /// \endcode
    ///
    template <typename A, typename T>
    unspecified make_accumulator(const AccumulatorLike<A>& accu, const T& arg);

    /// \brief Helper structure to get the return type of an accumulator-like object.
    /// It defines an internal member type named \p type which is the result type
    /// of the accumulator
    /// \tparam AccuLike The type of the accumulator-like object.
    /// \tparam T The argument type (the type of values to accumulate)
    /// \tparam Extractor The function used to extract the result from the accumulator
    template <typename AccuLike, typename T, typename Extractor = default_extractor>
    struct result_of
    {
    typedef unspecified type;
  };

    /// \brief The defaut extractor
    /// It calls the `to_result` method of the accumulator.
    struct default_extractor;
#   endif


#   ifndef MLN_DOXYGEN
    template <typename A, typename...>
    A& make_accumulator(Accumulator<A>& accu);

    template <typename A, typename T>
    const A& make_accumulator(const Accumulator<A>& accu, const T&);

    template <typename F, typename T>
    typename F::template apply<T>::type
    make_accumulator(const FeatureSet<F>& feat, const T& = T());

    struct default_extractor
    {
    template <typename A>
    typename A::result_type
    operator() (const Accumulator<A>& accu) const
    {
	return exact(accu).to_result();
  }
  };

#   endif


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

    template <typename A, typename T>
    const A& make_accumulator(const Accumulator<A>& accu, const T&)
    {
      return exact(accu);
    }

    template <typename F, typename T>
    typename F::template apply<T>::type
    make_accumulator(const FeatureSet<F>& fs, const T&)
    {
      return exact(fs).template make<T>();
    }

  }

  }

    /// \}

#endif // ! ACCUMULATOR_HPP
