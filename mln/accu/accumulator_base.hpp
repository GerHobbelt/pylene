#ifndef MLN_ACCU_ACCUMULATOR_BASE_HPP
# define ACCUMULATOR_BASE_HPP

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


    namespace internal
    {
      template <typename T>
      struct feature_to_accu
      {
	template <typename F>
	struct apply
	{
	  typedef typename F::template apply<T>::type type;
	};
      };

      struct accu_init
      {
	template <typename Accu>
	void operator () (Accu& acc) const { acc.init(); }
      };

      template <typename T>
      struct accu_take
      {
	accu_take(const T& x) : m_x(x) {}

	template <typename Accu>
	void operator () (Accu& acc) const { acc.take(m_x); }

      private:
	const T& m_x;
      };

      template <typename T>
      struct accu_untake
      {
	accu_untake(const T& x) : m_x(x) {}

	template <typename Accu>
	void operator () (Accu& acc) const { acc.untake(m_x); }

      private:
	const T& m_x;
      };

      template <typename Feature>
      struct accu_has_feature
      {
	template <typename Accu>
	struct apply
	{
	  typedef typename std::is_same<typename Accu::feature, Feature>::type type;
	};
      };


      template <typename AccuList, typename Feature>
      struct acculist_has_feature
      {
	typedef typename boost::fusion::result_of::find_if<AccuList, accu_has_feature<Feature> >::type iterator;
	typedef typename boost::fusion::result_of::end<AccuList>::type end;
	static constexpr bool value = not std::is_same<iterator, end>::value;
      };

      template <typename AccuList, typename Feature>
      struct acculist_get_feature
      {
	typedef typename boost::fusion::result_of::find_if<AccuList, accu_has_feature<Feature> >::type iterator;
	typedef typename boost::fusion::result_of::deref<iterator>::type accu;

	typedef decltype(extract (std::declval<accu>(), Feature ()) ) type;
      };

    }


    template <typename Accu, typename T, typename... depends>
    struct composite_accumulator_base : Accumulator<Accu>
    {
      typedef T					argument_type;
      typedef boost::fusion::list<depends...>	dependances;

      void init()
      {
	boost::fusion::for_each(m_accus, internal::accu_init ());
      }


      void take(const argument_type& x)
      {
	boost::fusion::for_each(m_accus, internal::accu_take<argument_type>(x));
      }

      template <typename Other>
      void take(const Accumulator<Other>& other)
      {
	boost::fusion::for_each(m_accus, internal::accu_take<Other>(exact(other)));
      }


      void untake(const argument_type& x)
      {
	boost::fusion::for_each(m_accus, internal::accu_untake<argument_type>(x));
      }

      template <typename Feature>
      friend
      typename boost::lazy_enable_if_c< internal::acculist_has_feature<dependances, Feature>::value,
					internal::acculist_get_feature<dependances, Feature> >::type
      extract(const composite_accumulator_base& accu, Feature feat)
      {
	auto res = boost::fusion::find_if< internal::accu_has_feature<Feature> >(accu.m_accus);
	return extract(*res, feat);
      }

    private:
      dependances m_accus;
    };


  }

}

#endif // ! ACCUMULATOR_BASE_HPP
