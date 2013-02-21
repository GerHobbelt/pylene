#ifndef MLN_CORE_IMAGE_IMAGE_ROUTINES_HPP
# define MLN_CORE_IMAGE_IMAGE_ROUTINES_HPP

# include <mln/core/algorithm/clone.hpp>
# include <mln/core/internal/get_border_from_nbh.hpp>

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
  void
  resize(Image<I>& ima, const Image<J>& other, unsigned b, const mln_value(I)& v)
  {
    resize(ima, other).border(b).init(v);
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
      : m_ima (ima), m_ref(ref), m_has_hook (true), m_failed(false)
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
      m_border = b;
      return *this;
    }

    template <typename Neighborhood>
    resizer&
    adjust(const Neighborhood& nbh)
    {
      unsigned b = internal::get_border_from_nbh(nbh);
      if (b != 0)
	m_border = b;
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
	  if (m_set_init and m_border)
	    m_ima.resize(m_ref.domain(), m_border, m_init_value);
	  else if (m_border)
	    m_ima.resize(m_ref.domain(), m_border);
	  else if (m_set_init)
	    m_ima.resize(m_ref.domain(), m_border);
	  reindex(m_ima, m_ref);
	  m_has_hook = false;
	}
    }

    template <typename dummy=void>
    void initborder_(typename std::enable_if<image_traits<I>::has_border::value, dummy>::type* = NULL)
    {
      m_border = m_ref.border();
    }

    template <typename dummy=void>
    void initborder_(typename std::enable_if<!image_traits<I>::has_border::value, dummy>::type* = NULL)
    {
      m_border = 0;
    }


  private:
    resizer(const resizer&);
    resizer& operator= (const resizer&);

    O&		m_ima;
    const I&	m_ref;
    bool	m_has_hook;
    bool	m_failed;

    unsigned	 m_border;
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

}

#endif // !MLN_CORE_IMAGE_IMAGE_ROUTINES_HPP
