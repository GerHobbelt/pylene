#pragma once

#include <mln/accu/accumulator.hpp>

namespace mln
{

  namespace accu
  {

    // \brief A base class for adaptable accumulator
    template <typename E, typename ArgumentType, typename ResultType, typename Feature>
    struct accumulator_base;

    /*********************/
    /** Implementation   */
    /********************/

    template <typename E, typename ArgumentType, typename ResultType, typename Feature>
    struct accumulator_base : Accumulator<E>
    {
      using argument_type = ArgumentType;
      using result_type   = ResultType;
      using feature       = Feature;
      using provides      = boost::mpl::set<Feature>;

      using has_untake = std::false_type;

      // You mut implement the following function
      // void init()
      // void take(const argument_type&)
      // void take(const Accumulator<A>& other)
      // friend result_type extract(const E&, Feature)
      //
      // and the optional following methods:
      // bool stop()
      // void untake(const argument_type&)

      constexpr bool stop() const { return false; }

      result_type to_result() const { return extract(mln::exact(*this), feature()); }
    };
  } // namespace accu
} // namespace mln
