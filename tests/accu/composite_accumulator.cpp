#include <mln/accu/composite_accumulator.hpp>

#define BOOST_TEST_MODULE Accu
#include <boost/test/unit_test.hpp>
#include <boost/mpl/has_key.hpp>
#include <mln/core/concept/check.hpp>

namespace mln
{

  namespace accu
  {

    namespace features
    {

      struct f1 {};
      struct f2 {};
      struct f3 {};
      struct f4 {};
      struct f5 {};

      template<>
      struct depends<f1> { typedef boost::mpl::set<f2> type; };

      template<>
      struct depends<f2> { typedef boost::mpl::set<f2,f3> type; };

      template<>
      struct depends<f3> { typedef boost::mpl::set<f4,f5> type; };

      template<>
      struct depends<f5> { typedef boost::mpl::set<f3> type; };

    }

  }

}

BOOST_AUTO_TEST_CASE(Accu_Features)
{
  using namespace mln::accu;

  typedef boost::mpl::set<features::f1> Set;
  typedef typename internal::resolve_dependances<Set>::type Features;



  static_assert(boost::mpl::has_key<Features, features::f1>::value, "");
  static_assert(boost::mpl::has_key<Features, features::f2>::value, "");
  static_assert(boost::mpl::has_key<Features, features::f3>::value, "");
  static_assert(boost::mpl::has_key<Features, features::f4>::value, "");
  static_assert(boost::mpl::has_key<Features, features::f5>::value, "");
}
