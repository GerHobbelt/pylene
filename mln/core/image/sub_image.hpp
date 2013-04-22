#ifndef MLN_CORE_IMAGE_SUB_IMAGE_HPP
# define MLN_CORE_IMAGE_SUB_IMAGE_HPP

# include <type_traits>

# include <mln/core/image_base.hpp>
# include <mln/core/range/range_traits.hpp>
# include <mln/core/range/iterator_range.hpp>
# include <mln/core/iterator/image_access_iterator.hpp>
# include <mln/core/range/filter.hpp>

namespace mln
{

  template <typename Image, typename Domain>
  struct sub_image;

  namespace internal
  {
    template <typename I>
    struct is_in_mask;
  }

  /// These overloads are worst-conversion possible
  /// \{

  // Ima | Domain
  template <typename I, typename Domain>
  sub_image<const I&, Domain>
  make_subimage(const Image<I>& ima, const Domain& domain, typename std::enable_if< not is_a<Domain, Image>::value >::type* = NULL);

  template <typename I, typename Domain>
  sub_image<I&, Domain>
  make_subimage(Image<I>& ima, const Domain& domain, typename std::enable_if< not is_a<Domain, Image>::value >::type* = NULL);

  template <typename I, typename Domain>
  sub_image<const I, Domain>
  make_subimage(const Image<I>&& ima, const Domain& domain, typename std::enable_if< not is_a<Domain, Image>::value >::type* = NULL);

  template <typename I, typename Domain>
  sub_image<I, Domain>
  make_subimage(Image<I>&& ima, const Domain& domain, typename std::enable_if< not is_a<Domain, Image>::value >::type* = NULL );


  // Ima | mask
  template <typename I, typename J>
  sub_image<const I&, filtered_range<const typename J::domain_type&, internal::is_in_mask<J> > >
  make_subimage(const Image<I>& ima, const Image<J>& mask);

  template <typename I, typename J>
  sub_image<I&, filtered_range<const typename J::domain_type&, internal::is_in_mask<J> > >
  make_subimage(Image<I>& ima, const Image<J>& mask);

  template <typename I, typename J>
  sub_image<const I, filtered_range<const typename J::domain_type&, internal::is_in_mask<J> > >
  make_subimage(const Image<I>&& ima, const Image<J>& mask);

  template <typename I, typename J>
  sub_image<I, filtered_range<const typename J::domain_type&, internal::is_in_mask<J> > >
  make_subimage(Image<I>&& ima, const Image<J>& mask);
  /// \}

  // Ima | {domain, mask}
  template <typename I, typename DomainOrMask>
  auto operator| (const Image<I>& ima, const DomainOrMask& other)
    -> decltype( make_subimage(exact(ima), other) );

  template <typename I, typename DomainOrMask>
  auto operator| (Image<I>& ima, const DomainOrMask& other)
    -> decltype( make_subimage(exact(ima), other) );

  template <typename I, typename DomainOrMask>
  auto operator| (const Image<I>&& ima, const DomainOrMask& other)
    -> decltype( make_subimage(move_exact(ima), other) );

  template <typename I, typename DomainOrMask>
  auto operator| (Image<I>&& ima, const DomainOrMask& other)
    -> decltype( make_subimage(move_exact(ima), other) );


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  /// These overloads are worst-conversion possible
  /// \{

  // Ima | Domain
  template <typename I, typename Domain>
  sub_image<const I&, Domain>
  make_subimage(const Image<I>& ima, const Domain& domain, typename std::enable_if< not is_a<Domain, Image>::value >::type* = NULL)
  {
    return sub_image<const I&, Domain>(exact(ima), domain);
  }

  template <typename I, typename Domain>
  sub_image<I&, Domain>
  make_subimage(Image<I>& ima, const Domain& domain, typename std::enable_if< not is_a<Domain, Image>::value >::type* = NULL)
  {
    return sub_image<I&, Domain>(exact(ima), domain);
  }

  template <typename I, typename Domain>
  sub_image<const I, Domain>
  make_subimage(const Image<I>&& ima, const Domain& domain, typename std::enable_if< not is_a<Domain, Image>::value >::type* = NULL)
  {
    return sub_image<I, Domain>(move_exact(ima), domain);
  }

  template <typename I, typename Domain>
  sub_image<I, Domain>
  make_subimage(Image<I>&& ima, const Domain& domain, typename std::enable_if< not is_a<Domain, Image>::value >::type* = NULL)
  {
    return sub_image<I, Domain>(move_exact(ima), domain);
  }

  // Ima | Mask
  namespace internal
  {
    template <typename I>
    struct is_in_mask
    {
      typedef typename I::point_type P;

      is_in_mask(const I& ima)
      : m_mask(ima)
      {
      }

      bool operator () (const P& p) const
      {
	return m_mask(p);
      }

    private:
      const I& m_mask;
    };
  }


  template <typename I, typename J>
  sub_image<const I&, filtered_range<const typename J::domain_type&, internal::is_in_mask<J> > >
  make_subimage(const Image<I>& ima, const Image<J>& mask)
  {
    static_assert(std::is_convertible<mln_value(J), bool>::value, "J's value type must be convertible to bool.");
    return make_subimage(exact(ima), rng::filter(exact(mask).domain(), internal::is_in_mask<J> (exact(mask))));
  }

  template <typename I, typename J>
  sub_image<I&, filtered_range<const typename J::domain_type&, internal::is_in_mask<J> > >
  make_subimage(Image<I>& ima, const Image<J>& mask)
  {
    static_assert(std::is_convertible<mln_value(J), bool>::value, "J's value type must be convertible to bool.");
    return make_subimage(exact(ima), rng::filter(exact(mask).domain(), internal::is_in_mask<J> (exact(mask))));
  }

  template <typename I, typename J>
  sub_image<const I, filtered_range<const typename J::domain_type&, internal::is_in_mask<J> > >
  make_subimage(const Image<I>&& ima, const Image<J>& mask)
  {
    static_assert(std::is_convertible<mln_value(J), bool>::value, "J's value type must be convertible to bool.");
    return make_subimage(move_exact(ima), rng::filter(exact(mask).domain(), internal::is_in_mask<J> (exact(mask))));
  }

  template <typename I, typename J>
  sub_image<I, filtered_range<const typename J::domain_type&, internal::is_in_mask<J> > >
  make_subimage(Image<I>&& ima, const Image<J>& mask)
  {
    static_assert(std::is_convertible<mln_value(J), bool>::value, "J's value type must be convertible to bool.");
    return make_subimage(move_exact(ima), rng::filter(exact(mask).domain(), internal::is_in_mask<J> (exact(mask))));
  }


  // Operator |
  template <typename I, typename DomainOrMask>
  auto operator| (const Image<I>& ima, const DomainOrMask& other)
    -> decltype( make_subimage(exact(ima), other) )
  {
    return make_subimage(exact(ima), other);
  }

  template <typename I, typename DomainOrMask>
  auto operator| (Image<I>& ima, const DomainOrMask& other)
    -> decltype( make_subimage(exact(ima), other) )
  {
    return make_subimage(exact(ima), other);
  }

  template <typename I, typename DomainOrMask>
  auto operator| (const Image<I>&& ima, const DomainOrMask& other)
    -> decltype( make_subimage(move_exact(ima), other) )
  {
    return make_subimage(move_exact(ima), other);
  }

  template <typename I, typename DomainOrMask>
  auto operator| (Image<I>&& ima, const DomainOrMask& other)
    -> decltype( make_subimage(move_exact(ima), other) )
  {
    return make_subimage(move_exact(ima), other);
  }



  /******************************************/
  /****              Traits              ****/
  /******************************************/


  template <typename Image, typename Domain>
  struct image_traits< sub_image<Image, Domain> >
  {
    typedef std::true_type      accessible;
    typedef forward_image_tag   category; // FIXME: category depends on domain category
    typedef typename image_traits<Image>::concrete  concrete;
    typedef std::false_type	indexable; // FIXME: depends
    typedef mln::extension::none_extension_tag extension;
  };



  template <typename I, typename Domain>
  struct sub_image : image_base< sub_image<I, Domain>,
                                 typename range_value<Domain>::type,
                                 typename image_value<typename std::remove_reference<I>::type>::type >
  {
    BOOST_CONCEPT_ASSERT((AccessibleImage<typename std::decay<I>::type>));

  private:
    typedef typename std::remove_reference<I>::type  image_t;
    typedef sub_image<I, Domain>		     this_type;

    static_assert( std::is_convertible<typename range_value<Domain>::type, typename image_t::point_type>::value,
		"Domain's site type must be convertible to image's site type." );


  public:
    typedef typename range_value<Domain>::type                  point_type;
    typedef typename image_value<image_t>::type                 value_type;
    typedef typename image_reference<image_t>::type             reference;
    typedef typename image_const_reference<image_t>::type       const_reference;
    typedef Domain						domain_type;

    typedef image_access_value_iterator<image_t, typename Domain::iterator>             value_iterator;
    typedef image_access_value_iterator<const image_t, typename Domain::iterator>       const_value_iterator;
    typedef image_access_pixel_iterator<image_t, typename Domain::iterator, this_type>             pixel_iterator;
    typedef image_access_pixel_iterator<const image_t, typename Domain::iterator, const this_type>       const_pixel_iterator;

    typedef typename pixel_iterator::value_type          pixel_type;
    typedef typename const_pixel_iterator::value_type    const_pixel_type;

    typedef iterator_range<value_iterator>		value_range;
    typedef iterator_range<const_value_iterator>	const_value_range;
    typedef iterator_range<pixel_iterator>		pixel_range;
    typedef iterator_range<const_pixel_iterator>	const_pixel_range;

    sub_image(I&& ima, const Domain& domain)
      : m_ima(std::forward<I>(ima)), m_domain(domain)
    {
    }

    template <typename dummy = void>
    sub_image(typename std::enable_if< not std::is_reference<I>::value, dummy >::type* = NULL )
    {
    }

    template <typename OtherImage, typename OtherDomain>
    void resize(const sub_image<OtherImage, OtherDomain>& other)
    {
      resize(m_ima, other.m_ima);
      m_domain = other.m_domain;
    }

    const Domain& domain() const
    {
      return m_domain;
    }

    value_range values()
    {
      return make_iterator_range( value_iterator(m_ima, m_domain.iter()) );
    }

    const_value_range values() const
    {
      return make_iterator_range( const_value_iterator(m_ima, m_domain.iter()) );
    }

    pixel_range pixels()
    {
      return make_iterator_range( pixel_iterator(m_ima, m_domain.iter(), *this) );
    }

    const_pixel_range pixels() const
    {
      return make_iterator_range( const_pixel_iterator(m_ima, m_domain.iter(), *this) );
    }

    reference
    operator() (const point_type& p)
    {
      return m_ima(p);
    }

    const_reference
    operator() (const point_type& p) const
    {
      return m_ima(p);
    }

    reference
    at (const point_type& p)
    {
      return m_ima.at(p);
    }

    const_reference
    at (const point_type& p) const
    {
      return m_ima.at(p);
    }


  private:
    I           m_ima;
    Domain      m_domain;
  };


  template <typename I, typename Domain>
  struct image_concrete< sub_image<I, Domain> >
  {
    typedef sub_image<mln_concrete(typename std::remove_reference<I>::type), Domain> type;
  };

  template <typename I, typename Domain, typename V>
  struct image_ch_value< sub_image<I, Domain>, V >
  {
    typedef sub_image<mln_ch_value(typename std::remove_reference<I>::type, V), Domain> type;
  };


  template <typename I, typename J, typename D1, typename D2>
  void resize(sub_image<I, D1>& out, const sub_image<J, D2>& ima)
  {
    out.resize(ima);
  }


} // end of namespace mln

# include <mln/core/image/sub_image.spe.hpp>

#endif //!MLN_CORE_IMAGE_SUB_IMAGE_HPP
