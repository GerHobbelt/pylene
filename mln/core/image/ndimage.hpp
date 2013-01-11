#ifndef MLN_CORE_IMAGE_NDIMAGE_HH
# define MLN_CORE_IMAGE_NDIMAGE_HH


# include <mln/core/image_base.hpp>

# include <mln/core/domain/box.hpp>
# include <mln/core/memory.hpp>
# include <mln/core/assert.hpp>
# include <mln/core/image_traits.hpp>
# include <mln/core/image_category.hpp>
# include <mln/core/image/ndimage_iter.hpp>



namespace mln
{

  // FWD
  //template <typename I, typename T> struct ndimage_iter;
  //template <typename I, typename T> struct ndimage_rev_iter;
  //template <typename T, unsigned dim, typename I> struct ndimage_pixel_iterator;
  //template <typename T, unsigned dim, typename I> struct ndimage_rev_pixel_iterator;
  template <typename T, unsigned dim, typename I> struct ndimage_pixel;
  template <typename T, unsigned dim> struct ndimage;

/******************************************/
/****              Traits              ****/
/******************************************/


  template <typename T, unsigned dim>
  struct image_traits< ndimage<T, dim> >
  {
    typedef raw_image_tag               category;
    typedef std::true_type              accessible;
    typedef std::true_type		indexable;
    typedef std::true_type		concrete;
    typedef std::true_type		shallow_copy;
  };


  template <typename T, unsigned dim, typename V>
  struct image_ch_value< ndimage<T, dim>, V >
  {
    typedef ndimage<V, dim> type;
  };


/******************************************/
/****            Definition            ****/
/******************************************/

  namespace internal
  {
    template <typename T, unsigned dim>
    struct ndimage_data
    {
      ndimage_data(size_t* shape_, unsigned border, T v = T());
      ~ndimage_data();

      size_t shape[dim];
      size_t strides[dim];
      size_t nbytes;
      char*  buffer;

    private:
      ndimage_data(const ndimage_data&);
    };
  }


  template <typename T, unsigned dim>
  struct ndimage :
    image_base< ndimage<T, dim>, point<short, dim>, T, ndimage_pixel<T, dim, ndimage<T, dim> >, ndimage_pixel<const T, dim, const ndimage<T, dim> > >
  {
  private:
    typedef image_base< ndimage<T, dim>,
			point<short, dim>, T,
			ndimage_pixel<T, dim, ndimage<T, dim> >,
			ndimage_pixel<const T, dim, const ndimage<T, dim> > > base;
    typedef ndimage<T, dim>                             this_type;
    typedef ndimage<T, dim>                             E;
  public:
    // As an Image

    typedef box<short, dim>					domain_type;
    typedef point<short,dim>                                    site_type;
    typedef point<short,dim>                                    point_type;
    typedef ndimage_pixel<const T, dim, const E>                const_pixel_type;
    typedef ndimage_pixel<T, dim, E>                            pixel_type;
    typedef T							value_type;
    enum { ndim = dim};

    // As a ContainerImage
    typedef T&			reference;
    typedef const T&		const_reference;
    typedef T*			pointer;
    typedef const T*		const_pointer;
    typedef ptrdiff_t		difference_type;
    typedef unsigned		index_type;
    typedef unsigned		size_type;



    // As an Image
    // \group Constructors
    // \{
    explicit ndimage(unsigned border = 3);
    explicit ndimage(const domain_type& domain, unsigned border = 3, T v = T());

    template <typename = typename std::enable_if<dim == 2>::type>
    ndimage(short nrows, short ncols, unsigned border = 3, T v = T()) :
      domain_ {{0,0},{nrows, ncols}},
      border_ (border)
    {
      resize_(domain_, border, v);
    }
    // \}

    const domain_type&  domain() const;

    // As an ContainerImage
    // \group Point-wise access
    // \{

    using base::operator ();
    using base::at;

    reference operator() (const site_type& p);
    const_reference operator() (const site_type& p) const;

    // without bound checking
    reference at (const site_type& p);
    const_reference at (const site_type& p) const;
    // \}

    // FIXME move to base
    pixel_type pix_at(site_type p)
    {
      mln_precondition(domain_.has(p));
      pixel_type pix;
      pix.point_ = p;
      pix.ima_  = (E*)this;
      pix.ptr_ = (char*) & (operator () (p));
      return pix;
    }

    const_pixel_type pix_at(site_type p) const
    {
      mln_precondition(domain_.has(p));
      const_pixel_type pix((const E*) this);
      pix.point_ = p;
      pix.ptr_ = (char*) & (operator () (p));
      return pix;
    }


    // As a Resizable Image
    void resize(const domain_type& domain, unsigned border = 3, T v = T());

    // As an IterableImage
    typedef ndimage_value_range<this_type, T>					value_range;
    typedef ndimage_value_range<const this_type, const T>			const_value_range;
    typedef typename value_range::iterator				value_iterator;
    typedef typename value_range::reverse_iterator			reverse_value_iterator;
    typedef typename const_value_range::iterator			const_value_iterator;
    typedef typename const_value_range::reverse_iterator		const_reverse_value_iterator;

    typedef ndimage_pixel_range<this_type, T>					pixel_range;
    typedef ndimage_pixel_range<const this_type, const T>			const_pixel_range;
    typedef typename pixel_range::iterator				pixel_iterator;
    typedef typename pixel_range::reverse_iterator			reverse_pixel_iterator;
    typedef typename const_pixel_range::iterator			const_pixel_iterator;
    typedef typename const_pixel_range::reverse_iterator		const_reverse_pixel_iterator;



    value_range         values();
    const_value_range   values() const;
    pixel_range                 pixels();
    const_pixel_range           pixels() const;


    // As a Raw Image
    const size_t*       strides() const;
    int border() const { return border_; }


    // Specialized algorithm
    template <typename T_, unsigned dim_, typename Domain_>
    friend typename std::enable_if<std::is_convertible<Domain_, typename ndimage<T_, dim_>::domain_type>::value, ndimage<T_, dim_> >::type
    make_subimage(const ndimage<T_, dim_>&, const Domain_& domain);


    // As an Indexable Image
    T&		operator[] (std::size_t i);
    const T&	operator[] (std::size_t i) const;
    const size_t*	index_strides() const     { return &m_index_strides[0]; }

    void reindex(std::size_t index_first)
    {
      std::ptrdiff_t diff = index_first - m_index_first;
      m_ptr_origin -= diff;
      m_index_first += diff;
      m_index_last += diff;
    }


    std::size_t         index_of_point(const point_type& p) const
    {
      std::size_t idx = m_index_first;
      point_type  q = p - domain_.pmin;
      for (unsigned i = 0; i < dim; ++i)
	idx += q[i] * m_index_strides[i];
      return idx;
    }

    point_type		point_at_index(std::size_t idx) const
    {
      int k = idx - m_index_first;
      point_type  p = domain_.pmin;

      for (unsigned i = 0; i < dim; ++i) {
	int q = (k >= 0) ? (int)(k / m_index_strides[i]) : (k / (int)m_index_strides[i] - 1); // because k can be negative
	p[i] += q;
	k -= q * m_index_strides[i];
      }
      mln_postcondition(vbox_.has(p));
      return p;
    }


    template <typename = typename std::enable_if<dim == 2>::type >
    unsigned nrows() const { return this->domain_.pmax[0] - this->domain_.pmin[0]; }

    template <typename = typename std::enable_if<dim == 2>::type >
    unsigned ncols() const { return this->domain_.pmax[1] - this->domain_.pmin[1]; }


  protected:
    friend struct ndimage_pixel<T, dim, E>;
    friend struct ndimage_pixel<const T, dim, const E>;
    template <typename, typename, unsigned, typename> friend struct indexible_ndimage;
    template <typename, typename> friend struct ndimage_value_range;
    template <typename, typename> friend struct ndimage_pixel_range;

    void resize_(const domain_type& domain, unsigned border = 3, T v = T());

    domain_type	domain_;	///< Domain of image
#ifndef MLN_NDEBUG
    domain_type vbox_;
#endif

    std::array<size_t, dim>	strides_;	///< Strides in bytes
    std::shared_ptr< internal::ndimage_data<T, dim> > data_;
    int		border_;
    char*	ptr_;           ///< Pointer to the first element in the domain
    char*	last_;          ///< Pointer to the last element in the domain (not past-the-end)

    T*					m_ptr_origin;		///< Pointer to the first element
    std::array<std::size_t, dim>	m_index_strides;	///< Strides in number of elements (including the border)
    size_t				m_index_first;          ///< index of pmin
    size_t				m_index_last;           ///< index of pmax
  };


  /******************************/
  /** Free function Impl        */
  /******************************/

  namespace internal
  {
    template <typename T, unsigned dim>
    ndimage_data<T, dim>::ndimage_data(size_t* shape_, unsigned border, T v)
    {
      for (unsigned i = 0; i < dim; ++i)
	shape[i] = shape_[i] + 2 * border;

      strides[dim-1] = sizeof(T);

      // Each row / page ... are 16 bytes aligned
      unsigned ndim = dim;

#ifdef MLN_128B_ALIGNMENT
      if (ndim >= 2)
	{
	  strides[dim-2] = ((shape[dim-1] * sizeof(T)) & ~(size_t)15) + (size_t) 16;
	  for (int i = dim-3; i >= 0; --i)
	    strides[i] = shape[i+1] * strides[i+1];
	}
#else
      if (ndim >= 2)
	{
	  strides[dim-2] = shape[dim-1] * sizeof(T);
	  for (int i = dim-3; i >= 0; --i)
	    strides[i] = shape[i+1] * strides[i+1];
	}
#endif

      // Allocate data
      nbytes = shape[0] * strides[0];
      buffer = (char*) mln::aligned_malloc(nbytes, border * sizeof(T));

      // Construct
      {
	char* ptr = buffer;
	unsigned nlines = 1;
	for (unsigned i = 0; i < dim-1; ++i)
	  nlines *= shape[i];
	unsigned nelements = shape[dim-1];

        if (dim > 1) {
          for (unsigned i = 0; i < nlines; ++i, ptr += strides[dim-2]) {
            T* p_ = (T*) ptr;
            for (unsigned j = 0; j < nelements; ++j, ++p_)
              new (p_) T(v);
          }
        } else {
          T* p_ = (T*) ptr;
          for (unsigned j = 0; j < nelements; ++j, ++p_)
            new (p_) T(v);
        }
      }
    }


    template <typename T, unsigned dim>
    ndimage_data<T, dim>::~ndimage_data()
    {
      char* ptr = buffer;
      unsigned nlines = 1;
      for (unsigned i = 0; i < dim-1; ++i)
	nlines *= shape[i];
      unsigned nelements = shape[dim-1];
      if (dim == 1) {
        for (unsigned k = 0; k < nelements; ++k)
          ((T*)ptr + k)->~T();
        //    std::destroy( (T*)ptr, (T*) ptr + nelements);
      } else {
        for (unsigned i = 0; i < nlines; ++i, ptr += strides[dim-2])
          for (unsigned k = 0; k < nelements; ++k)
            ((T*)ptr + k)->~T();
        //std::destroy((T*)ptr, (T*)ptr + nelements);
      }
      mln::aligned_free(buffer);
    }
  }


  template <typename T, unsigned dim>
  ndimage<T,dim>::ndimage(unsigned border)
  : domain_ (), border_ (border), ptr_ (NULL)
  {
    for (unsigned i = 0; i < dim; ++i){
      mln_postcondition(domain_.pmin[i] == 0);
      mln_postcondition(domain_.pmax[i] == 0);
    }
  }

  template <typename T, unsigned dim>
  ndimage<T,dim>::ndimage(const domain_type& domain, unsigned border, T v)
    : domain_ (domain),
      border_ (border)
  {
    resize_(domain_, border, v);
  }

  template <typename T, unsigned dim>
  void
  ndimage<T,dim>::resize_(const domain_type& domain, unsigned border, T v)
  {
    site_type shp = domain.shape();
    point<size_t, dim> sz;
    MLN_EVAL_IF_DEBUG(vbox_ = domain);
    MLN_EVAL_IF_DEBUG(vbox_.pmin -= border);
    MLN_EVAL_IF_DEBUG(vbox_.pmax += border);
    sz = shp;

    // Compute strides size (in bytes)
    // The row stride is 16 bytes aligned
    data_.reset(new internal::ndimage_data<T, dim>(&(sz[0]), border, v));
    std::copy(data_->strides, data_->strides + dim, strides_.begin());

    // Compute pointer at (0,0)
    m_ptr_origin = (T*)data_->buffer;
    m_index_strides[dim-1] = 1;
    m_index_first = border_;
    m_index_last  = border_ + sz[dim-1] - 1;
    ptr_  = data_->buffer + border * strides_[dim-1];
    last_ = data_->buffer + (border + sz[dim-1] - 1) * strides_[dim-1];
    for (int i = dim-2; i >= 0; --i)
      {
	m_index_strides[i] = strides_[i] / sizeof(T);
	m_index_first += border_ * m_index_strides[i];
	m_index_last  += (border_ + sz[i] - 1) * m_index_strides[i];
	ptr_ += border * strides_[i];
	last_ += (border + sz[i] - 1) * strides_[i];
      }
  }


  template <typename T, unsigned dim>
  void
  ndimage<T,dim>::resize(const domain_type& domain, unsigned border, T v)
  {
    domain_ = domain;
    border_ = border;
    resize_(domain_, border, v);
  }

  template <typename T, unsigned dim>
  inline
  T&
  ndimage<T,dim>::at (const site_type& p)
  {
    mln_precondition(vbox_.has(p));

    char* ptr = ptr_;
    for (unsigned i = 0; i < dim; ++i)
      ptr += (p[i] - domain_.pmin[i]) * strides_[i];
    return * (T*)ptr;
  }


  template <typename T, unsigned dim>
  inline
  const T&
  ndimage<T,dim>::at (const site_type& p) const
  {
    mln_precondition(vbox_.has(p));

    char* ptr = ptr_;
    for (unsigned i = 0; i < dim; ++i)
      ptr += (p[i] - domain_.pmin[i]) * strides_[i];
    return * (const T*)ptr;
  }


  template <typename T, unsigned dim>
  inline
  T&
  ndimage<T,dim>::operator() (const site_type& p)
  {
    mln_precondition(domain_.has(p));
    return at(p);
  }

  template <typename T, unsigned dim>
  inline
  const T&
  ndimage<T,dim>::operator() (const site_type& p) const
  {
    mln_precondition(domain_.has(p));
    return at(p);
  }

  template <typename T, unsigned dim>
  inline
  T&
  ndimage<T,dim>::operator[] (std::size_t i)
  {
    mln_precondition(vbox_.has(point_at_index(i)));
    return *(m_ptr_origin + i);
  }

  template <typename T, unsigned dim>
  inline
  const T&
  ndimage<T,dim>::operator[] (std::size_t i) const
  {
    mln_precondition(vbox_.has(point_at_index(i)));
    return *(m_ptr_origin + i);
  }


  // template <typename T, unsigned dim>
  // inline
  // T&
  // ndimage<T,dim>::element (difference_type n)
  // {
  //   return *reinterpret_cast<T*>(ptr_+n);
  // }

  // template <typename T, unsigned dim>
  // inline
  // const T&
  // ndimage<T,dim>::element (difference_type n) const
  // {
  //   return *reinterpret_cast<const T*>(ptr_+n);
  // }


  template <typename T, unsigned dim>
  const size_t*
  ndimage<T,dim>::strides () const
  {
    return &strides_[0];
  }


  // template <typename T, unsigned dim>
  // ptrdiff_t
  // ndimage<T,dim>::offset (point_type dp) const
  // {
  //   ptrdiff_t x = 0;
  //   for (unsigned i = 0; i < dim; ++i)
  //     x += p[i] * strides_[i];
  //   return x;
  // }

  template <typename T, unsigned dim>
  inline
  const typename ndimage<T,dim>::domain_type&
  ndimage<T,dim>::domain () const
  {
    return domain_;
  }

  /* -- Value range -- */

  template <typename T, unsigned dim>
  inline
  typename ndimage<T,dim>::const_value_range
  ndimage<T,dim>::values () const
  {
    return const_value_range(exact(*this));
  }

  template <typename T, unsigned dim>
  inline
  typename ndimage<T,dim>::value_range
  ndimage<T,dim>::values ()
  {
    return value_range(*this);
  }

  template <typename T, unsigned dim>
  inline
  typename ndimage<T,dim>::const_pixel_range
  ndimage<T,dim>::pixels () const
  {
    return const_pixel_range(*this);
  }

  template <typename T, unsigned dim>
  inline
  typename ndimage<T,dim>::pixel_range
  ndimage<T,dim>::pixels ()
  {
    return pixel_range(*this);
  }


} // end of namespace mln



#endif // !MLN_CORE_IMAGE_NDIMAGE_HH
