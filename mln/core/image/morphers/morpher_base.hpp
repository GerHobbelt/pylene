#ifndef MORPHER_BASE_HPP
# define MORPHER_BASE_HPP

# include <mln/core/image/image.hpp>


struct mln
{

  /// \brief Base class for morpher that acts like a mixin w.r.t to I
  ///
  /// 
  template <typename Derived, typename I>
  struct morpher_base;


  struct morpher_core_access;


  /***********************/
  /**  Implementation   **/
  /***********************/

  struct morpher_core_access
  {
    static
    template <typename Derived, typename I, template <typename, typename> class M>
    I& get_ima(M<Derived, I>* morpher)
    {
      return reinterpret_cast<Derived*>(morpher)->m_ima;
    }

    static
    template <typename Derived, typename I, template <typename, typename> class M>
    const I& get_ima(const M<Derived, I>* morpher)
    {
      return reinterpret_cast<const Derived*>(morpher)->m_ima;
    }
  };


  namespace impl
  {
    /// \brief Default implementation for morphing an accessible image
    template <typename Derived, typename I, bool _is_accessible = image_traits<Derived>::accessible::value>
    struct morpher_accessible
    {
    };

    template <typename Derived, typename I>
    struct morpher_accessible<Derived, I, true>
    {
      typedef mln_point(I) point_type;
      typedef mln_point(I) site_type;
      typedef typename image_reference<I>::type		reference;
      typedef typename image_const_reference<I>::type	const_reference;

      reference operator() (const point_type& p)
      {
	return morpher_core_access::get_ima(this) (p);
      }

      const_reference operator() (const point_type& p)
      {
	return morpher_core_access::get_ima(this) (p);
      }

      reference at(const point_type& p)
      {
	return morpher_core_access::get_ima(this).at(p);
      }

      const_reference at(const point_type& p)
      {
	return morpher_core_access::get_ima(this).at(p);
      }

    };

    /// \brief Default implementation for morphing an accessible image
    template <typename Derived, typename I, bool _is_indexable = image_traits<Derived>::indexable::value>
    struct morpher_indexable
    {
    };

    template <typename Derived, typename I>
    struct morpher_indexable<Derived, I, true>
    {
      typedef typename I::point_type			point_type;
      typedef typename I::size_type			size_type;
      typedef typename I::difference_type		difference_type;
      typedef typename image_reference<I>::type		reference;
      typedef typename image_const_reference<I>::type	const_reference;

      reference operator[] (size_type i)
      {
	return morpher_core_access::get_ima(this) [i];
      }

      const_reference operator[] (size_type p) const
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
    morpher_accessible<Derived, typename std::remove_reference<I>::type >,
    morpher_indexable<Derived, typename std::remove_reference<I>::type >
  {
  private:
    typedef typename std::remove_reference<I>::type image_t;

  public:
    typedef typename image_t::value_type	value_type;
    typedef typename image_t::site_type		site_type;
    typedef typename image_t::point_type	point_type;

    typedef typename image_t::domain_type		domain_type;
    typedef typename image_value_range<image_t>::type		value_range;
    typedef typename image_const_value_range<image_t>::type	const_value_range;
    typedef typename image_pixel_range<image_t>::type		value_range;
    typedef typename image_const_pixel_range<image_t>::type	const_value_range;


    const domain_type& domain()
    {
      return morpher_core_access::getima(this).domain();
    }

    value_range values()
    {
      return morpher_core_access::getima(this).values();
    }

    const_value_range values() const
    {
      return morpher_core_access::getima(this).values();
    }


    pixel_range pixels()
    {
      return morpher_core_access::getima(this).pixels();
    }

    const_pixel_range pixels() const
    {
      return morpher_core_access::getima(this).pixels();
    }

  };



}


#endif // ! MORPHER_BASE_HPP
