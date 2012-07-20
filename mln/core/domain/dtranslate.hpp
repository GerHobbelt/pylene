#ifndef MLN_CORE_DOMAINS_DTRANSLATE_HPP
# define MLN_CORE_DOMAINS_DTRANSLATE_HPP

//# include <mln/core/domain.hpp>

# include <boost/range/value_type.hpp>
# include <boost/range/adaptor/transformed.hpp>


namespace mln {

  namespace internal
  {
    template <typename Domain>
    struct translate_fun
    {
      typedef typename boost::range_value<Domain>::type P;
      typedef decltype( std::bind(std::plus<P> (), std::placeholders::_1, std::declval<P> ()) ) type;
    };

    //template <typename Domain>
    //using translate_fun = std::binder1st<std::add<typename boost::range_value<Domain>::type>>;
  };


  template <typename Domain>
  using translated_domain = boost::transformed_range<typename internal::translate_fun<Domain>::type, const Domain >;



  template <typename Domain>
  inline
  translated_domain<Domain>
  dtranslate(const Domain& domain, typename boost::range_value<Domain>::type p)
  {
    typedef typename boost::range_value<Domain>::type P;
    return boost::adaptors::transform(domain, std::bind(std::plus<P> (), std::placeholders::_1, p));
  }


} // end of namespace mln

#endif //!MLN_CORE_DOMAINS_DTRANSLATE_HPP
