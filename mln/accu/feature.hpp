#ifndef MLN_ACCU_FEATURE_HPP
# define MLN_ACCU_FEATURE_HPP

# include <boost/mpl/set.hpp>

namespace mln
{
  namespace accu
  {

    namespace features
    {

      template <typename FSet>
      struct Feature
      {
        typedef FSet provides;
      };

      template <typename F>
      struct feature_base : Feature<F>
      {
      };

      template <typename F>
      struct depends;


      template <typename F>
      struct depends< Feature<F> >
      {
        typedef boost::mpl::set<> type;
      };


    }  // end of namespace mln::accu::features

  } // end of namespace mln::accu

} // end of namespace mln

#endif //!MLN_ACCU_FEATURE_HPP
