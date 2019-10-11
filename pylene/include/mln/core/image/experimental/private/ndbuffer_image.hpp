#pragma once

#include <mln/core/concept/new/images.hpp>
#include <mln/core/image/experimental/ndbuffer_image.hpp>
#include <mln/core/image/experimental/private/ndbuffer_image_data.hpp>
#include <mln/core/image/experimental/private/ndbuffer_image_impl.hpp>
#include <mln/core/image/experimental/private/ndbuffer_image_pixel.hpp>
#include <mln/core/image/private/ndimage_extension.hpp>
#include <mln/core/rangev3/multi_span.hpp>

namespace mln
{

  template <class T, int N>
  class __ndbuffer_image;

  namespace internal
  {
    template <class T>
    std::byte* ndbuffer_image_allocate(std::size_t n, std::size_t /* sz */, const image_build_params& params,
                                       std::shared_ptr<internal::ndbuffer_image_data>& data)
    {
      if (!params.init_value.has_value())
        data = std::make_shared<internal::__ndbuffer_image_data<T>>(n);
      else
        data = std::make_shared<internal::__ndbuffer_image_data<T>>(n, std::any_cast<T>(params.init_value));
      return data->m_buffer;
    }
  }

  template <class T, int N>
  class __ndbuffer_image : public __ndbuffer_image<void, -1>, public mln::experimental::Image<__ndbuffer_image<T, N>>
  {
    using base = __ndbuffer_image<void, -1>;
    using Impl = mln::details::ndbuffer_image_impl<T, N>;
  public:
    /// Traits
    /// \{
    using category_type              = raw_image_tag;
    using accessible                 = std::true_type;
    using indexable                  = std::true_type;
    using view                       = std::false_type;
    using extension_category         = mln::extension::border_extension_tag;
    /// \}

    /// Type definitions
    /// \{
    using point_type           = experimental::ndpoint<N>;
    using value_type           = T;
    using reference            = T&;
    using const_reference      = const T&;
    using index_type           = int;
    using new_pixel_type       = experimental::details::ndpix<T, N>;
    using new_const_pixel_type = experimental::details::ndpix<const T, N>;

    using domain_type           = experimental::ndbox<N>;
    using extension_type        = internal::ndimage_extension<T, N>;
    using new_value_range       = ranges::multi_span<T, N>;
    using new_const_value_range = ranges::multi_span<const T, N>;
    using new_pixel_range       = experimental::details::ndpix_range<T, N>;
    using new_const_pixel_range = experimental::details::ndpix_range<const T, N>;


    template <class V>
    using ch_value_type = __ndbuffer_image<V, N>;
    using concrete_type = __ndbuffer_image<T, N>;
    /// \}

    /// \brief Constructors
    /// \{
    __ndbuffer_image();

    __ndbuffer_image(experimental::ndbox<N> domain, const image_build_params& params = image_build_params());

    template <int M = N, class = std::enable_if_t<M == 1>>
    __ndbuffer_image(int width, const image_build_params& params = image_build_params());

    template <int M = N, class = std::enable_if_t<M == 2>>
    __ndbuffer_image(int width, int height, const image_build_params& params = image_build_params());

    template <int M = N, class = std::enable_if_t<M == 3>>
    __ndbuffer_image(int width, int height, int depth, const image_build_params& params = image_build_params());


    template <int M = N, class = std::enable_if_t<M == 1>>
    __ndbuffer_image(std::initializer_list<T> values);

    template <int M = N, class = std::enable_if_t<M == 2>>
    __ndbuffer_image(std::initializer_list<std::initializer_list<T>> values);

    template <int M = N, class = std::enable_if_t<M == 3>>
    __ndbuffer_image(std::initializer_list<std::initializer_list<std::initializer_list<T>>> values);


    // FIXME: should be __ndbuffer_image<void, N>
    template <class U>
    __ndbuffer_image(const __ndbuffer_image<U, N>& other, const image_build_params& params);
    /// \}

    /// \name Concrete-related Image Methods
    /// \{
    image_builder<concrete_type, __ndbuffer_image<T,N>>  concretize() const noexcept;

    template <class V>
    image_builder<ch_value_type<V>, __ndbuffer_image<T,N>>  ch_value() const noexcept;
    /// \}


    /// \brief Resize methods
    /// \{
    void resize(experimental::ndbox<N> domain, const image_build_params& params = image_build_params());

    template <int M = N, class = std::enable_if_t<M == 1>>
    void resize(int width, const image_build_params& params = image_build_params());

    template <int M = N, class = std::enable_if_t<M == 2>>
    void resize(int width, int height, const image_build_params& params = image_build_params());

    template <int M = N, class = std::enable_if_t<M == 3>>
    void resize(int width, int height, int depth, const image_build_params& params = image_build_params());

    // FIXME: should be __ndbuffer_image<void, N>
    template <class U>
    void resize(const __ndbuffer_image<U, N>& other, image_build_params params);
    /// \}

    /// \brief Geometry information
    /// \{
    experimental::ndbox<N> domain() const noexcept;
    extension_type         extension() const noexcept;
    /// \}

    /// \brief Data and layout information
    /// \{
    T*                      buffer() noexcept;
    const T*                buffer() const noexcept;
    [[deprecated]] T*       data() noexcept { return this->buffer(); }
    [[deprecated]] const T* data() const noexcept { return this->buffer(); }

    index_type  index_of_point(point_type p) const noexcept;
    point_type  point_at_index(index_type i) const noexcept;
    index_type  delta_index(point_type p) const noexcept;
    /// \}

    /// Slicing & clipping operations
    /// \{
    __ndbuffer_image<T, 2> slice(int z) const;
    __ndbuffer_image<T, 1> row(int y) const;
    __ndbuffer_image<T, N> clip(const experimental::ndbox<N>& roi) const;
    /// \}

    /// Value access
    /// \{
    const_reference operator()(point_type p) const noexcept;
    reference       operator()(point_type p) noexcept;
    const_reference at(point_type p) const noexcept;
    reference       at(point_type p) noexcept;
    const_reference operator[](index_type i) const noexcept;
    reference       operator[](index_type i) noexcept;

    new_pixel_type       new_pixel(point_type p) noexcept;
    new_const_pixel_type new_pixel(point_type p) const noexcept;
    new_pixel_type       new_pixel_at(point_type p) noexcept;
    new_const_pixel_type new_pixel_at(point_type p) const noexcept;


    new_value_range       new_values() noexcept;
    new_const_value_range new_values() const noexcept;
    new_pixel_range       new_pixels() noexcept;
    new_const_pixel_range new_pixels() const noexcept;
    /// \}


    /// \{
    static __ndbuffer_image<T, N> from_buffer(T* buffer, int sizes[], std::ptrdiff_t byte_strides[] = nullptr,
                                              bool copy = false);

    static __ndbuffer_image<T, N> from_buffer(T* buffer, int topleft[], int sizes[],
                                              std::ptrdiff_t byte_strides[] = nullptr, bool copy = false);

    template <class E>
    static __ndbuffer_image<T, N> from(const mln::ndimage_base<T, N, E>& other);
    /// \}

  private:
    void __init(alloc_fun_t __allocate, int topleft[], int sizes[], std::ptrdiff_t byte_strides[],
                const image_build_params& params);


    // Get the index the given coords
    std::ptrdiff_t __index_of_point(const int coords[]) const noexcept;


    new_pixel_type       __pixel_at(point_type p) noexcept;
    new_const_pixel_type __pixel_at(point_type p) const noexcept;

    // Get a data reference at the given coords
    T& __at(const int coords[]) const noexcept;
  };

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class T, int N>
  inline experimental::ndbox<N> __ndbuffer_image<T, N>::domain() const noexcept
  {
    experimental::ndpoint<N> pmin, pmax;
    for (int k = 0; k < N; ++k)
    {
      pmin[k] = this->__axes(k).domain_begin;
      pmax[k] = this->__axes(k).domain_end;
    }
    return {pmin, pmax};
  }


  template <class T, int N>
  inline void __ndbuffer_image<T, N>::__init(alloc_fun_t __allocate, int topleft[], int sizes[], std::ptrdiff_t byte_strides[],
                                      const image_build_params& params)
  {
    base::__init(__allocate, sample_type_traits<T>::id(), sizeof(T), N, topleft, sizes, byte_strides, params);
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::concretize() const noexcept -> image_builder<concrete_type, __ndbuffer_image<T,N>>
  {
    return {*this};
  }

  template <class T, int N>
  template <class V>
  inline
  auto __ndbuffer_image<T, N>::ch_value() const noexcept -> image_builder<ch_value_type<V>, __ndbuffer_image<T, N>>
  {
    return {*this};
  }


  template <class T, int N>
  inline __ndbuffer_image<T, N> __ndbuffer_image<T, N>::from_buffer(T* buffer, int topleft[], int sizes[],
                                                             std::ptrdiff_t byte_strides[], bool copy)
  {
    image_build_params params;
    params.border = 0; // From external data, no border

    alloc_fun_t __alloc;
    if (copy)
      __alloc = [buffer](std::size_t n, std::size_t /* sz */, const image_build_params&,
                         std::shared_ptr<internal::ndbuffer_image_data>& data) -> std::byte* {
        data = std::make_shared<internal::__ndbuffer_image_data<T>>(n);
        std::copy_n(buffer, n, reinterpret_cast<T*>(data->m_buffer));
        return data->m_buffer;
      };
    else
      __alloc = [buffer](std::size_t, std::size_t, const image_build_params&,
                         std::shared_ptr<internal::ndbuffer_image_data>&) -> std::byte* {
        return reinterpret_cast<std::byte*>(buffer);
      };


    __ndbuffer_image<T, N> out;
    out.__init(__alloc, topleft, sizes, byte_strides, params);
    return out;
  }

  template <class T, int N>
  inline __ndbuffer_image<T, N> __ndbuffer_image<T, N>::from_buffer(T* buffer, int sizes[], std::ptrdiff_t byte_strides[], bool copy)
  {
    int topleft[N] = {0};
    return __ndbuffer_image<T, N>::from_buffer(buffer, topleft, sizes, byte_strides, copy);
  }

  template <class T, int N>
  template <class E>
  inline __ndbuffer_image<T, N> __ndbuffer_image<T, N>::from(const mln::ndimage_base<T, N, E>& other)
  {
    auto tmp = base::from(other);
    return static_cast<__ndbuffer_image&>(tmp);
  }


  template <class T, int N>
  inline __ndbuffer_image<T,N>::__ndbuffer_image(experimental::ndbox<N> domain, const image_build_params& params)
  {
    this->resize(domain, params);
  }

  template <class T, int N>
  inline __ndbuffer_image<T, N>::__ndbuffer_image()
    : base(details::ndbuffer_image_info_t{sample_type_traits<T>::id(), N, {}, nullptr})
  {
  }

  template <class T, int N>
  template <int, class>
  inline __ndbuffer_image<T, N>::__ndbuffer_image(int width, const image_build_params& params)
  {
    this->resize(width, params);
  }


  template <class T, int N>
  template <int, class>
  inline __ndbuffer_image<T, N>::__ndbuffer_image(int width, int height, const image_build_params& params)
  {
    this->resize(width, height, params);
  }

  template <class T, int N>
  template <int, class>
  inline __ndbuffer_image<T, N>::__ndbuffer_image(int width, int height, int depth, const image_build_params& params)
  {
    this->resize(width, height, depth, params);
  }


  template <class T, int N>
  template <int M, class>
  inline __ndbuffer_image<T, N>::__ndbuffer_image(std::initializer_list<T> values)
    : base(std::move(values))
  {
  }

  template <class T, int N>
  template <int M, class>
  inline __ndbuffer_image<T, N>::__ndbuffer_image(std::initializer_list<std::initializer_list<T>> values)
    : base(std::move(values))
  {
  }

  template <class T, int N>
  template <int M, class>
  inline __ndbuffer_image<T, N>::__ndbuffer_image(
      std::initializer_list<std::initializer_list<std::initializer_list<T>>> values)
    : base(std::move(values))
  {
  }


  template <class T, int N>
  template <class U>
  inline __ndbuffer_image<T, N>::__ndbuffer_image(const __ndbuffer_image<U, N>& other, const image_build_params& params)
  {
    this->resize(other, params);
  }

  template <class T, int N>
  inline void __ndbuffer_image<T, N>::resize(experimental::ndbox<N> domain, const image_build_params& params)
  {
    int tl[N];
    int sz[N];

    auto pmin = domain.tl();
    for (int k = 0; k < N; ++k)
    {
      tl[k] = pmin[k];
      sz[k] = domain.size(k);
    }
    __init(internal::ndbuffer_image_allocate<T>, tl, sz, nullptr, params);
  }


  template <class T, int N>
  template <int, class>
  inline void __ndbuffer_image<T, N>::resize(int width, const image_build_params& params)
  {
    auto domain = experimental::ndbox<N>(width);
    resize(domain, params);
  }


  template <class T, int N>
  template <int, class>
  inline void __ndbuffer_image<T, N>::resize(int width, int height, const image_build_params& params)
  {
    auto domain = experimental::ndbox<N>(width, height);
    resize(domain, params);
  }


  template <class T, int N>
  template <int, class>
  inline void __ndbuffer_image<T, N>::resize(int width, int height, int depth, const image_build_params& params)
  {
    auto domain = experimental::ndbox<N>(width, height, depth);
    resize(domain, params);
  }


  template <class T, int N>
  template <class U>
  inline void __ndbuffer_image<T, N>::resize(const __ndbuffer_image<U, N>& other, image_build_params params)
  {
    if (params.border == -1)
      params.border = other.border();

    auto domain = other.domain();
    resize(domain, params);
  }


  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::point_at_index(index_type i) const noexcept -> point_type
  {
    point_type coords;
    Impl::get_point(this->__info(), i, coords.data());
    return coords;
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::index_of_point(point_type p) const noexcept -> index_type
  {
    assert(Impl::is_point_valid(this->__info(), p.data()));
    return Impl::get_index(this->__info(), p.data());
  }


  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::delta_index(point_type p) const noexcept -> index_type
  {
    return Impl::get_index(this->__info(), p.data());
  }

  template <class T, int N>
  inline T* __ndbuffer_image<T, N>::buffer() noexcept
  {
    return reinterpret_cast<T*>(this->base::buffer());
  }

  template <class T, int N>
  inline const T* __ndbuffer_image<T, N>::buffer() const noexcept
  {
    return reinterpret_cast<T*>(this->base::buffer());
  }

  template <class T, int N>
  inline __ndbuffer_image<T, 2> __ndbuffer_image<T, N>::slice(int z) const
  {
    int begin[N] = {this->__axes(0).domain_begin, this->__axes(1).domain_begin, z};
    int end[N]   = {this->__axes(0).domain_end,   this->__axes(1).domain_end, z + 1};
    for (int k = 3; k < m_pdim; ++k)
    {
      begin[k] = 0;
      end[k]   = 1;
    }

    ndbuffer_image_info_t tmp = *this->__info();
    Impl::select(&tmp, 2, begin, end);
    return *Impl::template cast<T, 2>(&tmp);
  }

  template <class T, int N>
  inline __ndbuffer_image<T, 1> __ndbuffer_image<T, N>::row(int y) const
  {
    int begin[N] = {m_axes[0].domain_begin, y};
    int end[N]   = {m_axes[0].domain_end, y + 1};
    for (int k = 2; k < m_pdim; ++k)
    {
      begin[k] = 0;
      end[k]   = 1;
    }

    ndbuffer_image_info_t tmp = *this->__info();
    Impl::select(&tmp, 1, begin, end);
    return *Impl::template cast<T, 1>(&tmp);
  }

  template <class T, int N>
  inline __ndbuffer_image<T, N> __ndbuffer_image<T, N>::clip(const experimental::ndbox<N>& roi) const
  {
    __ndbuffer_image tmp = *this;
    Impl::select(&tmp, N, roi.tl().data(), roi.br().data());
    return tmp;
  }


  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::operator()(point_type p) const noexcept -> const_reference
  {
    assert(Impl::is_point_in_domain(this->__info(), p.data()));
    return *Impl::get_pointer(this->__info(), p.data());
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::operator()(point_type p) noexcept -> reference
  {
    assert(Impl::is_point_in_domain(this->__info(), p.data()));
    return *Impl::get_pointer(this->__info(), p.data());
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::__pixel_at(point_type p) const noexcept -> new_const_pixel_type
  {
    point_type lcoords = p;
    lcoords[0]         = 0;
    const T* lineptr   = Impl::get_pointer(this->__info(), lcoords.data());
    return new_const_pixel_type{{}, this->__info(), lineptr, p};
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::__pixel_at(point_type p) noexcept -> new_pixel_type
  {
    point_type lcoords = p;
    lcoords[0]         = 0;
    T* lineptr         = Impl::get_pointer(this->__info(), lcoords.data());
    return new_pixel_type{{}, this->__info(), lineptr, p};
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::new_pixel(point_type p) const noexcept -> new_const_pixel_type
  {
    assert(Impl::is_point_in_domain(this->__info(), p.data()));
    return this->__pixel_at(p);
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::new_pixel(point_type p) noexcept -> new_pixel_type
  {
    assert(Impl::is_point_in_domain(this->__info(), p.data()));
    return this->__pixel_at(p);
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::new_pixel_at(point_type p) const noexcept -> new_const_pixel_type
  {
    assert(Impl::is_point_valid(this->__info(), p.data()));
    return this->__pixel_at(p);
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::new_pixel_at(point_type p) noexcept -> new_pixel_type
  {
    assert(Impl::is_point_valid(this->__info(), p.data()));
    return this->__pixel_at(p);
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::at(point_type p) const noexcept -> const_reference
  {
    assert(Impl::is_point_valid(this->__info(), p.data()));
    return *Impl::get_pointer(this->__info(), p.data());
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::at(point_type p) noexcept -> reference
  {
    assert(Impl::is_point_valid(this->__info(), p.data()));
    return *Impl::get_pointer(this->__info(), p.data());
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::operator[](index_type i) const noexcept -> const_reference
  {
    return *Impl::get_pointer(this->__info(), i);
  }

  template <class T, int N>
  inline auto __ndbuffer_image<T, N>::operator[](index_type i) noexcept -> reference
  {
    return *Impl::get_pointer(this->__info(), i);
  }


  template <class T, int N>
  inline ranges::multi_span<T, N> __ndbuffer_image<T, N>::new_values() noexcept
  {
    std::array<std::size_t, N>    counts;
    std::array<std::ptrdiff_t, N> strides;
    for (int i = 0; i < N; ++i)
    {
      counts[N - i - 1]  = static_cast<std::size_t>(this->__axes(i).domain_end - this->__axes(i).domain_begin);
      strides[N - i - 1] = this->__axes(i).byte_stride / sizeof(T);
    }
    return {this->buffer(), counts, strides};
  }

  template <class T, int N>
  inline ranges::multi_span<const T, N> __ndbuffer_image<T, N>::new_values() const noexcept
  {
    std::array<std::size_t, N>    counts;
    std::array<std::ptrdiff_t, N> strides;
    for (int i = 0; i < N; ++i)
    {
      counts[N - i - 1]  = static_cast<std::size_t>(this->__axes(i).domain_end - this->__axes(i).domain_begin);
      strides[N - i - 1] = this->__axes(i).byte_stride / sizeof(T);
    }
    return {this->buffer(), counts, strides};
  }


  template <class T, int N>
  inline experimental::details::ndpix_range<T, N> __ndbuffer_image<T, N>::new_pixels() noexcept
  {
    return {this->__info()};
  }


  template <class T, int N>
  inline experimental::details::ndpix_range<const T, N> __ndbuffer_image<T, N>::new_pixels() const noexcept
  {
    return {this->__info()};
  }

  template <class T, int N>
  mln::internal::ndimage_extension<T, N> __ndbuffer_image<T, N>::extension() const noexcept
  {
    // Get the pointer of the first point
    // Old impl

    mln::point<short, N> shp;
    int                  coords[N];
    std::size_t          strides[N];

    for (int k = 0; k < N; ++k)
    {
      coords[k]  = this->__axes(k).domain_begin;
      shp[k]     = static_cast<short>(this->__axes(k).domain_end - this->__axes(k).domain_begin);
      strides[k] = this->__axes(k).byte_stride / sizeof(T);
    }

    auto ptr = Impl::get_pointer(this->__info(), coords);
    return mln::internal::ndimage_extension<T, N>{(char*)ptr, strides, shp, this->border()};
  }


} // namespace mln
