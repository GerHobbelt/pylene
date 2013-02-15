#ifndef COMPOSITE_ACCUMULATOR_HPP
# define COMPOSITE_ACCUMULATOR_HPP

# include <mln/accu/feature.hpp>
# include <boost/mpl/assert.hpp>
# include <boost/mpl/set.hpp>
//# include <boost/mpl/set10.hpp>
# include <boost/mpl/transform.hpp>
# include <boost/mpl/fold.hpp>
# include <boost/mpl/copy.hpp>
# include <boost/mpl/inserter.hpp>
# include <boost/mpl/remove_if.hpp>
# include <boost/mpl/placeholders.hpp>
# include <boost/mpl/quote.hpp>
# include <boost/mpl/insert.hpp>
# include <boost/mpl/has_key.hpp>
# include <boost/mpl/empty.hpp>
# include <boost/mpl/protect.hpp>

namespace mln
{

  namespace accu
  {

    namespace internal
    {

      namespace mpl = boost::mpl;
      namespace ph = boost::mpl::placeholders;

      template <typename Set>
      using mpl_set_inserter = mpl::inserter< Set, mpl::insert<ph::_1, ph::_2> >;

      template <typename Set>
      using mpl_set_inserter_p = mpl::inserter< Set, mpl::lambda< mpl::insert<ph::_, ph::_> > >;

      template <typename Set, typename Entry>
      struct whatdepends_helper_copy
        : mpl::copy<Entry, mpl_set_inserter<Set> >
      {
      };

      template <typename FeatureSet>
      struct whatdepends
      {

        typedef mpl::quote1<features::depends>     meta_depends;
        typedef typename mpl::transform<FeatureSet, meta_depends, mpl_set_inserter< mpl::set<> > >::type dependances;
        typedef typename mpl::fold< dependances, mpl::set<>,
                                    whatdepends_helper_copy<ph::_1, ph::_2>
                                    >::type type;
      };

      template <typename FeatureSet, typename Depends>
      struct get_unresolved
      {
        typedef typename mpl::remove_if< Depends, mpl::has_key<FeatureSet, ph::_1>, mpl_set_inserter< mpl::set<> > >::type type;
      };


      template <typename FeatureSet, typename Depends = typename whatdepends<FeatureSet>::type,
                bool empty = mpl::empty<Depends>::value >
      struct resolve_dependances
      {
        typedef typename get_unresolved<FeatureSet, Depends>::type unresolved;
        typedef typename mpl::copy<unresolved, mpl_set_inserter<FeatureSet> >::type new_feature_set;
        typedef typename resolve_dependances<new_feature_set, typename whatdepends<unresolved>::type>::type type;
      };

      template <typename FeatureSet, typename Depends>
      struct resolve_dependances<FeatureSet, Depends, true>
      {
        typedef FeatureSet type;
      };

    }


    template <typename E, typename T, typename FeatureSet>
    struct composite_accumulator_base
    {
    };

  }

}

#endif // ! COMPOSITE_ACCUMULATOR_HPP
