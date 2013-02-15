#ifndef MLN_ACCU_FEATURE_HPP
# define MLN_ACCU_FEATURE_HPP

# include <boost/mpl/set.hpp>
# include <boost/mpl/inserter.hpp>
# include <boost/mpl/insert.hpp>
# include <boost/mpl/copy.hpp>
# include <boost/mpl/placeholders.hpp>
# include <boost/fusion/container/list.hpp>
# include <boost/fusion/include/for_each.hpp>
# include <boost/fusion/include/find_if.hpp>
# include <boost/fusion/include/transform.hpp>
# include <type_traits>
# include <boost/utility/enable_if.hpp>


namespace mln
{
  namespace accu
  {

    // Fwd declaration
    template <typename T, typename FeatureSet>
    struct composite_accumulator;


    namespace features
    {

      template <typename FSet>
      struct FeatureSet
      {
        typedef FSet features;

	template <typename T>
	struct apply
	{
	  typedef composite_accumulator<T, FeatureSet> type;
	};
      };

      template <typename F>
      struct feature_base : FeatureSet< boost::mpl::set<F> >
      {
      };

      namespace ph = boost::mpl::placeholders;

      template <typename FSetA, typename FSetB>
      FeatureSet< typename boost::mpl::copy<FSetA , boost::mpl::inserter<FSetB, boost::mpl::insert<ph::_1, ph::_2> > >::type >
      operator& (const FeatureSet<FSetA>&, const FeatureSet<FSetB>& )
      {
	return FeatureSet< typename boost::mpl::copy<FSetA , boost::mpl::inserter<FSetB, boost::mpl::insert<ph::_1, ph::_2> > >::type > ();
      }


      template <typename F>
      struct depends;


      template <typename F>
      struct depends< FeatureSet<F> >
      {
        typedef boost::mpl::set<> type;
      };


    }  // end of namespace mln::accu::features

  } // end of namespace mln::accu

} // end of namespace mln

#endif //!MLN_ACCU_FEATURE_HPP
