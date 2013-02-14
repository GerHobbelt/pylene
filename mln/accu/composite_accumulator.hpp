#ifndef COMPOSITE_ACCUMULATOR_HPP
# define COMPOSITE_ACCUMULATOR_HPP

namespace mln
{

  namespace accu
  {

    template <typename FeatureSet>
    struct whatdepends
    {
      typedef typename boost::mpl::fold< FeatureSet, boost::mpl::set<>,
					 boost::mpl::copy<_2 , boost::mpl::inserter<_1> > >::type type;
    };

    template <typename FeatureSet, typename Depends>
    struct get_unresoved
    {
      typedef typename boost::mpl::remove_if< Depends, boost::mpl::has<FeatureSet, boost::mpl::placeholders::_1> >::type type;
    };


    template <typename FeatureSet, typename Depends = typename whatdepends<FeatureSet>::type, bool empty = boost::mpl::empty<Depends>::type::value >
    struct resolve_dependances
    {
      typedef typename get_unresolved<FeatureSet, Depends>::type unresolved;
      typedef typename boost::mpl::copy<unresolved, boost::mpl::inserter<FeatureSet> >::type new_feature_set;
      typedef typename resolve_dependances<new_features_set, typename whatdepends<unresolved>::type>::type
    };

    template <typename FeatureSet, typename Depends>
    struct resolve_dependances<FeatureSet, Depends, true>
    {
      typedef FeatureSet type;
    };




    template <typename E, typename T, typename FeatureSet>
    struct composite_accumulator_base
    {

    };

  }

}

#endif // ! COMPOSITE_ACCUMULATOR_HPP
