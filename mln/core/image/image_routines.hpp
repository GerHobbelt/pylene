#ifndef MLN_CORE_IMAGE_IMAGE_ROUTINES_HPP
# define MLN_CORE_IMAGE_IMAGE_ROUTINES_HPP

# include <mln/core/algorithm/clone.hpp>
# include <mln/core/internal/get_border_from_nbh.hpp>
# include <mln/core/iterator/transform_iterator.hpp>
# include <mln/core/iterator/filter_iterator.hpp>

# include <boost/mpl/or.hpp>
# include <boost/mpl/and.hpp>


namespace mln
{

  /// \defgroup free_functions Free functions
  /// \ingroup image
  /// \{


  /// \fn void reindex(Image<I>& ima1, const Image<J>& ima2);
  /// \brief Reindex \p ima1 to match indexes of ima2.
  ///
  /// It ensures that after reindexation, \p ima1.domain().pmin and \p ima2.domain().pmin
  /// have the same index. If \p ima1 and/or \p ima2 are not indexable, the function has no effect.
  ///
  /// Reindexation can be usefull when using view e.g. subimage and still be able to use indexes.
  ///
  /// \param ima1 The image to be reindexed.
  /// \param ima2 The reference image.
  ///
  ///
  template <typename I, typename J>
  typename std::enable_if<image_traits<I>::indexable::value and image_traits<J>::indexable::value>::type
  reindex(Image<I>& ima1, const Image<J>& ima2);

  template <typename I, typename J>
  bool are_indexes_compatible(const Image<I>& f, const Image<J>& g);


  template <typename O, typename I, typename has_border = typename image_traits<O>::has_border>
  struct resizer;


  /// \brief Initialize an image from an other such that
  /// domains correspond. If \p ima and \p other are indexable
  /// indexes match as well.
  ///
  /// \param ima Image to be initialized.
  /// \param other Image to initialize with.
  ///
  template <typename I, typename J>
  resizer<I,J>
  resize(Image<I>& ima, const Image<J>& other);

  template <typename I, typename J>
  void
  resize(Image<I>& ima, const Image<J>& other, unsigned border, const mln_value(I)& v) __attribute__ ((deprecated));


  /// \brief Return a concrete version of the image.
  ///
  /// \p eval returns a concrete version of \p ima. If
  /// If \p ima is already concrete then it returns the
  /// image unmodified. Otherwise, it clones the content of
  /// \p ima.
  ///
  /// \param ima Image to set concrete
  ///
  /// \return A concrete duplicate of the image or the image itself.
  ///
  template <typename InputImage>
  typename std::enable_if< image_traits<InputImage>::concrete::value, InputImage&& >::type
  eval(InputImage&& ima);


  namespace internal
  {
    // FWD declaration
    template <typename I, class Predicate, class Enable = bool>
    struct where_t;

    template <typename I>
    struct where_binary_t;
  };


  /// \brief Return the domain of the image where the predicate is true.
  ///
  /// \param ima The input image \f$E \rightarrow V\f$
  ///
  /// \param pred A unary predicate that takes either the pixel or the pixel
  /// value as argument i.e \f$V \rightarrow T\f$ or \f$(E,V) \rightarrow T\f$, where 
  /// \p T is convertible to bool.
  ///
  /// \return The subdomain of the image where the predicate is true. The result
  /// type fits the Domain concept.
  ///
  template <typename I, class Predicate>
  internal::where_t<const I&, Predicate>
  where(const Image<I>& ima, const Predicate& pred);

  template <typename I, class Predicate>
  internal::where_t<I, Predicate>
  where(Image<I>&& ima, const Predicate& pred);


  /// \brief  Return the domain of the image where it is true.
  ///
  /// \param ima The input image
  /// \return The subdomain of the image where it is true. The result
  /// type fits the Domain concept.
  /// \pre The value type of the image must be convertible to bool.
  template <typename I>
  internal::where_binary_t<const I&>
  where(const Image<I>& ima);

  template <typename I>
  internal::where_binary_t<I>
  where(Image<I>&& ima);
  ///\}


  /*************************/
  /***  Implementation   ***/
  /*************************/


  template <typename I, typename J>
  inline
  typename std::enable_if<image_traits<I>::indexable::value and image_traits<J>::indexable::value>::type
  reindex(Image<I>& ima, const Image<J>& ima2)
  {
    mln_piter(p, exact(ima2));
    p.init();
    auto pmin = *p;
    exact(ima).reindex(exact(ima2).index_of_point(pmin));
  }

  template <typename I, typename J>
  inline
  typename std::enable_if<!(image_traits<I>::indexable::value and image_traits<J>::indexable::value)>::type
  reindex(Image<I>&, const Image<J>&)
  {
  }

  template <typename I, typename J>
  inline
  resizer<I, J>
  resize(Image<I>& ima, const Image<J>& other)
  {
    return std::move(resizer<I, J>(exact(ima), exact(other)));
  }

  template <typename I, typename J>
  inline
  void
  resize(Image<I>& ima, const Image<J>& other, unsigned b, const mln_value(I)& v)
  {
    resize(ima, other).border(b).init(v);
  }

  namespace internal
  {

    template <typename... I>
    inline
    constexpr
    typename
    std::enable_if< not boost::mpl::and_<typename image_traits<I>::indexable ...>::value,
		    bool >::type
    are_indexes_compatible(const I&...)
    {
      return false;
    }


    template <typename I, typename J, typename... R>
    inline
    typename
    std::enable_if< boost::mpl::and_<
		      typename image_traits<I>::indexable,
		      typename image_traits<J>::indexable,
		      typename image_traits<R>::indexable...>::value,
		    bool>::type
    are_indexes_compatible(const I& f, const J& g, const R&... rest)
    {
      return f.is_index_compatible(g) and are_indexes_compatible(f, rest...);
    }

  }


  template <typename I, typename J>
  inline
  bool
  are_indexes_compatible(const Image<I>& f, const Image<J>& g)
  {
    return false;
    //return internal::are_indexes_compatible(exact(f), exact(g));
  }


  template <typename InputImage>
  inline
  typename std::enable_if< image_traits<InputImage>::concrete::value, InputImage&& >::type
  eval(InputImage&& ima)
  {
    return std::forward<InputImage>(ima);
  }

  template <typename InputImage>
  inline
  typename std::enable_if< !image_traits<InputImage>::concrete::value,
			   mln_concrete(typename std::remove_reference<InputImage>::type)>::type
  eval(InputImage&& ima)
  {
    return clone(std::forward<InputImage>(ima));
  }

  template <typename O, typename I>
  struct resizer<O, I, std::true_type>
  {
    static_assert(image_traits<O>::concrete::value, "Output image must be concrete.");

    resizer(O& ima, const I& ref)
      : m_ima (ima), m_ref(ref), m_has_hook (true), m_failed(false), m_set_init(false)
    {
      initborder_();
      //m_border = m_ref.border();
    }

    ~resizer()
    {
      do_it();
    }

    resizer(resizer&& other)
    : m_ima(other.m_ima),
      m_ref(other.m_ref),
      m_has_hook(true),
      m_failed(other.m_failed),
      m_border(other.m_border),
      m_set_init(other.m_set_init),
      m_init_value(other.m_init_value)
    {
      other.m_has_hook = false;
    }

    resizer&
    border(unsigned b)
    {
      // if (m_border != -1)
      //   m_border = b;
      // else
      //   m_failed = 1;
      if ((int)b > m_border)
	m_border = b;
      return *this;
    }

    template <typename Neighborhood>
    resizer&
    adjust(const Neighborhood& nbh)
    {
      unsigned b = internal::get_border_from_nbh(nbh);
      if (b != 0) // before: and m_border != -1 ? WHY (-1 just means UNINITIALIZED) 
	m_border = std::max<int>(m_border, b);
      else
	m_failed = true;
      return *this;
    }

    resizer&
    init(const mln_value(O)& v)
    {
      m_set_init = true;
      m_init_value = v;
      return *this;
    }

    operator bool ()
    {
      do_it();
      return !m_failed;
    }

  private:
    void do_it()
    {
      if (m_has_hook)
	{
	  if (m_set_init and m_border != -1)
	    m_ima.resize(m_ref.domain(), m_border, m_init_value);
	  else if (m_border != -1)
	    m_ima.resize(m_ref.domain(), m_border);
	  else if (m_set_init)
	    m_ima.resize(m_ref.domain(), 3, m_init_value);
	  else
	    m_ima.resize(m_ref.domain());
	  reindex(m_ima, m_ref);
	  m_has_hook = false;
	}
    }

    template <typename dummy=void>
    typename std::enable_if< std::is_same<typename image_extension<I>::type,
                                          mln::extension::border_extension_tag>::value, dummy>::type
    initborder_()
    {
      m_border = m_ref.border();
    }

    template <typename dummy=void>
    typename std::enable_if< !std::is_same<typename image_extension<I>::type,
                                          mln::extension::border_extension_tag>::value, dummy>::type
    initborder_()
    {
      m_border = -1;
    }


  private:
    resizer(const resizer&);
    resizer& operator= (const resizer&);

    O&		m_ima;
    const I&	m_ref;
    bool	m_has_hook;
    bool	m_failed;

    int		 m_border;
    bool	 m_set_init;
    mln_value(O) m_init_value;
  };

  template <typename O, typename I>
  struct resizer<O, I, std::false_type>
  {
    static_assert(image_traits<O>::concrete::value, "Output image must be concrete.");

    resizer(O& ima, const I& ref)
      : m_ima (ima), m_ref(ref)
    {
    }

    ~resizer()
    {
      if (m_has_hook)
	{
	  if (m_set_init)
	    m_ima.resize(m_ref.domain(), m_init_value);
	  else
	    m_ima.resize(m_ref.domain());
	}
      reindex(m_ima, m_ref);
    }

    resizer(resizer&& other)
    : m_ima(other.m_ima),
      m_ref(other.m_ref),
      m_has_hook(true),
      m_set_init(other.m_set_init),
      m_init_value(other.m_init_value)
    {
      other.m_has_hook = false;
    }


    resizer&
    init(const mln_value(O)& v)
    {
      m_set_init = true;
      m_init_value = v;
      return *this;
    }


  private:
    resizer(const resizer&);
    resizer& operator= (const resizer&);

    O&		 m_ima;
    const I&	 m_ref;
    bool	 m_has_hook;

    bool	 m_set_init;
    mln_value(O) m_init_value;
  };


  namespace internal
  {

    // FIXME: result_of still produces a HARD error, simplify as soon as it produces a SFINAE instead
    template <typename I, typename Predicate>
    struct where_t<I, Predicate, decltype( std::declval<Predicate>() (std::declval<mln_pixel(I)>()) )>
    {
    private:
      typedef typename std::remove_reference<I>::type                 image_t;
      typedef typename image_const_pixel_iterator<image_t>::type      IT;

      struct getpoint
      {
        mln_point(I)
        operator() (const mln_pixel(const I)& pix) const
        {
          return pix.point();
        }
      };

    public:
      typedef transform_iterator< filter_iterator<IT, Predicate>, getpoint> iterator;
      typedef iterator const_iterator;

      where_t(I&& ima, const Predicate& pred)
        : m_ima(std::forward<I>(ima)),
          m_pred(pred)
      {

      }

      where_t(const where_t&) = default;

      iterator iter() const
      {
        return iterator( filter_iterator<IT, Predicate>(m_ima.pixels().iter(), m_pred), getpoint() );
      }

      bool has(const mln_point(I)& p) const
      {
        return m_ima.domain().has(p) and m_pred(m_ima.pixel(p));
      }


    private:
      I                 m_ima;
      Predicate         m_pred;
    };


    template <typename I, typename Predicate>
    struct where_t<I, Predicate, decltype( std::declval<Predicate>() (std::declval<mln_value(I)>()) )>
    {
    private:
      typedef typename std::remove_reference<I>::type                 image_t;
      typedef typename image_const_pixel_iterator<image_t>::type      IT;

      struct getpoint
      {
        mln_point(I)
        operator() (const mln_pixel(const I)& pix) const
        {
          return pix.point();
        }
      };

      struct predicate_t
      {
        bool
        operator() (const mln_pixel(const I)& px) const
        {
          return pred(px.val());
        }

        Predicate pred;
      };

    public:
      typedef transform_iterator< filter_iterator<IT, predicate_t>, getpoint> iterator;
      typedef iterator const_iterator;

      where_t(I&& ima, const Predicate& pred)
        : m_ima(std::forward<I>(ima)),
          m_pred{pred}
      {
      }

      where_t(const where_t&) = default;

      iterator iter() const
      {
        return iterator( filter_iterator<IT, predicate_t>(m_ima.pixels().iter(), m_pred), getpoint() );
      }

      bool has(const mln_point(I)& p) const
      {
        return m_ima.domain().has(p) and m_pred.pred(m_ima(p));
      }


    private:
      I                   m_ima;
      predicate_t         m_pred;
    };


    template <typename I>
    struct where_binary_t
    {
    private:
      typedef typename std::remove_reference<I>::type                 image_t;
      typedef typename image_const_pixel_iterator<image_t>::type      IT;

      struct getpoint
      {
        mln_point(I)
        operator() (const mln_pixel(const I)& pix) const
        {
          return pix.point();
        }
      };

      struct predicate_t
      {
        bool
        operator() (const mln_pixel(const I)& px) const
        {
          return px.val();
        }
      };

    public:
      typedef transform_iterator< filter_iterator<IT, predicate_t>, getpoint> iterator;
      typedef iterator const_iterator;

      where_binary_t(I&& ima)
        : m_ima(std::forward<I>(ima))
      {
      }

      where_binary_t(const where_binary_t&) = default;

      iterator iter() const
      {
        return iterator( filter_iterator<IT, predicate_t>(m_ima.pixels().iter(), predicate_t() ), getpoint() );
      }

      bool has(const mln_point(I)& p) const
      {
        return m_ima.domain().has(p) and m_ima(p);
      }


    private:
      I m_ima;
    };

  }


  template <typename I, class Predicate>
  internal::where_t<const I&, Predicate>
  where(const Image<I>& ima, const Predicate& pred)
  {
    return internal::where_t<const I&, Predicate>(exact(ima), pred);
  }

  template <typename I>
  internal::where_binary_t<const I&>
  where(const Image<I>& ima)
  {
    return internal::where_binary_t<const I&>(exact(ima));
  }

  template <typename I, class Predicate>
  internal::where_t<I, Predicate>
  where(Image<I>&& ima, const Predicate& pred)
  {
    return internal::where_t<I, Predicate>(move_exact(ima), pred);
  }

  template <typename I>
  internal::where_binary_t<I>
  where(Image<I>&& ima)
  {
    return internal::where_binary_t<I>(move_exact(ima));
  }


}

#endif // !MLN_CORE_IMAGE_IMAGE_ROUTINES_HPP
