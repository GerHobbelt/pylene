#include <mln/accu/composite_accumulator.hpp>
#include <mln/accu/accumulator.hpp>

#define BOOST_TEST_MODULE Accu
#include <boost/test/unit_test.hpp>
#include <boost/mpl/has_key.hpp>

namespace mln
{

  namespace accu
  {
    namespace accumulators
    {
      template <typename T> struct accu1;
      template <typename T> struct accu2;
      template <typename T> struct accu3;
      template <typename T> struct accu4;
      template <typename T> struct accu5;
    }

    namespace features
    {

      struct f1 : feature_base<f1> { template <typename T> struct apply { typedef accumulators::accu1<T> type; }; };
      struct f2 : feature_base<f2> { template <typename T> struct apply { typedef accumulators::accu2<T> type; }; };
      struct f3 : feature_base<f3> { template <typename T> struct apply { typedef accumulators::accu3<T> type; }; };
      struct f4 : feature_base<f4> { template <typename T> struct apply { typedef accumulators::accu4<T> type; }; };
      struct f5 : feature_base<f5> { template <typename T> struct apply { typedef accumulators::accu5<T> type; }; };


      template<>
      struct depends<f1> { typedef boost::mpl::set<f2> type; };

      template<>
      struct depends<f2> { typedef boost::mpl::set<f2,f3> type; };

      template<>
      struct depends<f3> { typedef boost::mpl::set<f4,f5> type; };

      template<>
      struct depends<f5> { typedef boost::mpl::set<f3> type; };

    }

    namespace accumulators
    {

#define DEF_ACCU(N)							\
      template <typename T>						\
      struct accu##N : Accumulator< accu##N<T> >			\
      {									\
	typedef T argument_type;					\
	typedef boost::mpl::set<features::f##N> provides;		\
	void init() {};							\
	void take(const T&) {};						\
	template <typename Other> void take(const Accumulator<Other>&) {} \
	friend T extract(const accu##N&, features::f##N) {return T(); };	\
      };


      DEF_ACCU(1);
      DEF_ACCU(2);
      DEF_ACCU(3);
      DEF_ACCU(4);
      DEF_ACCU(5);
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


  auto x = make_accumulator<int>(features::f3 () );
  extract(x, features::f3 ());
  extract(x, features::f4 ());
  extract(x, features::f5 ());
}
