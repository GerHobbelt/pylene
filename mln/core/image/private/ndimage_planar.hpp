#pragma once

#include "ndimage_interface.hpp"
#include "ndimage.hpp"
#include "ndimage_interleaved.hpp"

namespace mln
{

  template <int N, class V>
  using ndimage_planar = ndimage<N, V, layout_t::PLANAR>;

  template <int N, class T, int vdim>
  using __ndimage_planar = __ndimage<N, T, layout_t::PLANAR, vdim>;


  /******************************************/
  /****              Traits              ****/
  /******************************************/

  template <int N, class V>
  struct image_traits<ndimage_planar<N, V>>
  {
    typedef raw_image_tag category;
    typedef std::true_type accessible;
    typedef std::true_type indexable;
    typedef std::true_type concrete;
    typedef std::true_type shallow_copy;
    typedef std::true_type has_border;
    typedef mln::extension::border_extension_tag extension;
  };

  template <int N, class V>
  struct image_concrete<ndimage_planar<N, V>>
  {
    using type = ndimage_planar<N, V>;
  };

  template <int N, class V, class U>
  struct image_ch_value<ndimage_planar<N, V>, U>
  {
    using type = ndimage_planar<N, U>;
  };

  /******************************************/
  /****    Specialized free functions    ****/
  /******************************************/

  template <int N, class V, class T>
  ndimage_planar<N, V> make_subimage(const ndimage_planar<N, V>& f, const mln::box<T, unsigned(N)>& domain);  // Force N to be non-deduced from domain


  /*****************************************************************************************/
  /****              Defintition  of  __ndimage<N, T, layout_t::PLANAR, 0>              ****/
  /*****************************************************************************************/

  template <int N, class T>
  class __ndimage<N, T, layout_t::PLANAR, 0> : public __ndimage<N, T, layout_t::UNKNOWN, 0>
  {
  private:
    using base = __ndimage<N, T, layout_t::UNKNOWN, 0>;
  public:
    using domain_type = typename base::domain_type;

    // As an Image
    // Constructors
    // \{
    explicit __ndimage(int border);
    __ndimage(const domain_type& domain, int border, int nchannels);
    __ndimage(const domain_type& domain, int border, int nchannels, T values[]);
    // \}

    /// \brief Resize the image to fit \p domain.
    ///
    /// Resize the image w.r.t to \p domain and \p border. The image is
    /// initialized with value  \p v.
    /// \warning The old values are not kept.
    /// \param domain The new domain of the image.
    /// \param border The new border of the image.
    /// \param values The initialization value of the image. If \p is given, values
    /// are copy constructed from \p v, otherwise, they are default-initialized.
    void resize(const domain_type& domain);
    void resize(const domain_type& domain, int border);
    void resize(const domain_type& domain, int border, T values[]);

    layout_t layout() const final;
    std::ptrdiff_t byte_stride(int k = 0) const final;

    ndimage<N, T, layout_t::INTERLEAVED> channel(int k = 0) const;

  private:
    virtual T* __ptr(int k) const = 0;
  };


  /******************************************************************************************/
  /****              Definition  of  __ndimage<N, T, layout_t::PLANAR, -1>              *****/
  /****                  (Planar image with dynamic-known bound                         *****/
  /******************************************************************************************/

  template <int N, class T>
  class __ndimage<N, T, layout_t::PLANAR, -1> : public __ndimage<N, T, layout_t::PLANAR, 0>
  {
  private:
    using base = __ndimage<N, T, layout_t::PLANAR, 0>;
  public:
    using domain_type = typename base::domain_type;

    // As an Image
    // Constructors
    // \{
    explicit __ndimage(int border);
    __ndimage(const domain_type& domain, int border, int nchannels);
    __ndimage(const domain_type& domain, int border, int nchannels, T values[]);
    // \}

    /// \brief Resize the image to fit \p domain.
    ///
    /// Resize the image w.r.t to \p domain and \p border. The image is
    /// initialized with value  \p v.
    /// \warning The old values are not kept.
    /// \param domain The new domain of the image.
    /// \param border The new border of the image.
    /// \param values The initialization value of the image. If \p is given, values
    /// are copy constructed from \p v, otherwise, they are default-initialized.
    void resize(const domain_type& domain);
    void resize(const domain_type& domain, int border);
    void resize(const domain_type& domain, int border, T values[]);



    T* buffer(int k = 0) final;
    const T* buffer(int k = 0) const final;
    int nbuffers() const final;
    int nchannels() const final;

  protected:
    void __from_buffer(void* buffers[], const domain_type& domain, int nchannels);
    void __from_buffer(void* buffers[], const domain_type& domain, int nchannels, const std::size_t strides[]);

  private:
    void __init(int nchannels);
    void* __buffer(int k = 0) const final;

  private:
    T* __ptr(int k) const final { return m_ptr[k]; }

  protected:
    std::vector<T*> m_ptr;
  };


  /******************************************************************************************/
  /****              Defintition  of  __ndimage<N, T, layout_t::PLANAR, vdim>            ****/
  /****                  (Planar image with static-known bound                          *****/
  /******************************************************************************************/

  template <int N, class T, int vdim>
  class __ndimage<N, T, layout_t::PLANAR, vdim> : public __ndimage<N, T, layout_t::PLANAR, 0>
  {
  private:
    using base = __ndimage<N, T, layout_t::PLANAR, 0>;
  public:
    using domain_type = typename base::domain_type;

    // As an Image
    // Constructors
    // \{
    explicit __ndimage(int border);
    __ndimage(const domain_type& domain, int border);
    __ndimage(const domain_type& domain, int border, T values[]);
    // \}

    /// Initialization constructors
    /// \{
    __ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, mln::init);
    __ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border);
    __ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border, T values[]);
    /// \}


    /// \brief Resize the image to fit \p domain.
    ///
    /// Resize the image w.r.t to \p domain and \p border. The image is
    /// initialized with value  \p v.
    /// \warning The old values are not kept.
    /// \param domain The new domain of the image.
    /// \param border The new border of the image.
    /// \param values The initialization value of the image. If \p is given, values
    /// are copy constructed from \p v, otherwise, they are default-initialized.
    void resize(const domain_type& domain);
    void resize(const domain_type& domain, int border);
    void resize(const domain_type& domain, int border, T values[]);


    int nbuffers() const final;
    int nchannels() const final;

  protected:
    void __from_buffer(void* buffers[], const domain_type& domain);
    void __from_buffer(void* buffers[], const domain_type& domain, const std::size_t strides[]);


  private:
    void __init();
    void* __buffer(int k = 0) const final;
    T* __ptr(int k) const final { return m_ptr[k]; }

  protected:
    // Pointer adress to origin (0,0). The pointer adress may not be legal if (0,0) does not belong to the virtual domain
    // Passing through uintptr_t for pointer arithmetic prevents clang to optimize
    T* m_ptr[vdim];
  };



  /*****************************************************************************************/
  /****              Defintition  of  ndimage<N, V, layout_t::PLANAR, E>           ****/
  /*****************************************************************************************/


  template <int N, class V>
  class ndimage<N, V, layout_t::PLANAR> :
    public ndimage_base<N, V, layout_t::PLANAR>,
    public Image<ndimage<N, V, layout_t::PLANAR>>
  {
  private:
    using __point_type = mln::point<std::ptrdiff_t, N>;
    using __index_type = std::ptrdiff_t;
    using base = ndimage_base<N, V, layout_t::PLANAR>;
    using this_type = ndimage<N, V, layout_t::PLANAR>;


  public:
    using sample_type = typename base::sample_type;

    ///
    using domain_type = typename base::domain_type;

    /// \copydoc image::pixel_type
    using pixel_type = void;

    /// \copydoc image::const_pixel_type
    using const_pixel_type = void;

    /// \copydoc image::value_type
    using value_type = V;

    /// \copydoc image::reference
    using reference = void;

    /// \copydoc image::const_reference
    using const_reference = V;

    // Extension
    using extension_type = void;
    /// \}


    /// \name Image Ranges Types
    /// \{

    /// \copydoc image::value_range
    using value_range = void;

    /// \copydoc image::const_value_range
    using const_value_range = void;

    /// \copydoc image::pixel_range
    using pixel_range = void;

    /// \copydoc image::const_pixel_range
    using const_pixel_range = void;
    /// \}

    using base::vdim;

    enum
    {
      ndim = N // Deprecated
    };

    /// As an Image
    /// Constructors
    /// \{
    explicit ndimage(int border = 3);
    explicit ndimage(const domain_type& domain, int border = 3);
    ndimage(const domain_type& domain, int border, V init);
    /// \}

    /// Initialization constructors
    /// \{
    ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, mln::init);
    ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border);
    ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border, V init);
    // \}

    /// Image resizeing
    /// \{
    void resize(const domain_type& domain);
    void resize(const domain_type& domain, int border);
    void resize(const domain_type& domain, int border, V init);
    /// \}

    /// \brief Constructors from external sources
    /// \{
    static ndimage from_buffer(void* buffers[], const domain_type& domain);

    static ndimage from_buffer(void* buffers[], const domain_type& domain, const std::size_t byte_strides[]);
    /// \}

    V* buffer(int k = 0);
    const V* buffer(int k = 0) const;

    /// \name Accession Operators
    /// \{

    /// \copydoc image::operator()(const site_type& p) const
    reference operator()(const __point_type& p);

    /// \copydoc image::operator()(const site_type& p) const
    const_reference operator()(const __point_type& p) const;

    /// \copydoc image::operator[](size_type i) const
    reference operator[](__index_type i);

    /// \copydoc image::operator[](size_type i) const
    const_reference operator[](__index_type i) const;

    /// \copydoc image::at(const site_type& p) const
    reference at(const __point_type& p);

    /// \copydoc image::at(const site_type& p) const
    const_reference at(const __point_type& p) const;
    /// \}

    /// \name Pixel utilities
    /// \{

    /// \copydoc image::pixel_at(const point_type&) const
    pixel_type pixel_at(const __point_type& p);

    /// \copydoc image::pixel_at(const point_type&) const
    const_pixel_type pixel_at(const __point_type& p) const;

    /// \copydoc image::pixel_at_index(size_type) const
    pixel_type pixel_at_index(__index_type i);

    /// \copydoc image::pixel_at_index(size_type) const
    const_pixel_type pixel_at_index(__index_type i) const;

    /// \copydoc image::pixel(const point_type&) const
    pixel_type pixel(const __point_type& p);

    /// \copydoc image::pixel(const point_type&) const
    const_pixel_type pixel(const __point_type& p) const;
    /// \}


    /// \name Image Ranges
    /// \{
    /// \copydoc image::values()
    value_range values();

    /// \copydoc image::values() const
    const_value_range values() const;

    /// \copydoc image::pixels()
    pixel_range pixels();

    /// \copydoc image::pixels() const
    const_pixel_range pixels() const;
    /// \}

    /// Return the extension of the image
    extension_type extension() const;


    /// Return a view over a subdomain
    ndimage subimage(const domain_type& domain, bool extended_by_image) const;
  };

    /*****************************************************************************************/
  /****          Implementation  of   __ndimage<N, T, layout_t::PLANAR, 0>               *****/
  /*******************************************************************************************/

    template <int N, class T>
  __ndimage<N, T, layout_t::PLANAR, 0>::__ndimage(int border)
    : base(border)
  {
    // Do not initialize (just m_border is set)
  }

  template <int N, class T>
  __ndimage<N, T, layout_t::PLANAR, 0>::__ndimage(const domain_type& domain, int border, int nchannels)
    : base(domain, border, nchannels, layout_t::PLANAR)
  {
  }

  template <int N, class T>
  __ndimage<N, T, layout_t::PLANAR, 0>::__ndimage(const domain_type& domain, int border, int nchannels, T values[])
    : base(domain, border, nchannels, layout_t::PLANAR, values)
  {
  }

  template <int N, class T>
  std::ptrdiff_t __ndimage<N, T, layout_t::PLANAR, 0>::byte_stride(int k) const
  {
    mln_precondition(k < this->nbuffers());
    return this->m_strides[k] * sizeof(T);
  }

  template <int N, class T>
  layout_t __ndimage<N, T, layout_t::PLANAR, 0>::layout() const
  {
    return layout_t::PLANAR;
  }

  template <int N, class T>
  ndimage<N, T, layout_t::INTERLEAVED>
  __ndimage<N, T, layout_t::PLANAR, 0>::channel(int k) const
  {
    ndimage<N, T, layout_t::INTERLEAVED> output;
    static_cast<base&>(output) = *this;
    output.m_ptr = this->__ptr(k);
    return output;
  }


  /*****************************************************************************************/
  /****          Implementation  of   __ndimage<N, T, layout_t::PLANAR, -1>             ****/
  /*****************************************************************************************/

    template <int N, class T>
  __ndimage<N, T, layout_t::PLANAR, -1>::__ndimage(int border)
    : base(border)
  {
  }

  template <int N, class T>
  __ndimage<N, T, layout_t::PLANAR, -1>::__ndimage(const domain_type& domain, int border, int nchannels)
    : base(nchannels, domain, border)
  {
    this->__init(nchannels);
  }

  template <int N, class T>
  __ndimage<N, T, layout_t::PLANAR, -1>::__ndimage(const domain_type& domain, int border, int nchannels, T values[])
    : base(nchannels, domain, border, values)
  {
    this->__init(nchannels);
  }

  template <int N, class T>
  void __ndimage<N, T, layout_t::PLANAR, -1>::__init(int nchannels)
  {
    m_ptr.resize(nchannels);

    T* buffer = this->m_data->buffer;
    auto offset = -this->m_first_index;
    auto plane_size = this->m_virtual_domain.size();

    for (int i = 0; i < nchannels; ++i)
    {
      m_ptr[i] = buffer + offset;
      buffer += plane_size;
    }
  }

  template <int N, class T>
  void __ndimage<N, T, layout_t::PLANAR, -1>::__from_buffer(void* buffers[], const domain_type& domain, int nchannels)
  {
    this->base::__from_buffer(domain);

    auto offset = -this->m_index_first;
    m_ptr.resize(nchannels);
    for (int i = 0; i < nchannels; ++i)
      m_ptr[i] = static_cast<T*>(buffers[i]) + offset;
  }

  template <int N, class T>
  void __ndimage<N, T, layout_t::PLANAR, -1>::__from_buffer(void* buffers[], const domain_type& domain, int nchannels, const std::size_t* byte_strides)
  {
    std::ptrdiff_t strides[N];
    for (int k = 0; k < N; ++k)
    {
      mln_assertion((byte_strides[k] % sizeof(T) == 0) && "Illegal strides. They must be multiple of the sizeof(T).");
      strides[k] = static_cast<std::ptrdiff_t>(byte_strides[k] / sizeof(T));
    }

    this->base::__from_buffer(domain, strides);

    auto offset = -this->m_index_first;
    m_ptr.resize(nchannels);
    for (int i = 0; i < nchannels; ++i)
      m_ptr[i] = static_cast<T*>(buffers[i]) + offset;
  }

  template <int N, class T>
  void __ndimage<N, T, layout_t::PLANAR, -1>::resize(const domain_type& domain)
  {
    this->resize(domain, this->border());
  }

  template <int N, class T>
  void __ndimage<N, T, layout_t::PLANAR, -1>::resize(const domain_type& domain, int border)
  {
    // Note: optimize this as a no-op if the buffer is already allocated and large enough
    __ndimage f(this->nchannels(), domain, border);
    *this = std::move(f);
  }

  template <int N, class T>
  void __ndimage<N, T, layout_t::PLANAR, -1>::resize(const domain_type& domain, int border, T values[])
  {
    // Note: optimize this as a no-op if the buffer is already allocated and large enough
    __ndimage f(this->nchannels(), domain, border, values);
    *this = std::move(f);
  }

  template <int N, class T>
  void* __ndimage<N, T, layout_t::PLANAR, -1>::__buffer(int k) const
  {
    (void)k;
    mln_precondition(k < this->nchannels());
    auto idx = this->index_of_point(this->m_domain.pmin);
    return m_ptr[k] + idx;
  }

  template <int N, class T>
  int __ndimage<N, T, layout_t::PLANAR, -1>::nchannels() const
  {
    return static_cast<int>(m_ptr.size());
  }

  template <int N, class T>
  int __ndimage<N, T, layout_t::PLANAR, -1>::nbuffers() const
  {
    return static_cast<int>(m_ptr.size());
  }


  /*****************************************************************************************/
  /****          Implementation  of   __ndimage<N, T, layout_t::PLANAR, vdim>      ****/
  /*****************************************************************************************/

    template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::PLANAR, vdim>::__ndimage(int border)
    : base(border)
  {
    // Do not initialize (just m_border is set)
  }

    template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::PLANAR, vdim>::__ndimage(const domain_type& domain, int border)
    : base(domain, border, vdim, layout_t::PLANAR)
  {
    __init();
  }

  template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::PLANAR, vdim>::__ndimage(const domain_type& domain, int border, T values[])
    : base(domain, border, vdim, layout_t::PLANAR, values)
  {
    __init();
  }

  template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::PLANAR, vdim>::__ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, mln::init)
    : base(other.domain(), other.border(), vdim, layout_t::PLANAR)
  {
    __init();
  }

  template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::PLANAR, vdim>::__ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border)
    : base(other.domain(), border, vdim, layout_t::PLANAR)
  {
    __init();
  }

  template <int N, class T, int vdim>
  __ndimage<N, T, layout_t::PLANAR, vdim>::__ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border, T values[])
    : base(other.domain(), border, vdim, layout_t::PLANAR, values)
  {
    __init();
  }


  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::PLANAR, vdim>::__init()
  {
    T* buffer = this->m_data->buffer;
    auto offset = -this->m_first_index;
    auto plane_size = this->m_virtual_domain.size();

    for (int i = 0; i < vdim; ++i)
    {
      m_ptr[i] = buffer + offset;
      buffer += plane_size;
    }
  }

  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::PLANAR, vdim>::__from_buffer(void* buffers[], const domain_type& domain)
  {
    this->base::__from_buffer(domain);

    auto offset = -this->m_first_index;
    for (int i = 0; i < vdim; ++i)
      m_ptr[i] = static_cast<T*>(buffers[i]) + offset;
  }

  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::PLANAR, vdim>::__from_buffer(void* buffer[], const domain_type& domain, const std::size_t* byte_strides)
  {
    std::ptrdiff_t strides[N];
    for (int k = 0; k < N; ++k)
    {
      mln_assertion((byte_strides[k] % sizeof(T) == 0) && "Illegal strides. They must be multiple of the sizeof(T[vdim]).");
      strides[k] = static_cast<std::ptrdiff_t>(byte_strides[k] / sizeof(T));
    }

    this->base::__from_buffer(domain, strides);

    auto offset = -this->m_first_index;
    for (int i = 0; i < vdim; ++i)
      m_ptr[i] = static_cast<T*>(buffer[i]) + offset;
  }

  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::PLANAR, vdim>::resize(const domain_type& domain)
  {
    this->resize(domain, this->border());
  }


  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::PLANAR, vdim>::resize(const domain_type& domain, int border)
  {
    // Note: optimize this as a no-op if the buffer is already allocated and large enough
    __ndimage f(domain, border);
    *this = std::move(f);
  }

  template <int N, class T, int vdim>
  void __ndimage<N, T, layout_t::PLANAR, vdim>::resize(const domain_type& domain, int border, T values[])
  {
    // Note: optimize this as a no-op if the buffer is already allocated and large enough
    __ndimage f(domain, border, values);
    *this = std::move(f);
  }

  template <int N, class T, int vdim>
  void* __ndimage<N, T, layout_t::PLANAR, vdim>::__buffer(int k) const
  {
    mln_precondition(k < vdim);
    auto idx = this->index_of_point(this->m_domain.pmin);
    return m_ptr[k] + idx;
  }

  template <int N, class T, int vdim>
  int __ndimage<N, T, layout_t::PLANAR, vdim>::nchannels() const
  {
    return vdim;
  }

  template <int N, class T, int vdim>
  int __ndimage<N, T, layout_t::PLANAR, vdim>::nbuffers() const
  {
    return vdim;
  }


  /*****************************************************************************************/
  /****          Implementation  of  ndimage<N, V, layout_t::PLANAR>               ****/
  /*****************************************************************************************/


  template <int N, class V>
  ndimage<N, V, layout_t::PLANAR>::ndimage(int border)
    : base(border)
  {
  }

  template <int N, class V>
  ndimage<N, V, layout_t::PLANAR>::ndimage(const domain_type& domain, int border)
  : base(domain, border)
  {
  }

  template <int N, class V>
  ndimage<N, V, layout_t::PLANAR>::ndimage(const domain_type& domain, int border, V init)
    : base(domain, border, reinterpret_cast<sample_type*>(&init))
  {
  }
  template <int N, class V>
  ndimage<N, V, layout_t::PLANAR>::ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, mln::init)
    : base(other, mln::init())
  {
  }

  template <int N, class V>
  ndimage<N, V, layout_t::PLANAR>::ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border)
    : base(other, border)
  {
  }

  template <int N, class V>
  ndimage<N, V, layout_t::PLANAR>::ndimage(const __ndimage<N, void, layout_t::UNKNOWN, 0>& other, int border, V init)
    : base(other, border, reinterpret_cast<sample_type*>(&init))
  {
  }

  template <int N, class V>
  ndimage<N, V, layout_t::PLANAR>
  ndimage<N, V, layout_t::PLANAR>::from_buffer(void* buffers[], const domain_type& domain)
  {
    ndimage output;
    output.__from_buffer(buffers, domain);
    return output;
  }

  template <int N, class V>
  ndimage<N, V, layout_t::PLANAR>
  ndimage<N, V, layout_t::PLANAR>::from_buffer(void* buffers[], const domain_type& domain, const std::size_t byte_strides[])
  {
    ndimage output;
    output.__from_buffer(buffers, domain, byte_strides);
    return output;
  }

  template <int N, class V>
  const V* ndimage<N, V, layout_t::PLANAR>::buffer(int k) const
  {
    return reinterpret_cast<const V*>(this->__buffer(k));
  }

  template <int N, class V>
  V* ndimage<N, V, layout_t::PLANAR>::buffer(int k)
  {
    return reinterpret_cast<V*>(this->__buffer(k));
  }

  template <int N, class V>
  ndimage<N, V, layout_t::PLANAR>
  ndimage<N, V, layout_t::PLANAR>::subimage(const domain_type& domain, bool extended_by_image) const
  {
    ndimage output(*this);
    output.clip(domain, extended_by_image);
    return output;
  }

  template <int N, class V>
  void ndimage<N, V, layout_t::PLANAR>::resize(const domain_type& domain,  int border, V init)
  {
    base::resize(domain, border, reinterpret_cast<sample_type*>(&init));
  }

  template <int N, class V>
  void ndimage<N, V, layout_t::PLANAR>::resize(const domain_type& domain, int border)
  {
    base::resize(domain, border);
  }

  template <int N, class V>
  void ndimage<N, V, layout_t::PLANAR>::resize(const domain_type& domain)
  {
    base::resize(domain);
  }

  /*
  template <int N, class V>
  V& ndimage<N, V, layout_t::PLANAR>::operator[] (__index_type i)
  {
    return reinterpret_cast<V*>(this->m_ptr)[i];
  }
  */

  template <int N, class V>
  V ndimage<N, V, layout_t::PLANAR>::operator[] (__index_type i) const
  {
    V val;

    if constexpr (vdim == 1)
    {
      val = this->m_ptr[0][i];
    }
    else
    {
      for (int k = 0; k < vdim; ++k)
        val[k] = this->m_ptr[k][i];
    }
    return val;
  }

  /*
  template <int N, class V>
  V& ndimage<N, V, layout_t::PLANAR>::operator() (const __point_type& p)
  {
    mln_precondition(this->m_domain.has(p));
    return this->at(p);
  }
  */

  template <int N, class V>
  V ndimage<N, V, layout_t::PLANAR>::operator() (const __point_type& p) const
  {
    mln_precondition(this->m_domain.has(p));
    return this->at(p);
  }

  /*
  template <int N, class V>
  V& ndimage<N, V, layout_t::PLANAR>::at(const __point_type& p)
  {
    mln_precondition(this->m_virtual_domain.has(p));
    auto i =  this->index_of_point(p);
    return (*this)[i];
  }
  */

  template <int N, class V>
  V ndimage<N, V, layout_t::PLANAR>::at(const __point_type& p) const
  {
    mln_precondition(this->m_virtual_domain.has(p));
    auto i = this->index_of_point(p);
    return (*this)[i];
  }

  /*

  template <int N, class V>
  details::ndimage_pixel<N, V, ndimage<N, V, layout_t::PLANAR>>
  ndimage<N, V, layout_t::PLANAR>::pixel_at(const __point_type& p)
  {
    mln_precondition(this->m_virtual_domain.has(p));
    pixel_type pix(this->exact());
    pix.m_ptr = reinterpret_cast<V*>(this->m_ptr);
    pix.m_point = p;
    pix.m_index = this->index_of_point(p);
    return pix;
  }

  template <int N, class V>
  details::ndimage_pixel<N, const V, const ndimage<N, V, layout_t::PLANAR>>
  ndimage<N, V, layout_t::PLANAR>::pixel_at(const __point_type& p) const
  {
    mln_precondition(this->m_virtual_domain.has(p));
    const_pixel_type pix(this->exact());
    pix.m_ptr =  reinterpret_cast<const V*>(this->m_ptr);
    pix.m_point = p;
    pix.m_index = this->index_of_point(p);
    return pix;
  }

  template <int N, class V>
  details::ndimage_pixel<N, V, ndimage<N, V, layout_t::PLANAR>>
  ndimage<N, V, layout_t::PLANAR>::pixel_at_index(__index_type i)
  {
    pixel_type pix(this->exact());
    pix.m_ptr = reinterpret_cast<V*>(this->m_ptr);
    pix.m_point = this->point_at_index(i);
    pix.m_index = i;
    return pix;
  }

  template <int N, class V>
  details::ndimage_pixel<N, const V, const ndimage<N, V, layout_t::PLANAR>>
  ndimage<N, V, layout_t::PLANAR>::pixel_at_index(__index_type i) const
  {
    const_pixel_type pix(this->exact());
    pix.m_ptr = reinterpret_cast<const V*>(this->m_ptr);
    pix.m_point = this->point_at_index(i);
    pix.m_index = i;
    return pix;
  }


  template <int N, class V>
  details::ndimage_pixel<N, V, ndimage<N, V, layout_t::PLANAR>>
  ndimage<N, V, layout_t::PLANAR>::pixel(const __point_type& p)
  {
    mln_precondition(this->m_domain.has(p));
    return pixel_at(p);
  }

  template <int N, class V>
  details::ndimage_pixel<N, const V, const ndimage<N, V, layout_t::PLANAR>>
  ndimage<N, V, layout_t::PLANAR>::pixel(const __point_type& p) const
  {
    mln_precondition(this->m_domain.has(p));
    return pixel_at(p);
  }

  template <int N, class V>
  details::ndimage_extension<N, V>
  ndimage<N, V, layout_t::PLANAR>::extension() const
  {
    details::ndimage_extension<N, V> e(this->m_domain, this->m_virtual_domain, this->m_strides, reinterpret_cast<V*>(this->m_ptr) + this->m_first_index);
    return e;
  }


  template <int N, class V>
  details::ndimage_value_range<N, V, ndimage<N, V, layout_t::PLANAR>>
  ndimage<N, V, layout_t::PLANAR>::values()
  {
    return value_range(this);
  }

  template <int N, class V>
  details::ndimage_value_range<N, const V, const ndimage<N, V, layout_t::PLANAR>>
  ndimage<N, V, layout_t::PLANAR>::values() const
  {
    return const_value_range(this);
  }

  template <int N, class V>
  details::ndimage_pixel_range<N, V, ndimage<N, V, layout_t::PLANAR>>
  ndimage<N, V, layout_t::PLANAR>::pixels()
  {
    return pixel_range(this);
  }

  template <int N, class V>
  details::ndimage_pixel_range<N, const V, const ndimage<N, V, layout_t::PLANAR>>
  ndimage<N, V, layout_t::PLANAR>::pixels() const
  {
    return const_pixel_range(this);
  }

  */

  template <int N, class V, class T>
  ndimage_planar<N, V> make_subimage(const ndimage_planar<N, V>& f, const mln::box<T, unsigned(N)>& domain)
  {
    const bool extended_by_image = false;
    return f.subimage(domain, extended_by_image);
  }

}
