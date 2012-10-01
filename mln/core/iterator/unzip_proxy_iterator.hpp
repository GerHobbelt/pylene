#ifndef UNZIP_PROXY_ITERATOR_HPP
# define UNZIP_PROXY_ITERATOR_HPP

# include <mln/core/iterator/iterator_base.hpp>
# include <type_traits>

namespace mln
{

  template <typename Iterator>
  struct proxy_iterator;

  template <typename ZipIterator, size_t n>
  struct unzip_proxy_pixel_iterator;

  template <typename ZipIterator, size_t n>
  struct unzip_proxy_iterator;


  template <typename Iterator>
  proxy_iterator<Iterator>
  make_proxy_iterator(const Iterator& x);

  template <size_t n, typename ProxyIterator>
  unzip_proxy_iterator<ProxyIterator, n>
  make_unzip_proxy_iterator(const ProxyIterator& x);

  /*****************/
  /***** Implem ****/
  /*****************/


  template <typename Iterator>
  struct proxy_iterator
    : iterator_base< proxy_iterator<Iterator>,
		     typename Iterator::value_type,
		     typename Iterator::reference >
  {

    proxy_iterator(const Iterator& x)
    : x_ (x)
    {
    }

    void init()
    {
      x_.init();
    }

    void next()
    {
      if (!deja_vu_)
	{
	  x_.next();
	  deja_vu_ = true;
	}
    }

    bool finished() const { return x_.finished(); }
    typename Iterator::reference dereference() const { return *x_; }

    void set_dejavu_(bool v)
    {
      deja_vu_ = v;
    }

  private:
    Iterator x_;
    bool deja_vu_;
  };

  template <typename Iterator>
  proxy_iterator<Iterator>
  make_proxy_iterator(const Iterator& x)
  {
    return proxy_iterator<Iterator>(x);
  }


  namespace internal
  {

    template <typename T>
    struct is_boost_tuple_base
    {
      template <typename... U>
      static constexpr std::true_type check(boost::tuple<U...> );
      static constexpr std::false_type check(...);

      typedef decltype(check( std::declval<T>() )) type;
    };

    template <typename T>
    struct is_boost_tuple : is_boost_tuple_base<T>::type
    {
    };

    template <typename ZipIterator, size_t n>
    struct unzip_proxy_iterator_helper
    {
      typedef typename ZipIterator::reference Reference;
      typedef typename ZipIterator::value_type Value;
      typedef typename boost::tuples::element<n, Reference>::type	reference;
      typedef typename boost::tuples::element<n, Value>::type		value_type;
    };
  }




  template <typename ZipIterator, size_t n>
  struct unzip_proxy_iterator
    : iterator_base< unzip_proxy_iterator<ZipIterator, n>,
		     typename internal::unzip_proxy_iterator_helper<ZipIterator, n>::value_type,
		     typename internal::unzip_proxy_iterator_helper<ZipIterator, n>::reference >
  {

  private:
    typedef typename ZipIterator::reference Reference;
    //static_assert(internal::is_boost_tuple<Reference>::value, "");
  public:
    typedef typename internal::unzip_proxy_iterator_helper<ZipIterator, n>::reference  reference;

    unzip_proxy_iterator(ZipIterator& zip)
    : zip_(&zip)
    {
    }

    void init() { zip_->init(); }
    void next() { zip_->next(); }
    bool finished() const { return zip_->finished(); }
    reference dereference() const { return boost::get<n>(zip_->dereference()); }

    void set_dejavu_(bool v)
    {
      zip_->set_dejavu_(v);
    }

  private:
    ZipIterator* zip_;
  };

  template <size_t n, typename ProxyIterator>
  unzip_proxy_iterator<ProxyIterator, n>
  make_unzip_proxy_iterator(ProxyIterator& x)
  {
    return unzip_proxy_iterator<ProxyIterator,n>(x);
  }

}

#endif // ! UNZIP_PROXY_ITERATOR_HPP
