#pragma once

#include <ano/core/concepts/image.hpp>

#include <ano/core/box.hpp>
#include <ano/core/image/private/image_builder.hpp>
#include <ano/core/image_format.hpp>
#include <ano/core/private/init_list.hpp>
#include <ano/core/value/value_set.hpp>

#include <functional>
#include <memory>


namespace ano
{
  // Forward declaration of the legacy image type
  template <typename T, unsigned dim, typename E>
  struct ndimage_base;

  constexpr int ANONYMOUS_NDBUFFER_DEFAULT_DIM = 4;

  ///
  template <class T, int N>
  class __ndbuffer_image;

  using ndbuffer_image = __ndbuffer_image<void, -1>;

  namespace internal
  {
    struct ndbuffer_image_data;
  }

  namespace details
  {
    struct ndbuffer_image_info_t
    {
      struct axis_info_t
      {
        std::ptrdiff_t byte_stride;
        int            domain_begin;
        int            domain_end;
        int            vbox_begin;
        int            vbox_end;
      };

      sample_type_id m_sample_type;                          // Sample type of the data
      int            m_pdim;                                 // Dim of the domain
      axis_info_t    m_axes[ANONYMOUS_NDBUFFER_DEFAULT_DIM]; // Note: in the future, it will be a SBO (for now it is a
                                                             // limitation the number of dims)
      std::byte* m_buffer;                                   // Pointer to origin (0,0) (it might be out of range)
    };
  } // namespace details

  template <>
  class __ndbuffer_image<void, -1> : protected details::ndbuffer_image_info_t
  {
  public:
    static constexpr int DEFAULT_BORDER_SIZE = 3;
    static constexpr int DEFAULT_DIM         = 4;


    /// Type definitions
    /// \{
    using point_type      = ano::Point;
    using value_type      = void*;       // FIXME: std::any
    using reference       = void*;       // FIXME: any_ref
    using const_reference = const void*; // FIXME: any_ref
    using index_type      = int;
    /// \}


    /// \brief Constructors
    /// \{
    __ndbuffer_image();
    __ndbuffer_image(sample_type_id sample_type, int width, int height = 1, int depth = 1);
    __ndbuffer_image(sample_type_id sample_type, int width, const image_build_params& params);
    __ndbuffer_image(sample_type_id sample_type, int width, int height, const image_build_params& params);
    __ndbuffer_image(sample_type_id sample_type, int width, int height, int depth, const image_build_params& params);

    __ndbuffer_image(sample_type_id sample_type, ConstBoxRef domain);
    __ndbuffer_image(sample_type_id sample_type, ConstBoxRef domain, const image_build_params&);

    __ndbuffer_image(const __ndbuffer_image& other, const image_build_params&);

    template <class T>
    __ndbuffer_image(std::initializer_list<T>);

    template <class T>
    __ndbuffer_image(std::initializer_list<std::initializer_list<T>>);

    template <class T>
    __ndbuffer_image(std::initializer_list<std::initializer_list<std::initializer_list<T>>>);

  protected:
    __ndbuffer_image(ndbuffer_image_info_t x);

  private:
    // Constructor from initializer lists
    __ndbuffer_image(sample_type_id sample_type, const details::init_list& l);
    /// \}


    /// \brief Resizer
    /// \{
  public:
    void resize(sample_type_id sample_type, int width, int height = 1, int depth = 1);
    void resize(sample_type_id sample_type, int width, const image_build_params&);
    void resize(sample_type_id sample_type, int width, int height, const image_build_params&);
    void resize(sample_type_id sample_type, int width, int height, int depth, const image_build_params&);
    void resize(sample_type_id sample_type, ConstBoxRef domain);
    void resize(sample_type_id sample_type, ConstBoxRef domain, const image_build_params&);
    void resize(const __ndbuffer_image& other, image_build_params params);

    // unsafe API (consider make it private ?)
    void resize(sample_type_id sample_type, int dim, const int sizes[], const image_build_params&);
    void resize(sample_type_id sample_type, int dim, const int topleft[], const int sizes[], const image_build_params&);
    /// \}


  public:
    /// \{
    static __ndbuffer_image from_buffer(std::byte* buffer, sample_type_id sample_type, int dim, const int sizes[],
                                        const std::ptrdiff_t byte_strides[] = nullptr, bool copy = false);
    static __ndbuffer_image from_buffer(std::byte* buffer, sample_type_id sample_type, int dim, const int topleft[],
                                        const int sizes[], const std::ptrdiff_t byte_strides[] = nullptr,
                                        bool copy = false);

    /// Convert from legacy image
    template <class T, unsigned dim, typename E>
    static __ndbuffer_image from(const ano::ndimage_base<T, dim, E>& other);

    /// Convert to legacy image
    template <class T, unsigned dim, typename E>
    void to(ano::ndimage_base<T, dim, E>& other, bool copy);
    /// \}

    /// \brief Geometry information
    /// \{
    int pdim() const noexcept;
    int width() const noexcept;
    int height() const noexcept;
    int depth() const noexcept;
    int size(int dim = 0) const noexcept;

    int border() const noexcept;
    Box domain() const noexcept;
    /// \}


    /// \brief Data and layout information
    /// \{
    sample_type_id sample_type() const noexcept;
    std::byte*     buffer() const noexcept;
    std::ptrdiff_t byte_stride(int dim = 1) const noexcept;
    std::ptrdiff_t stride(int dim = 1) const noexcept;
    value_set<>    get_value_set() const noexcept;

    index_type index_of_point(ConstPointRef p) const noexcept;
    point_type point_at_index(index_type i) const noexcept;
    index_type delta_index(ConstPointRef p) const noexcept;
    /// \}

    /// Extension-related and domain-releated operation
    /// \{

    /// \brief Inflate (or reduce, if the value is negative) the domain size by
    /// a given amount, by reducing (inflating) the border.
    ///
    /// \note No memory reallocation is performed by the method
    ///
    /// \param[in] delta The domain inflation value. It can be negative.
    /// \precondition The extension must be wide enough, i.e. delta < border()
    void inflate_domain(int delta);


    /// \brief Inflate the domain to match the underlying extension
    ///
    /// \note No memory reallocation is performed by the method
    void inflate_domain_to_extension();


    /// \brief Shift the topleft corner of the domain. The size of the domain is not affected
    void shift_domain_topleft(ConstPointRef dp) noexcept;

    /// \brief Set the topleft corner of the domain. The size of the domain is not affected.
    void set_domain_topleft(ConstPointRef p) noexcept;
    /// \}


    /// Slicing & clipping operations
    /// \{
    __ndbuffer_image slice(int z) const;
    __ndbuffer_image row(int y) const;
    __ndbuffer_image clip(ConstBoxRef roi) const;
    /// \}

    /// Value access
    /// \{
    const void* operator()(ConstPointRef p) const noexcept;
    void*       operator()(ConstPointRef p) noexcept;
    const void* at(ConstPointRef p) const noexcept;
    void*       at(ConstPointRef p) noexcept;
    const void* operator[](index_type i) const noexcept;
    void*       operator[](index_type i) noexcept;
    /// \}


    /// Casting
    /// \{
    template <class T, int N>
    const __ndbuffer_image<T, N>* cast_to() const;

    template <class T, int N>
    __ndbuffer_image<T, N>* cast_to();
    /// \}

    template <class T, int N>
    __ndbuffer_image<T, N>& __cast();

    template <class T, int N>
    const __ndbuffer_image<T, N>& __cast() const;

  protected:
    /// Private and unsafe API (no bound checking) to be used with type-safe and downstream classes

  protected:
    using alloc_fun_t = std::function<std::byte*(std::size_t, std::size_t, const image_build_params&,
                                                 std::shared_ptr<internal::ndbuffer_image_data>&)>;

    // Note __init is called from a constructor, we need to pass the allocation function from children (it cannot be
    // made virtual !!) Create from a custom storage (either external or internal)
    void __init(alloc_fun_t __allocate, sample_type_id sample_type, std::size_t sample_type_size, int dim,
                const int topleft[], const int sizes[], const std::ptrdiff_t byte_strides[],
                const image_build_params& params);


    static __ndbuffer_image __from_legacy_image(std::byte* buffer, sample_type_id sample_type, int dim,
                                                const int topleft[], const int sizes[],
                                                const std::ptrdiff_t byte_strides[], int border);

  private:
    std::shared_ptr<internal::ndbuffer_image_data> m_data;

  protected:
    // Read-only access to member for child class
    const details::ndbuffer_image_info_t* __info() const { return this; }
    const axis_info_t&                    __axes(int i) const { return m_axes[i]; }
    std::byte*                            __buf() const { return m_buffer; }

  public:
    /// Access to the shared_ptr holding the data
    // SHOULD NOT BE USED EXCEPT FOR PYTHON BINDINGS
    std::shared_ptr<internal::ndbuffer_image_data>&       __data();
    const std::shared_ptr<internal::ndbuffer_image_data>& __data() const;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class T>
  __ndbuffer_image<void, -1>::__ndbuffer_image(std::initializer_list<T> l)
    : __ndbuffer_image(sample_type_traits<T>::id(), details::init_list(l))
  {
  }

  template <class T>
  __ndbuffer_image<void, -1>::__ndbuffer_image(std::initializer_list<std::initializer_list<T>> l)
    : __ndbuffer_image(sample_type_traits<T>::id(), details::init_list(l))
  {
  }


  template <class T>
  __ndbuffer_image<void, -1>::__ndbuffer_image(std::initializer_list<std::initializer_list<std::initializer_list<T>>> l)
    : __ndbuffer_image(sample_type_traits<T>::id(), details::init_list(l))
  {
  }


  template <class T, int N>
  const __ndbuffer_image<T, N>* __ndbuffer_image<void, -1>::cast_to() const
  {
    if (m_pdim == N && sample_type_traits<T>::id() == m_sample_type)
      return static_cast<const __ndbuffer_image<T, N>*>(this);

    return nullptr;
  }

  template <class T, int N>
  __ndbuffer_image<T, N>* __ndbuffer_image<void, -1>::cast_to()
  {
    if (m_pdim == N && sample_type_traits<T>::id() == m_sample_type)
      return static_cast<__ndbuffer_image<T, N>*>(this);

    return nullptr;
  }


  template <class T, int N>
  const __ndbuffer_image<T, N>& __ndbuffer_image<void, -1>::__cast() const
  {
    assert(m_pdim == N && sample_type_traits<T>::id() == m_sample_type);
    return static_cast<const __ndbuffer_image<T, N>&>(*this);
  }


  template <class T, int N>
  __ndbuffer_image<T, N>& __ndbuffer_image<void, -1>::__cast()
  {
    assert(m_pdim == N && sample_type_traits<T>::id() == m_sample_type);
    return static_cast<__ndbuffer_image<T, N>&>(*this);
  }


  template <class T, unsigned dim, typename E>
  auto __ndbuffer_image<void, -1>::from(const ano::ndimage_base<T, dim, E>& other) -> __ndbuffer_image
  {
    int            topleft[dim];
    int            sizes[dim];
    std::ptrdiff_t strides[dim];
    auto           domain = other.domain();

    for (int i = 0; i < static_cast<int>(dim); ++i)
    {
      topleft[i] = domain.pmin[dim - i - 1];
      sizes[i]   = domain.pmax[dim - i - 1] - domain.pmin[dim - i - 1];
      strides[i] = other.strides(dim - i - 1);
    }

    return __from_legacy_image(reinterpret_cast<std::byte*>(const_cast<T*>(other.data())), sample_type_traits<T>::id(),
                               dim, topleft, sizes, strides, other.border());
  }


  template <class T, unsigned dim, typename E>
  void __ndbuffer_image<void, -1>::to(ano::ndimage_base<T, dim, E>& other, bool copy)
  {
    assert(dim == m_pdim);
    assert(sample_type_traits<T>::id() == m_sample_type);

    typename E::domain_type domain;
    std::size_t             strides[dim];

    std::byte* ptr = m_buffer;
    for (int i = 0; i < static_cast<int>(dim); ++i)
    {
      domain.pmin[i] = m_axes[dim - 1 - i].vbox_begin;
      domain.pmax[i] = m_axes[dim - 1 - i].vbox_end;
      strides[i]     = m_axes[dim - 1 - i].byte_stride;
      ptr += domain.pmin[i] * strides[i];
    }
    other = E::from_buffer(ptr, domain, strides, copy);
    other.inflate_domain(-this->border());
  }


} // namespace ano
