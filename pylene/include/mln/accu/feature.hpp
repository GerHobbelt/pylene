#pragma once

#include <mln/accu/concept/accumulator.hpp>

#include <boost/fusion/container/list.hpp>
#include <boost/fusion/include/find_if.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/inserter.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/set.hpp>

#include <type_traits>


/// Fixme:
/// Use boost fusion to transmit dynamic data of features when composing

namespace mln
{
  namespace accu
  {

    // Fwd declaration
    template <typename T, typename FeatureSet>
    struct composite_accumulator;

    namespace features
    {

      // FeatureSet uses a composite design patern
      //          +----------+
      //          |FeatureSet|
      //          +----------+
      //               |
      //         +-----+ -----------------+
      //         |                        |
      //  +------+--------+    +----------+------+
      //  | simple_feature|    |composite_feature|
      //  +---------------+    +-----------------+
      //
      //
      //
      template <typename FSet>
      struct composite_feature : FeatureSet<composite_feature<FSet>>
      {
        using features = FSet;

        template <typename T>
        struct apply
        {
          using type = composite_accumulator<T, composite_feature<FSet>>;
        };

        // Create an accumulator for a composite feature
        template <typename T>
        typename apply<T>::type make() const
        {
          return typename apply<T>::type();
        }
      };

      // A simple feature must implement:
      // F::apply<T>
      // F::make<T>(...)
      template <typename F>
      struct simple_feature : FeatureSet<F>
      {
        using features = boost::mpl::set<F>;

        // Create an accumulator for a simple feature
        // May me reimplemented in features that have
        // dynamic parameters.
        /*
        template <typename V>
        typename F::template apply<V>::type
        make()
        {
          return typename F::template apply<V>::type ();
        }
        */
      };

      /// \brief A facace other a simple feature that does not have
      /// dynamic parameters.
      /// \tparam F the feature implemented
      /// \tparam A must be a single parameter accumulator and must be default constructible.
      template <typename F, template <typename> class A>
      struct simple_feature_facade : FeatureSet<F>
      {
        using features = boost::mpl::set<F>;

        template <typename T>
        struct apply
        {
          using type = A<T>;
        };

        template <typename T>
        A<T> make() const
        {
          return A<T>();
        }
      };

      namespace ph = boost::mpl::placeholders;

      template <typename fsetA, typename fsetB>
      composite_feature<typename boost::mpl::copy<
          typename fsetA::features,
          boost::mpl::inserter<typename fsetB::features, boost::mpl::insert<ph::_1, ph::_2>>>::type>
      operator&(const FeatureSet<fsetA>&, const FeatureSet<fsetB>&)
      {
        return composite_feature<typename boost::mpl::copy<
            typename fsetA::features,
            boost::mpl::inserter<typename fsetB::features, boost::mpl::insert<ph::_1, ph::_2>>>::type>();
      }

      template <typename F>
      struct depends
      {
        using type = boost::mpl::set<>;
      };

      template <typename F>
      struct depends<simple_feature<F>>
      {
        using type = boost::mpl::set<>;
      };

    } // namespace features

  } // namespace accu

} // namespace mln
