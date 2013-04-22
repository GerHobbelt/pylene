#ifndef MORPHER_BASE_HPP
# define MORPHER_BASE_HPP

# include <mln/core/image/image.hpp>


namespace mln
{

  /// \brief Base class for morpher that acts like a mixin w.r.t to I
  ///
  /// 
  template <typename Derived, typename I, typename P, typename V>
  struct morpher_base;

  // FWD declaration
  struct morpher_core_access;


  /***********************/
  /**  Implementation   **/
  /***********************/

  struct morpher_core_access
  {
    template <typename Morpher>
    static
    typename Morpher::image_t&
    get_ima(Morpher* morpher)
    {
      return reinterpret_cast<typename Morpher::derived_t*>(morpher)->m_ima;
    }


    template <typename Morpher>
    static
    const typename Morpher::image_t&
    get_ima(const Morpher* morpher)
    {
      return reinterpret_cast<const typename Morpher::derived_t*>(morpher)->m_ima;
    }
  };


  namespace impl
  {
    /// \brief Default implementation for morphing an accessible image
    template <typename Derived, typename I, bool _is_accessible = image_traits<Derived>::accessible::value>
    struct morpher_accessible
    {
      friend struct mln::morpher_core_access;
    };

    template <typename Derived, typename I>
    struct morpher_accessible<Derived, I, true>
    {
    private:
      typedef I         image_t;
      typedef Derived   derived_t;

    public:
      typedef mln_point(I) point_type;
      typedef mln_point(I) site_type;
      typedef typename image_reference<I>::type		reference;
      typedef typename image_const_reference<I>::type	const_reference;

      reference operator() (const point_type& p)
      {
	return morpher_core_access::get_ima(this) (p);
      }

      const_reference operator() (const point_type& p) const
      {
	return morpher_core_access::get_ima(this) (p);
      }

      reference at(const point_type& p)
      {
	return morpher_core_access::get_ima(this).at(p);
      }

      const_reference at(const point_type& p) const
      {
	return morpher_core_access::get_ima(this).at(p);
      }

      friend struct mln::morpher_core_access;
    };

    /// \brief Default implementation for morphing an accessible image
    template <typename Derived, typename I, bool _is_indexable = image_traits<Derived>::indexable::value>
    struct morpher_indexable
    {
    };

    template <typename Derived, typename I>
    struct morpher_indexable<Derived, I, true>
    {
    private:
      friend  struct morpher_core_access;
      typedef I          image_t;
      typedef Derived    derived_t;

    public:
      typedef typename I::point_type			point_type;
      typedef typename I::size_type			size_type;
      typedef typename I::difference_type		difference_type;
      typedef typename image_reference<I>::type		reference;
      typedef typename image_const_reference<I>::type	const_reference;

      reference operator[] (size_type i)
      {
	return morpher_core_access::get_ima(this) [i];
      }

      const_reference operator[] (size_type i) const
      {
	return morpher_core_access::get_ima(this) [i];
      }

      size_type index_of_point(const point_type& p) const
      {
	return morpher_core_access::get_ima(this).index_of_point(p);
      }

      difference_type delta_index(const point_type& p) const
      {
	return morpher_core_access::get_ima(this).delta_index(p);
      }

      point_type point_at_index(size_type i) const
      {
	return morpher_core_access::get_ima(this).point_at_index(i);
      }
    };


  }

  template <typename Derived, typename I, typename P, typename V>
  struct morpher_base : image_base<Derived, P, V>,
    impl::morpher_accessible<Derived, typename std::remove_reference<I>::type >,
    impl::morpher_indexable<Derived, typename std::remove_reference<I>::type >
  {
  private:
    friend  struct morpher_core_access;
    typedef typename std::remove_reference<I>::type image_t;
    typedef Derived                                 derived_t;

  public:
    typedef typename image_t::value_type	value_type;
    typedef typename image_t::site_type		site_type;
    typedef typename image_t::point_type	point_type;

    typedef typename image_t::domain_type		domain_type;
    typedef typename image_value_range<image_t>::type		value_range;
    typedef typename image_const_value_range<image_t>::type	const_value_range;
    typedef typename image_pixel_range<image_t>::type		pixel_range;
    typedef typename image_const_pixel_range<image_t>::type	const_pixel_range;


    const domain_type& domain() const
    {
      return morpher_core_access::get_ima(this).domain();
    }

    value_range values()
    {
      return morpher_core_access::get_ima(this).values();
    }

    const_value_range values() const
    {
      return morpher_core_access::get_ima(this).values();
    }


    pixel_range pixels()
    {
      return morpher_core_access::get_ima(this).pixels();
    }

    const_pixel_range pixels() const
    {
      return morpher_core_access::get_ima(this).pixels();
    }

  };



}


#endif // ! MORPHER_BASE_HPP
