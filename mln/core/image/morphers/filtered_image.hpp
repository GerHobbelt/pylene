#ifndef MLN_CORE_IMAGE_MORPHERS_FILTERED_IMAGE_HPP
# define MLN_CORE_IMAGE_MORPHERS_FILTERED_IMAGE_HPP

# include <mln/core/image/morphers/morpher_base.hpp>
# include <mln/core/pixel_utility.hpp>

namespace mln
{

  template <typename I, class Predicate>
  struct filtered_image;

  template <typename I, class Predicate>
  filtered_image<I, Predicate>
  imfilter(Image<I>&& ima, const Predicate& pred);

  template <typename I, class Predicate>
  filtered_image<const I&, Predicate>
  imfilter(const Image<I>& ima, const Predicate& pred);

  template <typename I, class Predicate>
  filtered_image<I&, Predicate>
  imfilter(Image<I>& ima, const Predicate& pred);


  /******************************************/
  /****              Traits              ****/
  /******************************************/

  template <typename I, class Predicate>
  struct image_traits< filtered_image<I, Predicate> >
  {
  private:
    typedef typename std::remove_reference<I>::type image_t;

  public:
    typedef std::false_type                     concrete;
    typedef typename std::common_type<typename image_traits<image_t>::category,
                                      bidirectional_image_tag>::type category;
    typedef std::true_type                      accessible;
    typedef std::false_type                     indexable;
    typedef mln::extension::none_extension_tag  extension;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace internal
  {

    template <typename I, class Predicate>
    struct use_pix_helper
    {
      template <typename U>
      static
      std::true_type foo(const U&, decltype( std::declval<Predicate>() (std::declval<U>()) )* = NULL);

      static
      std::false_type foo(...);

      typedef decltype(foo(std::declval<mln_pixel(I)>())) type;
    };

  }

  template <typename I, class Predicate, bool use_pix = internal::use_pix_helper<I, Predicate>::type::value >
  struct filtered_image_base;

  template <typename I, class Predicate>
  struct filtered_image_base<I, Predicate, true>
    : morpher_base< filtered_image<I, Predicate>, I, mln_point(I), mln_value(I) >
  {
  private:
    typedef typename std::remove_reference<I>::type image_t;

    static_assert(image_traits<image_t>::accessible::value, "Image must be accessible.");
    typedef filtered_image<I, Predicate> self_t;

    typedef transform_iterator<
      filter_iterator<typename image_pixel_iterator<image_t>::type, Predicate>,
      get_pixel_value > value_iterator;

    typedef transform_iterator<
      filter_iterator<typename image_const_pixel_iterator<image_t>::type, Predicate>,
      get_pixel_value > const_value_iterator;

    typedef rebind_pixel_iterator<
      self_t,
      filter_iterator<typename image_pixel_iterator<image_t>::type, Predicate>
      > pixel_iterator;

    typedef rebind_pixel_iterator<
      const self_t,
      filter_iterator<typename image_const_pixel_iterator<image_t>::type, Predicate>
      > const_pixel_iterator;


  public:
    typedef internal::where_t<const image_t&, Predicate>        domain_type;

    typedef typename image_t::point_type                        point_type;
    typedef typename image_reference<image_t>::type             reference;
    typedef typename image_const_reference<image_t>::type       const_reference;
    typedef rebinded_pixel<self_t, typename image_t::pixel_type>                pixel_type;
    typedef rebinded_pixel<const self_t, typename image_t::const_pixel_type>    const_pixel_type;
    typedef iterator_range<value_iterator>                      value_range;
    typedef iterator_range<const_value_iterator>                const_value_range;
    typedef iterator_range<pixel_iterator>                      pixel_range;
    typedef iterator_range<const_pixel_iterator>                const_pixel_range;


    filtered_image_base(I&& ima, const Predicate& pred)
      : m_ima(std::forward<I>(ima)),
      m_pred(pred),
      m_domain(m_ima, m_pred)
        {
        }

    const domain_type& domain() const
    {
      return m_domain;
    }


    const_value_range values() const
    {
      return make_iterator_range
        (const_value_iterator
         (make_filter_iterator(m_ima.pixels().iter(), m_pred),
          get_pixel_value() ));
    }

    value_range values()
    {
      return make_iterator_range
        (value_iterator
         (make_filter_iterator(m_ima.pixels().iter(), m_pred),
          get_pixel_value() ));
    }


    const_pixel_range pixels() const
    {
      return make_iterator_range
        (const_pixel_iterator
         (static_cast<const self_t&>(*this),
          make_filter_iterator(m_ima.pixels().iter(), m_pred)));
    }

    pixel_range pixels()
    {
      return make_iterator_range
        (pixel_iterator
         (static_cast<self_t&>(*this),
          make_filter_iterator(m_ima.pixels().iter(), m_pred)));
    }


    const_reference     operator() (const point_type& p) const
    {
      mln_precondition(m_domain.has(p));
      return m_ima(p);
    }

    reference           operator() (const point_type& p)
    {
      mln_precondition(m_domain.has(p));
      return m_ima(p);
    }

    const_reference     at (const point_type& p) const
    {
      return m_ima.at(p);
    }

    reference           at (const point_type& p)
    {
      return m_ima.at(p);
    }



  private:
    I           m_ima;
    Predicate   m_pred;
    domain_type m_domain;
  };



  template <typename I, class Predicate>
  struct filtered_image : filtered_image_base<I, Predicate>
  {
    filtered_image(I&& ima, const Predicate& pred)
      : filtered_image_base<I, Predicate>(std::forward<I>(ima), pred)
    {
    }

  };


  template <typename I, class Predicate>
  filtered_image<I, Predicate>
  imfilter(Image<I>&& ima, const Predicate& pred)
  {
    return filtered_image<I, Predicate>(move_exact(ima), pred);
  }

  template <typename I, class Predicate>
  filtered_image<const I&, Predicate>
  imfilter(const Image<I>& ima, const Predicate& pred)
  {
    return filtered_image<const I&, Predicate>(exact(ima), pred);
  }

  template <typename I, class Predicate>
  filtered_image<I&, Predicate>
  imfilter(Image<I>& ima, const Predicate& pred)
  {
    return filtered_image<I&, Predicate>(exact(ima), pred);
  }


} // end of namespace mln

#endif //!MLN_CORE_IMAGE_MORPHERS_FILTERED_IMAGE_HPP
