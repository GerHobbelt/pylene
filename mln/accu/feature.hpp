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

      // FeatureSet uses a composite design patern
      //	  +----------+
      //          |FeatureSet|
      //	  +----------+
      //	       |
      //         +-----+ -----------------+
      //	 |			  |
      //  +------+--------+    +----------+------+
      //  | simple_feature|    |composite_feature|
      //  +---------------+    +-----------------+
      //
      //
      //
      template <typename FSet>
      struct composite_feature : FeatureSet< composite_feature<FSet> >
      {
        typedef FSet features;

	template <typename T>
	struct apply
	{
	  typedef composite_accumulator<T, composite_feature<FSet> > type;
	};
      };

      template <typename F>
      struct simple_feature : FeatureSet<F>
      {
	typedef boost::mpl::set<F> features;
      };

      namespace ph = boost::mpl::placeholders;

      template <typename fsetA, typename fsetB>
      composite_feature<typename boost::mpl::copy<typename fsetA::features, boost::mpl::inserter<
						  typename fsetB::features, boost::mpl::insert<ph::_1, ph::_2> > >::type >
      operator& (const FeatureSet<fsetA>&, const FeatureSet<fsetB>& )
      {
	return composite_feature<typename boost::mpl::copy<typename fsetA::features, boost::mpl::inserter<
	  typename fsetB::features, boost::mpl::insert<ph::_1, ph::_2> > >::type > ();
      }


      template <typename F>
      struct depends
      {
	//static_assert(false, "dgfd");
	typedef boost::mpl::set<> type;
      };


      template <typename F>
      struct depends< simple_feature<F> >
      {
        typedef boost::mpl::set<> type;
      };


    }  // end of namespace mln::accu::features

  } // end of namespace mln::accu

} // end of namespace mln

#endif //!MLN_ACCU_FEATURE_HPP
