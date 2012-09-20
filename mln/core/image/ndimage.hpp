#ifndef MLN_CORE_IMAGE_NDIMAGE_HH
# define MLN_CORE_IMAGE_NDIMAGE_HH


# include <mln/core/image_base.hpp>

# include <mln/core/domain/box.hpp>
# include <mln/core/memory.hpp>
# include <mln/core/assert.hpp>
# include <mln/core/image_traits.hpp>
# include <mln/core/image_category.hpp>

# include <boost/range/iterator_range.hpp>

# include <mln/core/image/ndimage_iter.hpp>
# include <mln/core/image/ndimage_pixel_iterator.hpp>

namespace mln
{
  // FWD
  //template <typename I, typename T> struct ndimage_iter;
  //template <typename I, typename T> struct ndimage_rev_iter;
  //template <typename T, unsigned dim, typename I> struct ndimage_pixel_iterator;
  //template <typename T, unsigned dim, typename I> struct ndimage_rev_pixel_iterator;
  template <typename T, unsigned dim, typename I> struct ndimage_pixel;
  template <typename T, unsigned dim, typename E> struct ndimage_base;

/******************************************/
/****              Traits              ****/
/******************************************/


  template <typename T, unsigned dim, typename E>
  struct image_traits< ndimage_base<T, dim, E> >
  {
    typedef raw_image_tag               category;
    typedef std::true_type              accessible;
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


  template <typename T, unsigned dim, typename E>
  struct ndimage_base : image_base<E, point<short, dim>, T,
                                   ndimage_pixel<T, dim, E>,
                                   ndimage_pixel<const T, dim, const E>
                                   >
  {
    BOOST_CONCEPT_ASSERT((IterableImage<E>));
    BOOST_CONCEPT_ASSERT((AccessibleImage<E>));

  private:
    typedef ndimage_base<T, dim, E>                             this_type;
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
    typedef size_t		size_type;


    // As an Image
    // \group Constructors
    // \{
    explicit ndimage_base(unsigned border = 3);
    explicit ndimage_base(const domain_type& domain, unsigned border = 3);
    // \}

    const domain_type&  domain() const;

    // As an ContainerImage
    // \group Point-wise access
    // \{
    reference operator() (site_type p);
    const_reference operator() (site_type p) const;
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
    void resize(const domain_type& domain, unsigned border = 3);

    // As a RandomAccessImage
    reference at (difference_type n);
    const_reference at (difference_type n) const;

    // As an IterableImage
    typedef ndimage_iter<this_type, T>                                  value_iterator;
    typedef ndimage_iter<const this_type, const T>                      const_value_iterator;
    typedef boost::reverse_iterator<value_iterator>                     reverse_value_iterator;
    typedef boost::reverse_iterator<const_value_iterator>               const_reverse_value_iterator;
    typedef ndimage_pixel_iterator<T, dim, E>                           pixel_iterator;
    typedef ndimage_pixel_iterator<const T, dim, const E>               const_pixel_iterator;
    typedef boost::reverse_iterator<pixel_iterator>                     reverse_pixel_iterator;
    typedef boost::reverse_iterator<const_pixel_iterator>               const_reverse_pixel_iterator;

    typedef boost::iterator_range<value_iterator>                                   value_range;
    typedef boost::iterator_range<const_value_iterator>                       const_value_range;
    typedef boost::iterator_range<pixel_iterator>                                   pixel_range;
    typedef boost::iterator_range<const_pixel_iterator>                       const_pixel_range;


    value_range         values();
    const_value_range   values() const;


    pixel_range                 pixels();
    const_pixel_range           pixels() const;





    // As a Raw Image
    const size_t*       strides() const;
    int border() const { return border_; }

  protected:
    friend struct ndimage_pixel<T, dim, E>;
    friend struct ndimage_pixel<const T, dim, const E>;

    //friend struct ndimage_iter<this_type, T>;
    //friend struct ndimage_iter<this_type, const T>;

    //friend struct ndimage_pixel_iterator<T, dim, E>;
    //friend struct ndimage_pixel_iterator<const T, dim, const E>;


    domain_type	domain_;	///< Domain of image
    std::array<size_t, dim>	strides_;	///< Strides in bytes
    std::shared_ptr< internal::ndimage_data<T, dim> > data_;
    int		border_;
    char*	ptr_;           ///< Pointer to the first element
    char*	last_;          ///< Pointer to the last element
  };


  template <typename T>
  struct image3d : ndimage_base<T, 3, image3d<T> >
  {
  protected:
    typedef ndimage_base<T, 3, image3d<T> > base;
    typedef typename base::domain_type domain_type;

  public:
    explicit image3d (unsigned border = 3) : base (border) {}

    explicit image3d(const domain_type& domain, unsigned border = 3)
      : base(domain, border)
    {
    }

    image3d(short nslices, short nrows, short ncols, unsigned border = 3)
      : base( (box<short,3>) {{{0,0,0}},{{nslices, nrows, ncols}}}, border)
    {
    }
  };


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
      if (ndim >= 2)
	{
	  strides[dim-2] = ((shape[dim-1] * sizeof(T)) & ~(size_t)15) + (size_t) 16;
	  for (int i = dim-3; i >= 0; --i)
	    strides[i] = shape[i+1] * strides[i+1];
	}

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


  template <typename T, unsigned dim, typename E>
  ndimage_base<T,dim,E>::ndimage_base(unsigned border)
    : domain_ (), border_ (border), ptr_ (NULL)
  {
    for (unsigned i = 0; i < dim; ++i){
      mln_postcondition(domain_.pmin[i] == 0);
      mln_postcondition(domain_.pmax[i] == 0);
    }
  }

  template <typename T, unsigned dim, typename E>
  ndimage_base<T,dim,E>::ndimage_base(const domain_type& domain, unsigned border)
    : domain_ (domain),
      border_ (border)
  {
    //mln_precondition(domain.size() > 0);
    site_type shp = domain.shape();
    point<size_t, dim> sz;
    sz = shp;

    // Compute strides size (in bytes)
    // The row stride is 16 bytes aligned
    data_.reset(new internal::ndimage_data<T, dim>(&(sz[0]), border));
    std::copy(data_->strides, data_->strides + dim, strides_.begin());

    // Compute pointer at (0,0)
    ptr_ = data_->buffer;
    last_ = data_->buffer;
    for (unsigned i = 0; i < dim; ++i) {
      ptr_ += border * strides_[i];
      last_ += (border + sz[i] - 1) * strides_[i];
    }

  }

  template <typename T, unsigned dim, typename E>
  void
  ndimage_base<T,dim,E>::resize(const domain_type& domain, unsigned border)
  {
    domain_ = domain;
    border_ = border;

    site_type shp = domain.shape();
    point<size_t, dim> sz;
    sz = shp;

    // Compute strides size (in bytes)
    // The row stride is 16 bytes aligned
    data_.reset(new internal::ndimage_data<T, dim>(&(sz[0]), border));
    std::copy(data_->strides, data_->strides + dim, strides_.begin());

    // Compute pointer at (0,0)
    ptr_ = data_->buffer;
    last_ = data_->buffer;
    for (unsigned i = 0; i < dim; ++i) {
      ptr_ += border * strides_[i];
      last_ += (border + sz[i] - 1) * strides_[i];
    }
  }

  template <typename T, unsigned dim, typename E>
  inline
  T&
  ndimage_base<T,dim,E>::operator() (site_type p)
  {
    mln_precondition(domain_.has(p));
    site_type q = p - domain_.pmin;

    char* ptr = ptr_;
    for (unsigned i = 0; i < dim; ++i)
      ptr += q[i] * strides_[i];
    return * (T*)ptr;
  }

  template <typename T, unsigned dim, typename E>
  inline
  const T&
  ndimage_base<T,dim,E>::operator() (site_type p) const
  {
    mln_precondition(domain_.has(p));
    site_type q = p - domain_.pmin;

    const char* ptr = ptr_;
    for (unsigned i = 0; i < dim; ++i)
      ptr += q[i] * strides_[i];
    return * (const T*)ptr;
  }

  template <typename T, unsigned dim, typename E>
  inline
  T&
  ndimage_base<T,dim,E>::at (difference_type n)
  {
    mln_precondition(0 <= n && n < data_->nbytes);
    return *reinterpret_cast<T*>(ptr_+n);
  }

  template <typename T, unsigned dim, typename E>
  inline
  const T&
  ndimage_base<T,dim,E>::at (difference_type n) const
  {
    mln_precondition(0 <= n && n < data_->nbytes);
    return *reinterpret_cast<const T*>(ptr_+n);
  }

  template <typename T, unsigned dim, typename E>
  const size_t*
  ndimage_base<T,dim,E>::strides () const
  {
    return &strides_[0];
  }

  // template <typename T, unsigned dim, typename E>
  // ptrdiff_t
  // ndimage_base<T,dim,E>::offset (point_type dp) const
  // {
  //   ptrdiff_t x = 0;
  //   for (unsigned i = 0; i < dim; ++i)
  //     x += p[i] * strides_[i];
  //   return x;
  // }

  template <typename T, unsigned dim, typename E>
  inline
  const typename ndimage_base<T,dim,E>::domain_type&
  ndimage_base<T,dim,E>::domain () const
  {
    return domain_;
  }

  /* -- Value range -- */

  template <typename T, unsigned dim, typename E>
  inline
  typename ndimage_base<T,dim,E>::const_value_range
  ndimage_base<T,dim,E>::values () const
  {
    point_type shp = domain_.shape();
    size_t sz = shp[0] * strides_[0];
    const_pixel_type pix_begin_(exact(this)), pix_end_(exact(this));
    pix_begin_.ptr_ = ptr_;
    pix_begin_.point_ = (point_type) {{0,}} ;
    pix_end_.ptr_ = ptr_ + sz;
    pix_end_.point_ = (point_type) {{0,}};
    pix_end_.point_[0] = shp[0];

    const_value_iterator begin_( pix_begin_,
                                 internal::make_point_visitor(shp),
                                 internal::make_strided_pointer_value_visitor(ptr_, shp, strides_.begin()));


    const_value_iterator end_(   pix_end_,
                                 internal::make_point_visitor(shp),
                                 internal::make_strided_pointer_value_visitor(ptr_ + sz, shp, strides_.begin()));

    return boost::make_iterator_range(begin_, end_);
  }

  template <typename T, unsigned dim, typename E>
  inline
  typename ndimage_base<T,dim,E>::value_range
  ndimage_base<T,dim,E>::values ()
  {
    point_type shp = domain_.shape();
    size_t sz = shp[0] * strides_[0];
    pixel_type pix_begin_(exact(this)), pix_end_(exact(this));
    pix_begin_.ptr_ = ptr_;
    pix_begin_.point_ = point_type ();
    pix_end_.ptr_ = ptr_ + sz;
    pix_end_.point_ = point_type ();
    pix_end_.point_[0] = shp[0];

    value_iterator begin_( pix_begin_,
                           internal::make_point_visitor(shp),
                           internal::make_strided_pointer_value_visitor(ptr_, shp, strides_.begin()));


    value_iterator end_(   pix_end_,
                           internal::make_point_visitor(shp),
                           internal::make_strided_pointer_value_visitor(ptr_ + sz, shp, strides_.begin()));

    return boost::make_iterator_range(begin_, end_);
  }

  /* -- Value range -- */

  template <typename T, unsigned dim, typename E>
  inline
  typename ndimage_base<T,dim,E>::const_pixel_range
  ndimage_base<T,dim,E>::pixels () const
  {
    point_type shp = domain_.shape();
    size_t sz = shp[0] * strides_[0];
    const_pixel_type pix_begin_(exact(this)), pix_end_(exact(this));
    pix_begin_.ptr_ = ptr_;
    pix_begin_.point_ = domain_.pmin;
    pix_end_.ptr_ = ptr_ + sz;
    pix_end_.point_ = domain_.pmin;
    pix_end_.point_[0] = domain_.pmax[0];

    const_pixel_iterator begin_( pix_begin_,
                                 internal::make_point_visitor(domain_.pmin, domain_.pmax),
                                 internal::make_strided_pointer_value_visitor(ptr_, shp, strides_.begin()));


    const_pixel_iterator end_(   pix_end_,
                                 internal::make_point_visitor(domain_.pmin, domain_.pmax),
                                 internal::make_strided_pointer_value_visitor(ptr_ + sz, shp, strides_.begin()));

    return boost::make_iterator_range(begin_, end_);
  }

  template <typename T, unsigned dim, typename E>
  inline
  typename ndimage_base<T,dim,E>::pixel_range
  ndimage_base<T,dim,E>::pixels ()
  {
    point_type shp = domain_.shape();
    size_t sz = shp[0] * strides_[0];
    pixel_type pix_begin_(exact(this)), pix_end_(exact(this));
    pix_begin_.ptr_ = ptr_;
    pix_begin_.point_ = domain_.pmin;
    pix_end_.ptr_ = ptr_ + sz;
    pix_end_.point_ = domain_.pmin;
    pix_end_.point_[0] = domain_.pmax[0];

    pixel_iterator begin_( pix_begin_,
                           internal::make_point_visitor(domain_.pmin, domain_.pmax),
                           internal::make_strided_pointer_value_visitor(ptr_, shp, strides_.begin()));

    pixel_iterator end_(   pix_end_,
                           internal::make_point_visitor(domain_.pmin, domain_.pmax),
                           internal::make_strided_pointer_value_visitor(ptr_ + sz, shp, strides_.begin()));

    return boost::make_iterator_range(begin_, end_);
  }


  // /* -- Pixel range  */
  // template <typename T, unsigned dim, typename E>
  // inline
  // typename ndimage_base<T,dim,E>::const_pixel_range
  // ndimage_base<T,dim,E>::pixels() const
  // {
  //   size_t sz = (domain_.pmax[0] - domain_.pmin[0]) * strides_[0];
  //   point_type pend = domain_.pmin; pend[0] = domain_.pmax[0];
  //   return boost::make_iterator_range(const_pixel_iterator(reinterpret_cast<const E*>(this), domain_.pmin, ptr_),
  //                                     const_pixel_iterator(reinterpret_cast<const E*>(this), pend, ptr_ + sz));
  // }

  // template <typename T, unsigned dim, typename E>
  // inline
  // typename ndimage_base<T,dim,E>::pixel_range
  // ndimage_base<T,dim,E>::pixels()
  // {
  //   size_t sz = (domain_.pmax[0] - domain_.pmin[0]) * strides_[0];
  //   point_type pend = domain_.pmin; pend[0] = domain_.pmax[0];
  //   return boost::make_iterator_range(pixel_iterator(reinterpret_cast<E*>(this), domain_.pmin, ptr_),
  //                                     pixel_iterator(reinterpret_cast<E*>(this), pend, ptr_ + sz));
  // }


} // end of namespace mln



#endif // !MLN_CORE_IMAGE_NDIMAGE_HH
