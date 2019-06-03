#pragma once

/// \file

#include <mln/core/assert.hpp>
#include <mln/core/concept/new/neighborhoods.hpp>
#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/concept/new/values.hpp>
#include <mln/core/domain/box.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/ndimage_iter.hpp>
#include <mln/core/image/private/image_builder.hpp>
#include <mln/core/image/private/ndimage_pixel.hpp>
#include <mln/core/image_category.hpp>
#include <mln/core/image_traits.hpp>
#include <mln/core/memory.hpp>
#include <mln/core/rangev3/multi_span.hpp>

#include <optional>
#include <type_traits>


namespace mln
{

  /// \brief base class for ndimage
  template <typename T, unsigned dim, typename E>
  struct ndimage_base;

  // FWD
  template <typename T, unsigned dim, typename I>
  struct ndimage_pixel;

  /******************************************/
  /****              Traits              ****/
  /******************************************/

  template <typename T, unsigned dim, typename E>
  struct image_traits<ndimage_base<T, dim, E>>
  {
    typedef raw_image_tag                        category;
    typedef std::true_type                       accessible;
    typedef std::true_type                       indexable;
    typedef std::true_type                       concrete;
    typedef std::true_type                       shallow_copy;
    typedef std::true_type                       has_border;
    typedef mln::extension::border_extension_tag extension;
  };

  /******************************************/
  /****            Definition            ****/
  /******************************************/

  namespace internal
  {
    template <typename T, unsigned dim>
    struct ndimage_data
    {
      // Constructor for library managed memory
      ndimage_data(size_t* shape_, unsigned border, T v = T());
      ~ndimage_data();

      // These elements are used to ensure a correct destruction
      size_t shape[dim];
      size_t strides[dim];

      // These elements are used to free memory
      size_t nbytes;
      char*  buffer;

    private:
      ndimage_data(const ndimage_data&);
    };

    template <typename T, unsigned dim>
    struct ndimage_extension
    {

    public:
      using value_type        = T;
      using point_type        = point<short, dim>;
      using support_fill      = std::true_type;
      using support_mirror    = std::true_type;
      using support_periodize = std::false_type; // TODO
      using support_clamp     = std::true_type;  // TODO
      using support_buffer    = std::false_type; // TODO

      ndimage_extension(char* ptr, const std::size_t* strides, const point_type& shp, const box<short, dim>* dom,
                        int border);

      template <typename SE>
      bool                       fit(const SE& se) const;
      constexpr bool             is_finite() const;
      std::optional<std::size_t> size() const;
      const value_type&          value(const point_type& pnt) const;
      void                       fill(const T& v);
      void                       mirror(std::size_t padding = 0);

      // TODO
      void periodize();
      void clamp();
      // template <typename U>
      // void buffer(U&& u);

    private:
      template <unsigned d>
      typename std::enable_if<(d < dim)>::type _fill(char* ptr, const T& v);

      template <unsigned d>
      typename std::enable_if<(d == dim)>::type _fill(char* ptr, const T& v);

      template <unsigned d>
      typename std::enable_if<(d < dim)>::type _fillall(char* ptr, const T& v);

      template <unsigned d>
      typename std::enable_if<(d == dim)>::type _fillall(char* ptr, const T& v);

      template <unsigned d>
      typename std::enable_if<(d < dim)>::type _mirror(char* ptr, std::size_t /*padding*/);

      template <unsigned d>
      typename std::enable_if<(d == dim)>::type _mirror(char* ptr, std::size_t /*padding*/);

      template <unsigned d>
      typename std::enable_if<(d < dim)>::type _copy_line(char* src, char* dest);

      template <unsigned d>
      typename std::enable_if<(d == dim)>::type _copy_line(char* src, char* dest);

    private:
      size_t                 m_strides[dim];
      point_type             m_shp;
      const box<short, dim>* m_dom;
      char*                  m_ptr;
      int                    m_border;
    };

  } // namespace internal

  template <typename T, unsigned dim, typename E>
  struct ndimage_base
#ifndef MLN_DOXYGEN
    : image_base<E, point<short, dim>, T, ndimage_pixel<T, dim, E>, ndimage_pixel<const T, dim, const E>>,
      mln::experimental::Image<E>
#endif
  {
  private:
    using this_type = ndimage_base<T, dim, E>;

    template <class, unsigned, class>
    friend struct ndimage_base;

    template <unsigned d, typename T1, typename E1, typename T2, typename E2>
    friend bool are_indexes_compatible(const ndimage_base<T1, d, E1>& self, const ndimage_base<T2, d, E2>& other);

  public:
    using category[[deprecated]]     = raw_image_tag;
    using category_type              = category;
    using accessible                 = std::true_type;
    using indexable                  = std::true_type;
    using concrete[[deprecated]]     = std::true_type;
    using view                       = std::false_type;
    using shallow_copy[[deprecated]] = std::true_type;
    using has_border[[deprecated]]   = std::true_type;
    using extension_category         = mln::extension::border_extension_tag;
    using concrete_type              = E;
#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <concepts::Value Val>
#else
    template <typename Val>
#endif // PYLENE_CONCEPT_TS_ENABLED
    using ch_value_type = typename image_ch_value<E, Val>::type;

    /// \name Image point/value/pixel types
    /// \{

    /// \copydoc image::site_type
    using site_type[[deprecated]] = point<short, dim>;

    /// \copydoc image::point_type
    using point_type = point<short, dim>;

    /// \copydoc image::pixel_type
    using pixel_type[[deprecated]] = ndimage_pixel<T, dim, E>;
    using new_pixel_type           = details::ndpixel<T, dim>;

    /// \copydoc image::const_pixel_type
    using const_pixel_type[[deprecated]] = ndimage_pixel<const T, dim, const E>;
    using new_const_pixel_type           = details::ndpixel<const T, dim>;

    /// \copydoc image::value_type
    using value_type = T;

    /// \copydoc image::reference
    using reference = T&;

    /// \copydoc image::const_reference
    using const_reference[[deprecated]] = const T&;

    /// \copydoc image::difference_type
    using difference_type[[deprecated]] = int;

    /// \copydoc image::size_type
    // FIXME: switch to signed when concept is consolidated
    using size_type[[deprecated]] = unsigned;
    using index_type              = size_type;

    using pointer       = T*;
    using const_pointer = const T*;

    // Extension
    using extension_type = internal::ndimage_extension<T, dim>;
    /// \}

    /// \name Image Ranges Types
    /// \{

    /// \copydoc image::domain_type
    using domain_type = box<short, dim>;

    /// \copydoc image::value_range
    using value_range[[deprecated]] = ndimage_value_range<this_type, T>;
    using new_value_range           = ranges::multi_span<T, dim>;

    /// \copydoc image::const_value_range
    using const_value_range[[deprecated]] = ndimage_value_range<const this_type, const T>;
    using new_const_value_range           = ranges::multi_span<const T, dim>;

    /// \copydoc image::pixel_range
    using pixel_range[[deprecated]] = ndimage_pixel_range<this_type, T>;
    using new_pixel_range           = details::ndpix_range<T, dim>;

    /// \copydoc image::const_pixel_range
    using const_pixel_range[[deprecated]] = ndimage_pixel_range<const this_type, const T>;
    using new_const_pixel_range           = details::ndpix_range<const T, dim>;
    /// \}

    enum
    {
      ndim = dim
    };

    // As an Image
    // Constructors
    // \{
    explicit ndimage_base(unsigned border = 3);
    explicit ndimage_base(const domain_type& domain, unsigned border = 3, T v = T());

    template <typename U, class E2>
    ndimage_base(const ndimage_base<U, dim, E2>& other, const image_build_params& params);

    template <typename U, class E2>
    ndimage_base(const ndimage_base<U, dim, E2>& other, unsigned border, T v = T());
    // \}

    /// \brief Constructors from external sources
    /// \{
    static E from_buffer(void* buffer, const domain_type& domain, bool copy = false);

    static E from_buffer(void* buffer, const domain_type& domain, const size_t* strides, bool copy = false);
    /// \}

    /// \name Accession Operators
    /// \{

    /// \copydoc image::operator()(const site_type& p) const
    reference operator()(const site_type& p);

    /// \copydoc image::operator()(const site_type& p) const
    const_reference operator()(const site_type& p) const;

    /// \copydoc image::operator[](size_type i) const
    reference operator[](size_type i);

    /// \copydoc image::operator[](size_type i) const
    const_reference operator[](size_type i) const;

    /// \copydoc image::at(const site_type& p) const
    reference at(const site_type& p);

    /// \copydoc image::at(const site_type& p) const
    const_reference at(const site_type& p) const;

    /// \}

    /// \name Pixel utilities
    /// \{

    /// \copydoc image::pixel_at(const point_type&) const
    [[deprecated]] pixel_type pixel_at(const point_type& p);
    new_pixel_type            new_pixel_at(const point_type& p);

    /// \copydoc image::pixel_at(const point_type&) const
    [[deprecated]] const_pixel_type pixel_at(const point_type& p) const;
    new_const_pixel_type            new_pixel_at(const point_type& p) const;

    /// \copydoc image::pixel_at_index(size_type) const
    [[deprecated]] pixel_type pixel_at_index(size_type i);
    // new_pixel_type new_pixel_at_index(size_type i);

    /// \copydoc image::pixel_at_index(size_type) const
    [[deprecated]] const_pixel_type pixel_at_index(size_type i) const;
    // new_const_pixel_type new_pixel_at_index(size_type i) const;

    /// \copydoc image::pixel(const point_type&) const
    [[deprecated]] pixel_type pixel(const point_type& p);
    new_pixel_type            new_pixel(const point_type& p);

    /// \copydoc image::pixel(const point_type&) const
    [[deprecated]] const_pixel_type pixel(const point_type& p) const;
    new_const_pixel_type            new_pixel(const point_type& p) const;

    /// \}

    using value_iterator[[deprecated]]               = typename value_range::iterator;
    using reverse_value_iterator[[deprecated]]       = typename value_range::reverse_iterator;
    using const_value_iterator[[deprecated]]         = typename const_value_range::iterator;
    using const_reverse_value_iterator[[deprecated]] = typename const_value_range::reverse_iterator;
    using pixel_iterator[[deprecated]]               = typename pixel_range::iterator;
    using reverse_pixel_iterator[[deprecated]]       = typename pixel_range::reverse_iterator;
    using const_pixel_iterator[[deprecated]]         = typename const_pixel_range::iterator;
    using const_reverse_pixel_iterator[[deprecated]] = typename const_pixel_range::reverse_iterator;

    /// \name Image Ranges
    /// \{

    /// \copydoc image::domain()
    const domain_type& domain() const;

    /// \copydoc image::values()
    [[deprecated]] value_range values();
    new_value_range            new_values();

    /// \copydoc image::values() const
    [[deprecated]] const_value_range values() const;
    new_const_value_range            new_values() const;

    /// \copydoc image::pixels()
    [[deprecated]] pixel_range pixels();
    new_pixel_range            new_pixels();

    /// \copydoc image::pixels() const
    [[deprecated]] const_pixel_range pixels() const;
    new_const_pixel_range            new_pixels() const;
    /// \}

    /// \name Index-related methods
    /// \{

    /// \copydoc image::index_of_point(const point_type&) const
    size_type index_of_point(const point_type& p) const;

    /// \copydoc image::point_at_index(size_type i) const
    point_type point_at_index(size_type i) const;

    /// \copydoc image::delta_index(const point_type&) const
    difference_type delta_index(const point_type& p) const;

    /// \}

    /// \name Concrete-related Image Methods
    /// \{

    image_builder<concrete_type, E> concretize() const { return {exact(*this)}; }


#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <concepts::Value Val>
#else
    template <typename Val>
#endif // PYLENE_CONCEPT_TS_ENABLED
    image_builder<ch_value_type<Val>, E> ch_value() const
    {
      return {exact(*this)};
    }

    /// \brief Resize the image to fit \p domain.
    ///
    /// Resize the image w.r.t to \p domain and \p border. The image is
    /// initialized with value  \p v.
    /// \warning The old values are not kept.
    /// \param domain The new domain of the image.
    /// \param border The new border of the image.
    /// \param v The initialization value of the image. If \p is given, values
    /// are copy constructed from \p v, otherwise, they are default-initialized.
    void resize(const domain_type& domain, unsigned border = 3, T v = T());

    void resize(const domain_type& domain, const image_build_params& params);

    /// \brief Re-indexation of the image.
    ///
    /// Reindex the image so that the index of the first point is \p
    /// index_first.
    /// \param index_first
    /// \postcondition `this->index_of_point(domain().pmin) == index_first`
    void reindex(size_type index_first)
    {
      mln_precondition(index_first >= m_index_first);
      std::ptrdiff_t diff = index_first - m_index_first;
      m_ptr_origin -= diff;
      m_index_first += diff;
      m_index_last += diff;
    }
    /// \}

    // As a Raw Image
    const std::size_t* strides() const;
    int                border() const { return m_border; }

    std::ptrdiff_t strides(int d) const { return m_strides[static_cast<size_t>(d)]; }
    const_pointer  data() const { return reinterpret_cast<const_pointer>(m_data->buffer); }
    pointer        data() { return reinterpret_cast<pointer>(m_data->buffer); }

    // Specialized algorithm
    template <typename T_, unsigned dim_, typename E_, typename Domain_>
    friend std::enable_if_t<std::is_convertible<Domain_, typename ndimage_base<T_, dim_, E_>::domain_type>::value, E_>
        make_subimage(ndimage_base<T_, dim_, E_>&, const Domain_& domain);

    // As an Indexable Image
    const std::ptrdiff_t* index_strides() const { return &m_index_strides[0]; }

    difference_type delta_offset(const point_type& p) const
    {
      difference_type idx = 0;
      for (unsigned i = 0; i < dim; ++i)
        idx += static_cast<difference_type>(p[i] * m_strides[i]);
      return idx;
    }

    extension_type extension() const;

    /// \brief Inflate (or reduce, if the value is negative) the domain size by
    /// a given amount, by reducing (inflating) the border.
    ///
    /// \note No memory reallocation is performed by the method
    ///
    /// \param[in] delta The domain inflation value. It can be negative.
    /// \precondition -amin(shape() / 2) <= delta <= border
    void inflate_domain(int delta);


    /// \brief clip an image to a domain
    E clip(const domain_type& domain) const;

  private:
    static E from_buffer_copy_(void* buffer, const domain_type& domain, const size_t* strides);

    static E from_buffer_extern_(void* buffer, const domain_type& domain, const size_t* strides);

    /// Unchecked access to a location (can lead to an out-of-range address)
    T* __at(const site_type& p) const;

  protected:
    template <typename, typename, unsigned, typename>
    friend struct indexible_ndimage_base;
    template <typename, typename>
    friend struct ndimage_value_range;
    template <typename, typename>
    friend struct ndimage_pixel_range;

    void resize_(const domain_type& domain, unsigned border = 3, T v = T());

    domain_type m_domain; ///< Domain of image
    //#ifndef MLN_NDEBUG
    domain_type vbox_;
    //#endif

    std::array<size_t, dim>                         m_strides; ///< Strides in bytes
    std::shared_ptr<internal::ndimage_data<T, dim>> m_data;
    int                                             m_border;
    char*                                           m_ptr; ///< Pointer to the first element in the domain
    char* m_last; ///< Pointer to the last element in the domain (not past-the-end)

    T*                              m_ptr_origin;    ///< Pointer to the first element (pmin)
    std::array<std::ptrdiff_t, dim> m_index_strides; ///< Strides in number of elements (including the border)
    size_type                       m_index_first;   ///< index of pmin
    size_type                       m_index_last;    ///< index of pmax-1
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

      strides[dim - 1] = sizeof(T);

      // Each row / page ... are 16 bytes aligned
      unsigned ndim = dim;

#ifdef MLN_128B_ALIGNMENT
      if (ndim >= 2)
      {
        strides[dim - 2] = ((shape[dim - 1] * sizeof(T)) & ~(size_t)15) + (size_t)16;
        for (int i = dim - 3; i >= 0; --i)
          strides[i] = shape[i + 1] * strides[i + 1];
      }
#else
      if (ndim >= 2)
      {
        strides[dim - 2] = shape[dim - 1] * sizeof(T);
        for (int i = dim - 3; i >= 0; --i)
          strides[i] = shape[i + 1] * strides[i + 1];
      }
#endif

      // Allocate data
      nbytes = shape[0] * strides[0];
      buffer = (char*)mln::aligned_malloc(nbytes, border * sizeof(T));

      // Construct
      {
        char*    ptr    = buffer;
        unsigned nlines = 1;
        for (unsigned i = 0; i < dim - 1; ++i)
          nlines *= static_cast<unsigned>(shape[i]);
        unsigned nelements = static_cast<unsigned>(shape[dim - 1]);

        if (dim > 1)
        {
          for (unsigned i = 0; i < nlines; ++i, ptr += strides[dim - 2])
          {
            T* p_ = (T*)ptr;
            for (unsigned j = 0; j < nelements; ++j, ++p_)
              new (p_) T(v);
          }
        }
        else
        {
          T* p_ = (T*)ptr;
          for (unsigned j = 0; j < nelements; ++j, ++p_)
            new (p_) T(v);
        }
      }
    }

    template <typename T, unsigned dim>
    ndimage_data<T, dim>::~ndimage_data()
    {
      char*    ptr    = buffer;
      unsigned nlines = 1;
      for (unsigned i = 0; i < dim - 1; ++i)
        nlines *= static_cast<unsigned>(shape[i]);
      unsigned nelements = static_cast<unsigned>(shape[dim - 1]);
      if (dim == 1)
      {
        for (unsigned k = 0; k < nelements; ++k)
          ((T*)ptr + k)->~T();
        //    std::destroy( (T*)ptr, (T*) ptr + nelements);
      }
      else
      {
        for (unsigned i = 0; i < nlines; ++i, ptr += strides[dim - 2])
          for (unsigned k = 0; k < nelements; ++k)
            ((T*)ptr + k)->~T();
        // std::destroy((T*)ptr, (T*)ptr + nelements);
      }
      mln::aligned_free(buffer);
    }
  } // namespace internal

  template <typename T, unsigned dim, typename E>
  ndimage_base<T, dim, E>::ndimage_base(unsigned border)
    : m_domain()
    , m_border(border)
    , m_ptr(NULL)
    , m_last(NULL)
  {
    for (unsigned i = 0; i < dim; ++i)
    {
      mln_postcondition(m_domain.pmin[i] == 0);
      mln_postcondition(m_domain.pmax[i] == 0);
    }
  }

  template <typename T, unsigned dim, typename E>
  ndimage_base<T, dim, E>::ndimage_base(const domain_type& domain, unsigned border, T v)
    : m_domain(domain)
    , m_border(border)
  {
    resize_(m_domain, border, v);
  }

  template <typename T, unsigned dim, typename E>
  template <typename U, typename E2>
  ndimage_base<T, dim, E>::ndimage_base(const ndimage_base<U, dim, E2>& g, unsigned border, T v)
    : m_domain(g.domain())
    , m_border(border)
  {
    resize_(m_domain, m_border, v);
  }


  template <typename T, unsigned dim, typename E>
  template <typename U, class E2>
  ndimage_base<T, dim, E>::ndimage_base(const ndimage_base<U, dim, E2>& other, const image_build_params& params)
    : m_domain(other.domain())
  {
    m_border = (params.border >= 0) ? params.border : other.border();

    T val{};
    if (params.init_value.has_value())
      val = std::any_cast<T>(params.init_value);

    resize_(m_domain, m_border, val);
  }


  template <typename T, unsigned dim, typename E>
  E ndimage_base<T, dim, E>::from_buffer_extern_(void* buffer, const domain_type& domain, const size_t* strides)
  {
    E image_;
    image_.m_domain = domain;
    image_.m_border = 0;

    MLN_EVAL_IF_DEBUG(image_.vbox_ = domain);
    std::copy(strides, strides + dim, image_.m_strides.begin());

    if (image_.m_strides[dim - 1] % sizeof(T) != 0)
    {
      throw std::runtime_error("The padding of the image is not compatible with the size of the element.");
    }

    point<size_t, dim> sz           = domain.shape();
    image_.m_ptr_origin             = (T*)buffer;
    image_.m_index_strides[dim - 1] = 1;
    image_.m_index_first            = 0;
    image_.m_index_last             = sz[dim - 1] - 1;
    image_.m_ptr                    = (char*)buffer;
    image_.m_last                   = (char*)buffer + (sz[dim - 1] - 1) * image_.m_strides[dim - 1];

    for (int i = dim - 2; i >= 0; --i)
    {
      if (image_.m_strides[i] % sizeof(T) != 0)
      {
        throw std::runtime_error("The padding of the image is not compatible with the size of the element.");
      }

      image_.m_index_strides[i] = image_.m_strides[i] / sizeof(T);
      image_.m_index_last += (sz[i] - 1) * image_.m_index_strides[i];
      image_.m_last += (sz[i] - 1) * image_.m_strides[i];
    }

    return image_;
  }

  template <typename T, unsigned dim, typename E>
  E ndimage_base<T, dim, E>::from_buffer_copy_(void* buffer, const domain_type& domain, const size_t* strides)
  {
    E in = from_buffer_extern_(buffer, domain, strides);

    E out(domain);

    mln_pixter(pin, pout, in, out);
    mln_forall (pin, pout)
      pout->val() = pin->val();

    return out;
  }

  template <typename T, unsigned dim, typename E>
  E ndimage_base<T, dim, E>::from_buffer(void* buffer, const domain_type& domain, const size_t* strides, bool copy)
  {
    if (copy)
      return from_buffer_copy_(buffer, domain, strides);
    else
      return from_buffer_extern_(buffer, domain, strides);
  }

  template <typename T, unsigned dim, typename E>
  E ndimage_base<T, dim, E>::from_buffer(void* buffer, const domain_type& domain, bool copy)
  {
    point<size_t, dim> sz = domain.shape();
    size_t             strides[dim];

    strides[dim - 1] = sizeof(T);
    for (int i = dim - 2; i >= 0; --i)
      strides[i] = strides[i + 1] * sz[i + 1];

    return from_buffer(buffer, domain, strides, copy);
  }

  template <typename T, unsigned dim, typename E>
  void ndimage_base<T, dim, E>::resize_(const domain_type& domain, unsigned border, T v)
  {
    site_type          shp = domain.shape();
    point<size_t, dim> sz;
    MLN_EVAL_IF_DEBUG(vbox_ = domain);
    MLN_EVAL_IF_DEBUG(vbox_.pmin -= border);
    MLN_EVAL_IF_DEBUG(vbox_.pmax += border);
    sz = shp;

    // Compute strides size (in bytes)
    // The row stride is 16 bytes aligned
    m_data.reset(new internal::ndimage_data<T, dim>(&(sz[0]), border, v));
    std::copy(m_data->strides, m_data->strides + dim, m_strides.begin());

    // Compute pointer at (0,0)
    m_ptr_origin             = (T*)m_data->buffer;
    m_index_strides[dim - 1] = 1;
    m_index_first            = m_border;
    m_index_last             = m_border + sz[dim - 1] - 1;
    m_ptr                    = m_data->buffer + border * m_strides[dim - 1];
    m_last                   = m_data->buffer + (border + sz[dim - 1] - 1) * m_strides[dim - 1];
    for (int i = dim - 2; i >= 0; --i)
    {
      m_index_strides[i] = m_strides[i] / sizeof(T);
      m_index_first += m_border * m_index_strides[i];
      m_index_last += (m_border + sz[i] - 1) * m_index_strides[i];
      m_ptr += border * m_strides[i];
      m_last += (border + sz[i] - 1) * m_strides[i];
    }
  }

  template <typename T, unsigned dim, typename E>
  void ndimage_base<T, dim, E>::resize(const domain_type& domain, unsigned border, T v)
  {
    if (not(domain == m_domain) or (int) border != m_border)
    {
      m_domain = domain;
      m_border = border;
      resize_(m_domain, border, v);
    }
    else
    {
      std::fill((T*)m_ptr, ((T*)m_last) + 1, v);
    }
  }

  template <typename T, unsigned dim, typename E>
  void ndimage_base<T, dim, E>::resize(const domain_type& domain, const image_build_params& params)
  {
    int border = (params.border >= 0) ? params.border : m_border;

    T val{};
    if (params.init_value.has_value())
      val = std::any_cast<T>(params.init_value);

    if (not(domain == m_domain) or border != m_border)
    {
      m_domain = domain;
      m_border = border;
      resize_(m_domain, border, val);
    }
    else if (params.init_value.has_value())
    {
      std::fill((T*)m_ptr, ((T*)m_last) + 1, val);
    }
  }


  template <typename T, unsigned dim, typename E>
  inline T* ndimage_base<T, dim, E>::__at(const site_type& p) const
  {
    char* ptr = m_ptr;
    for (unsigned i = 0; i < dim; ++i)
      ptr += (p[i] - m_domain.pmin[i]) * m_strides[i];
    return reinterpret_cast<T*>(ptr);
  }

  template <typename T, unsigned dim, typename E>
  inline T& ndimage_base<T, dim, E>::at(const site_type& p)
  {
    mln_precondition(vbox_.has(p));
    return *__at(p);
  }

  template <typename T, unsigned dim, typename E>
  inline const T& ndimage_base<T, dim, E>::at(const site_type& p) const
  {
    mln_precondition(vbox_.has(p));
    return *__at(p);
  }

  template <typename T, unsigned dim, typename E>
  inline T& ndimage_base<T, dim, E>::operator()(const site_type& p)
  {
    mln_precondition(m_domain.has(p));
    return at(p);
  }

  template <typename T, unsigned dim, typename E>
  inline const T& ndimage_base<T, dim, E>::operator()(const site_type& p) const
  {
    mln_precondition(m_domain.has(p));
    return at(p);
  }

  template <typename T, unsigned dim, typename E>
  inline T& ndimage_base<T, dim, E>::operator[](size_type i)
  {
    mln_precondition(vbox_.has(point_at_index(i)));
    return m_ptr_origin[i];
  }

  template <typename T, unsigned dim, typename E>
  inline const T& ndimage_base<T, dim, E>::operator[](size_type i) const
  {
    mln_precondition(vbox_.has(point_at_index(i)));
    return m_ptr_origin[i];
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::pixel_type ndimage_base<T, dim, E>::pixel_at(const point_type& p)
  {
    pixel_type pix;
    pix.m_ima   = this->exact();
    pix.m_ptr   = m_ptr_origin;
    pix.m_point = p;
    pix.m_index = this->index_of_point(p);
    return pix;
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::const_pixel_type ndimage_base<T, dim, E>::pixel_at(const point_type& p) const
  {
    const_pixel_type pix;
    pix.m_ima   = this->exact();
    pix.m_ptr   = m_ptr_origin;
    pix.m_point = p;
    pix.m_index = this->index_of_point(p);
    return pix;
  }

  template <typename T, unsigned dim, typename E>
  details::ndpixel<const T, dim> ndimage_base<T, dim, E>::new_pixel_at(const point_type& p) const
  {
    T* ptr = this->__at(mln::literal::zero);

    details::ndpixel<const T, dim> pix;
    pix.m_info.stride = m_index_strides;
    pix.m_info.from   = m_domain.pmin;
    pix.m_info.to     = m_domain.pmax;
    pix.m_info.buffer = ptr;
    pix.m_lineptr     = ptr;
    pix.m_point       = mln::literal::zero;
    pix.shift(p);
    return pix;
  }

  template <typename T, unsigned dim, typename E>
  details::ndpixel<T, dim> ndimage_base<T, dim, E>::new_pixel_at(const point_type& p)
  {
    T* ptr = this->__at(mln::literal::zero);

    details::ndpixel<T, dim> pix;
    pix.m_info.stride = m_index_strides;
    pix.m_info.from   = m_domain.pmin;
    pix.m_info.to     = m_domain.pmax;
    pix.m_info.buffer = ptr;
    pix.m_lineptr     = ptr;
    pix.m_point       = mln::literal::zero;
    pix.shift(p);
    return pix;
  }

  template <typename T, unsigned dim, typename E>
  details::ndpixel<const T, dim> ndimage_base<T, dim, E>::new_pixel(const point_type& p) const
  {
    mln_precondition(m_domain.has(p));
    return this->new_pixel_at(p);
  }

  template <typename T, unsigned dim, typename E>
  details::ndpixel<T, dim> ndimage_base<T, dim, E>::new_pixel(const point_type& p)
  {
    mln_precondition(m_domain.has(p));
    return this->new_pixel_at(p);
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::pixel_type ndimage_base<T, dim, E>::pixel_at_index(size_type i)
  {
    pixel_type pix;
    pix.m_ima   = this->exact();
    pix.m_ptr   = m_ptr_origin;
    pix.m_point = this->point_at_index(i);
    pix.m_index = i;
    return pix;
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::const_pixel_type ndimage_base<T, dim, E>::pixel_at_index(size_type i) const
  {
    const_pixel_type pix;
    pix.m_ima   = this->exact();
    pix.m_ptr   = m_ptr_origin;
    pix.m_point = this->point_at_index(i);
    pix.m_index = i;
    return pix;
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::pixel_type ndimage_base<T, dim, E>::pixel(const point_type& p)
  {
    mln_precondition(m_domain.has(p));
    return pixel_at(p);
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::const_pixel_type ndimage_base<T, dim, E>::pixel(const point_type& p) const
  {
    mln_precondition(m_domain.has(p));
    return pixel_at(p);
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::size_type ndimage_base<T, dim, E>::index_of_point(const point_type& p) const
  {
    std::size_t idx = m_index_first;
    point_type  q   = p - m_domain.pmin;
    for (unsigned i = 0; i < dim; ++i)
      idx += q[i] * m_index_strides[i];
    return static_cast<typename ndimage_base<T, dim, E>::size_type>(idx);
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::point_type ndimage_base<T, dim, E>::point_at_index(size_type idx) const
  {
    point_type p = m_domain.pmin - m_border;

    int diff = static_cast<int>(idx) - static_cast<int>(m_index_first);
    for (unsigned i = 0; i < dim; ++i)
    {
      diff += static_cast<int>(m_index_strides[i] * m_border);
    }

    for (unsigned i = 0; i < dim; ++i)
    {
      std::div_t q = std::div(diff, (int)m_index_strides[i]);
      if (q.rem < 0)
      {
        q.rem += static_cast<int>(m_index_strides[i]);
        q.quot -= 1;
      }
      p[i] += static_cast<short>(q.quot);
      diff = q.rem;
    }

    mln_postcondition(vbox_.has(p));
    return p;
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::difference_type
      ndimage_base<T, dim, E>::delta_index(const point_type& p) const
  {
    difference_type idx = 0;
    for (unsigned i = 0; i < dim; ++i)
      idx += static_cast<int>(p[i] * m_index_strides[i]);
    return idx;
  }

  template <typename T, unsigned dim, typename E>
  inline void ndimage_base<T, dim, E>::inflate_domain(int delta)
  {
    mln_precondition(delta <= m_border);
    for (unsigned k = 0; k < dim; ++k)
      mln_precondition((m_domain.pmin[k] - delta) <= (m_domain.pmax[k] + delta));

    m_domain.pmin -= delta;
    m_domain.pmax += delta;
    m_border -= delta;

    for (unsigned k = 0; k < dim; ++k)
    {
      m_ptr -= delta * m_strides[k];
      m_last += delta * m_strides[k];
      m_index_first -= delta * m_index_strides[k];
      m_index_last += delta * m_index_strides[k];
    }
  }

  // template <typename T, unsigned dim, typename E>
  // inline
  // const T&
  // ndimage_base<T,dim,E>::element (difference_type n) const
  // {
  //   return *reinterpret_cast<const T*>(m_ptr+n);
  // }

  template <typename T, unsigned dim, typename E>
  const size_t* ndimage_base<T, dim, E>::strides() const
  {
    return &m_strides[0];
  }

  // template <typename T, unsigned dim, typename E>
  // ptrdiff_t
  // ndimage_base<T,dim,E>::offset (point_type dp) const
  // {
  //   ptrdiff_t x = 0;
  //   for (unsigned i = 0; i < dim; ++i)
  //     x += p[i] * m_strides[i];
  //   return x;
  // }

  template <typename T, unsigned dim, typename E>
  inline const typename ndimage_base<T, dim, E>::domain_type& ndimage_base<T, dim, E>::domain() const
  {
    return m_domain;
  }

  /* -- Value range -- */

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::const_value_range ndimage_base<T, dim, E>::values() const
  {
    return const_value_range(*this);
  }

  template <typename T, unsigned dim, typename E>
  ranges::multi_span<T, dim> ndimage_base<T, dim, E>::new_values()
  {
    return {reinterpret_cast<T*>(m_ptr), m_domain.extents(), m_index_strides};
  }

  template <typename T, unsigned dim, typename E>
  ranges::multi_span<const T, dim> ndimage_base<T, dim, E>::new_values() const
  {
    return {reinterpret_cast<const T*>(m_ptr), m_domain.extents(), m_index_strides};
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::value_range ndimage_base<T, dim, E>::values()
  {
    return value_range(*this);
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::const_pixel_range ndimage_base<T, dim, E>::pixels() const
  {
    return const_pixel_range(*this);
  }

  template <typename T, unsigned dim, typename E>
  details::ndpix_range<T, dim> ndimage_base<T, dim, E>::new_pixels()
  {
    T* ptr = this->__at(mln::literal::zero);
    return details::ndpix_range<T, dim>(m_index_strides, m_domain.pmin, m_domain.pmax, ptr);
  }

  template <typename T, unsigned dim, typename E>
  details::ndpix_range<const T, dim> ndimage_base<T, dim, E>::new_pixels() const
  {
    const T* ptr = this->__at(mln::literal::zero);
    return details::ndpix_range<const T, dim>(m_index_strides, m_domain.pmin, m_domain.pmax, ptr);
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::pixel_range ndimage_base<T, dim, E>::pixels()
  {
    return pixel_range(*this);
  }

  template <typename T, unsigned dim, typename E>
  inline typename ndimage_base<T, dim, E>::extension_type ndimage_base<T, dim, E>::extension() const
  {
    return extension_type(m_ptr, &m_strides[0], m_domain.shape(), &m_domain, m_border);
  }


  template <typename T, unsigned dim, typename E>
  E ndimage_base<T, dim, E>::clip(const domain_type& domain) const
  {
    E other(exact(*this));
    other.m_domain      = domain;
    other.m_border      = this->m_border; // FIXME
    other.m_ptr         = (char*)&(this->at(domain.pmin));
    other.m_last        = (char*)&(this->at(domain.pmax - 1));
    other.m_ptr_origin  = this->m_ptr_origin;
    other.m_index_first = this->index_of_point(domain.pmin);
    other.m_index_last  = this->index_of_point(domain.pmax - 1);
    return other;
  }

  template <unsigned d, typename T1, typename E1, typename T2, typename E2>
  inline bool are_indexes_compatible(const ndimage_base<T1, d, E1>& self, const ndimage_base<T2, d, E2>& other)
  {
    return (self.index_of_point(self.domain().pmin) == other.index_of_point(other.domain().pmin)) and
           (self.m_index_strides == other.m_index_strides);
  }


  /******************************************/
  /****     Extension implementation     ****/
  /******************************************/

  namespace internal
  {

    template <typename T, unsigned dim>
    ndimage_extension<T, dim>::ndimage_extension(char* ptr, const std::size_t* strides, const point_type& shp,
                                                 const box<short, dim>* dom, int border)
      : m_shp(shp)
      , m_dom(dom)
      , m_ptr(ptr)
      , m_border(border)
    {
      for (unsigned i = 0; i < dim; ++i)
      {
        m_strides[i] = strides[i];
        m_ptr -= m_strides[i] * border;
      }
    }

    template <typename T, unsigned dim>
    template <typename SE>
    bool ndimage_extension<T, dim>::fit(const SE& se) const
    {
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>() || mln::is_a<SE, mln::Neighborhood>());

      if constexpr (std::is_convertible_v<typename SE::category, dynamic_neighborhood_tag>)
      {
        return se.radial_extent() <= m_border;
      }
      else
      {
        return false; // TODO check wether default value should be true false or error
      }
    }

    template <typename T, unsigned dim>
    constexpr bool ndimage_extension<T, dim>::is_finite() const
    {
      return true;
    }

    template <typename T, unsigned dim>

    std::optional<std::size_t> ndimage_extension<T, dim>::size() const
    {
      return m_border;
    }

    template <typename T, unsigned dim>

    auto ndimage_extension<T, dim>::value(const point_type& pnt) const -> const value_type&
    {
      char* ptr = m_ptr;
      for (unsigned i = 0; i < dim; ++i)
        ptr += (pnt[i] - m_dom->pmin[i]) * m_strides[i];
      return *reinterpret_cast<T*>(ptr);
    }

    template <typename T, unsigned dim>
    void ndimage_extension<T, dim>::fill(const T& v)
    {
      _fill<0>(m_ptr, v);
    }

    template <typename T, unsigned dim>
    void ndimage_extension<T, dim>::mirror(std::size_t padding)
    {
      _mirror<0>(m_ptr, padding);
    }

    template <typename T, unsigned dim>
    void ndimage_extension<T, dim>::periodize()
    {
      // TODO
    }

    template <typename T, unsigned dim>
    void ndimage_extension<T, dim>::clamp()
    {
      // TODO
    }

    template <typename T, unsigned dim>
    template <unsigned d>
    typename std::enable_if<(d < dim)>::type ndimage_extension<T, dim>::_mirror(char* ptr, std::size_t padding)
    {
      // FIXME: padding param is ignored for now
      char* pori = ptr + m_border * m_strides[d];
      char* p    = pori;
      for (int i = 0; i < m_shp[d]; ++i, p += m_strides[d])
        _mirror<d + 1>(p, padding);

      char* src1  = pori;
      char* dest1 = pori - m_strides[d];
      char* dest2 = pori + m_shp[d] * m_strides[d];
      char* src2  = dest2 - m_strides[d];

      for (int i = 0; i < m_border; ++i)
      {
        _copy_line<d + 1>(src1, dest1);
        _copy_line<d + 1>(src2, dest2);
        src1 += m_strides[d];
        dest1 -= m_strides[d];
        src2 -= m_strides[d];
        dest2 += m_strides[d];
      }
    }

    template <typename T, unsigned dim>
    template <unsigned d>
    typename std::enable_if<(d == dim)>::type ndimage_extension<T, dim>::_mirror(char* ptr, std::size_t /*padding*/)
    {
      (void)ptr;
    }

    template <typename T, unsigned dim>
    template <unsigned d>
    typename std::enable_if<(d < dim)>::type ndimage_extension<T, dim>::_copy_line(char* src, char* dest)
    {
      for (int k = 0; k < m_shp[d] + 2 * m_border; ++k)
      {
        _copy_line<d + 1>(src, dest);
        src += m_strides[d];
        dest += m_strides[d];
      }
    }

    template <typename T, unsigned dim>
    template <unsigned d>
    typename std::enable_if<(d == dim)>::type ndimage_extension<T, dim>::_copy_line(char* src, char* dest)
    {
      *(reinterpret_cast<T*>(dest)) = *(reinterpret_cast<T*>(src));
    }

    template <typename T, unsigned dim>
    template <unsigned d>
    typename std::enable_if<(d < dim)>::type ndimage_extension<T, dim>::_fill(char* ptr, const T& v)
    {
      for (int i = 0; i < m_border; ++i, ptr += m_strides[d])
        _fillall<d + 1>(ptr, v);

      for (int i = 0; i < m_shp[d]; ++i, ptr += m_strides[d])
        _fill<d + 1>(ptr, v);

      for (int i = 0; i < m_border; ++i, ptr += m_strides[d])
        _fillall<d + 1>(ptr, v);
    }

    template <typename T, unsigned dim>
    template <unsigned d>
    typename std::enable_if<(d == dim)>::type ndimage_extension<T, dim>::_fill(char* ptr, const T& v)
    {
      (void)ptr;
      (void)v;
    }

    template <typename T, unsigned dim>
    template <unsigned d>
    typename std::enable_if<(d < dim)>::type ndimage_extension<T, dim>::_fillall(char* ptr, const T& v)
    {
      for (int i = 0; i < m_shp[d] + (2 * m_border); ++i, ptr += m_strides[d])
        _fillall<d + 1>(ptr, v);
    }

    template <typename T, unsigned dim>
    template <unsigned d>
    typename std::enable_if<(d == dim)>::type ndimage_extension<T, dim>::_fillall(char* ptr, const T& v)
    {
      *(reinterpret_cast<T*>(ptr)) = v;
    }
  } // namespace internal

} // namespace mln
