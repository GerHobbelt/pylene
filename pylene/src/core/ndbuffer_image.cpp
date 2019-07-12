#include <mln/core/assert.hpp>

#include <mln/core/image_format.hpp>
#include <mln/core/image/experimental/ndbuffer_image.hpp>
#include <mln/core/image/experimental/private/ndbuffer_image_data.hpp>
#include <mln/core/image/experimental/private/ndbuffer_image_impl.hpp>

#include <cstring>


namespace
{
  constexpr int PYLENE_NDBUFFER_MAX_DIM = 16;


  using Impl = mln::details::ndbuffer_image_impl<void, -1>;
}

namespace mln
{

  __ndbuffer_image<void, -1>::__ndbuffer_image()
    : ndbuffer_image_info_t{} // Force 0-initialization
  {
  }


  __ndbuffer_image<void, -1>::__ndbuffer_image(ndbuffer_image_info_t x)
    : ndbuffer_image_info_t{x}
  {
  }


  __ndbuffer_image<void, -1>::__ndbuffer_image(sample_type_id sample_type, experimental::ConstBoxRef domain,
                                             const image_build_params& params)
  {
    resize(sample_type, domain, params);
  }


  __ndbuffer_image<void, -1>::__ndbuffer_image(sample_type_id sample_type, experimental::ConstBoxRef domain)
  {
    resize(sample_type, domain);
  }


  __ndbuffer_image<void, -1>::__ndbuffer_image(sample_type_id sample_type, int width, int height, int depth)
  {
    resize(sample_type, width, height, depth);
  }

  __ndbuffer_image<void, -1>::__ndbuffer_image(sample_type_id sample_type, int width, const image_build_params& params)
  {
    resize(sample_type, width, params);
  }

  __ndbuffer_image<void, -1>::__ndbuffer_image(sample_type_id sample_type, int width, int height,
                                             const image_build_params& params)
  {
    resize(sample_type, width, height, params);
  }

  __ndbuffer_image<void, -1>::__ndbuffer_image(sample_type_id sample_type, int width, int height, int depth,
                                             const image_build_params& params)
  {
    resize(sample_type, width, height, depth, params);
  }


  __ndbuffer_image<void, -1>::__ndbuffer_image(const __ndbuffer_image& other, const image_build_params& params)
  {
    resize(other, params);
  }


  __ndbuffer_image<void, -1>::__ndbuffer_image(sample_type_id sample_type, const details::init_list& data)
  {
    int sizes[3] = {static_cast<int>(data.size(0)), static_cast<int>(data.size(1)), static_cast<int>(data.size(2))};
    int dim      = data.dim();

    resize(sample_type, dim, sizes, image_build_params{});

    // Copy data
    std::ptrdiff_t line_byte_stride  = dim > 1 ? this->byte_stride(1) : 0;
    std::ptrdiff_t slice_byte_stride = dim > 2 ? this->byte_stride(2) : 0;
    data.copy_to_buffer(this->m_buffer, line_byte_stride, slice_byte_stride);
  }



  experimental::Box __ndbuffer_image<void, -1>::domain() const noexcept
  {
    int a_coords[PYLENE_NDBUFFER_MAX_DIM];
    int b_coords[PYLENE_NDBUFFER_MAX_DIM];

    for (int i = 0; i < m_pdim; ++i)
    {
      a_coords[i] = m_axes[i].domain_begin;
      b_coords[i] = m_axes[i].domain_end;
    }

    experimental::ConstPointRef a(m_pdim, a_coords);
    experimental::ConstPointRef b(m_pdim, b_coords);
    return {a, b};
  }


  namespace
  {
    std::byte* __allocate(sample_type_id sample_type, std::size_t n, const image_build_params&,
                          std::shared_ptr<internal::ndbuffer_image_data>& data)
    {
      data = std::make_shared<internal::__ndbuffer_image_data<void>>(n, sample_type);
      return data->m_buffer;
    }
  } // namespace


  void __ndbuffer_image<void, -1>::__init(alloc_fun_t __allocate, sample_type_id sample_type, int dim,
                                          const int topleft[], const int sizes[], const std::ptrdiff_t strides[],
                                          const image_build_params& params)
  {
    mln_precondition(topleft != nullptr);
    mln_precondition(sizes != nullptr);
    mln_precondition(dim > 0);

    int border = (params.border >= 0) ? params.border : DEFAULT_BORDER_SIZE;

    if (dim > 4)
    {
      throw std::runtime_error("Dynamic dimension greater than 4 are not supported");
    }

    // Set sample type and other info
    m_sample_type = sample_type;
    m_pdim        = dim;

    // Set domain and extension
    for (int k = 0; k < dim; ++k)
    {
      m_axes[k].domain_begin = topleft[k];
      m_axes[k].domain_end   = topleft[k] + sizes[k];
      m_axes[k].vbox_begin   = m_axes[k].domain_begin - border;
      m_axes[k].vbox_end     = m_axes[k].domain_end + border;
    }

    // Compute strides
    if (strides == nullptr)
    {
      m_axes[0].stride = 1;
      for (int k = 1; k < dim; ++k)
        m_axes[k].stride = (sizes[k - 1] + 2 * border) * m_axes[k - 1].stride;
    }
    else // Or copy strides
    {
      for (int k = 0; k < dim; ++k)
        m_axes[k].stride = strides[k];
    }

    // Set out-of-dim values
    for (int k = dim; k < DEFAULT_DIM; ++k)
    {
      m_axes[k].domain_begin = 0;
      m_axes[k].domain_end   = 1;
      m_axes[k].vbox_begin   = 0;
      m_axes[k].vbox_end     = 1;
      m_axes[k].stride       = 0;
    }

    // Allocate (or get the buffer)
    std::size_t size = (sizes[dim - 1] + 2 * border) * m_axes[dim - 1].stride;
    m_buffer         = __allocate(sample_type, size, params, m_data); // dynamic behavior

    // Set buffer to (0,0) position
    std::ptrdiff_t x = -m_axes[0].vbox_begin;
    for (int k = 1; k < m_pdim; ++k)
      x += -m_axes[k].vbox_begin * m_axes[k].stride;
    m_buffer += x * get_sample_type_id_traits(sample_type).size();
  }


  void
  __ndbuffer_image<void, -1>::resize(sample_type_id sample_type, int dim, const int topleft[], const int sizes[],
                                     const image_build_params& params)
  {
    __init(__allocate, sample_type, dim, topleft, sizes, nullptr, params);
  }


  void
  __ndbuffer_image<void, -1>::resize(sample_type_id sample_type, int dim, const int sizes[],
                                     const image_build_params& params)
  {
    int topleft[PYLENE_NDBUFFER_MAX_DIM] = {0};
    resize(sample_type, dim, topleft, sizes, params);
  }

  void
  __ndbuffer_image<void, -1>::resize(sample_type_id sample_type, int width, int height, int depth,
                                     const image_build_params& params)
  {
    int topleft[PYLENE_NDBUFFER_MAX_DIM] = {0};
    int sizes[PYLENE_NDBUFFER_MAX_DIM]   = {width, height, depth};
    for (int i = 3; i < PYLENE_NDBUFFER_MAX_DIM; ++i)
      sizes[i] = 1;

    int dim = 1;
    if (depth != 1)
      dim = 3;
    else if (height != 1)
      dim = 2;
    else if (width != 1)
      dim = 1;
    resize(sample_type, dim, topleft, sizes, params);
  }


  void __ndbuffer_image<void, -1>::resize(sample_type_id sample_type, int width, int height,
                                          const image_build_params& params)
  {
    resize(sample_type, width, height, 1, params);
  }


  void __ndbuffer_image<void, -1>::resize(sample_type_id sample_type, int width, const image_build_params& params)
  {
    resize(sample_type, width, 1, 1, params);
  }


  void __ndbuffer_image<void, -1>::resize(sample_type_id sample_type, int width, int height, int depth)
  {
    resize(sample_type, width, height, depth, image_build_params{});
  }


  void __ndbuffer_image<void, -1>::resize(sample_type_id sample_type, experimental::ConstBoxRef domain,
                                          const image_build_params& params)
  {
    int topleft[PYLENE_NDBUFFER_MAX_DIM];
    int sizes[PYLENE_NDBUFFER_MAX_DIM];
    int dim = domain.dim();
    for (int i = 0; i < dim; ++i)
    {
      topleft[i] = domain.tl(i);
      sizes[i]   = domain.size(i);
    }

    resize(sample_type, dim, topleft, sizes, params);
  }


  void __ndbuffer_image<void, -1>::resize(sample_type_id sample_type, experimental::ConstBoxRef domain)
  {
    resize(sample_type, domain, image_build_params{});
  }



  void __ndbuffer_image<void, -1>::resize(const __ndbuffer_image& other, image_build_params params)
  {
    if (params.border == -1)
      params.border = other.border();
    int d = other.m_pdim;
    int topleft[PYLENE_NDBUFFER_MAX_DIM];
    int sizes[PYLENE_NDBUFFER_MAX_DIM];
    for (int i = 0; i < d; ++i)
    {
      topleft[i] = other.m_axes[i].domain_begin;
      sizes[i]   = other.m_axes[i].domain_end - other.m_axes[i].domain_begin;
    }

    resize(other.m_sample_type, d, topleft, sizes, std::move(params));
  }


  auto __ndbuffer_image<void, -1>::from_buffer(std::byte* buffer, sample_type_id sample_type, int dim,
                                               const int topleft[], const int sizes[], const std::ptrdiff_t strides[],
                                               bool copy) -> __ndbuffer_image
  {
    image_build_params params;
    params.border = 0; // From external data, no border

    alloc_fun_t __alloc;
    if (copy)
      __alloc = [buffer](sample_type_id st, std::size_t n, const image_build_params&,
                         std::shared_ptr<internal::ndbuffer_image_data>& data) {
        data = std::make_shared<internal::__ndbuffer_image_data<void>>(n, st);
        std::memcpy(data->m_buffer, buffer, n * get_sample_type_id_traits(st).size());
        return data->m_buffer;
      };
    else
      __alloc = [buffer](sample_type_id, std::size_t, const image_build_params&,
                         std::shared_ptr<internal::ndbuffer_image_data>&) { return buffer; };


    __ndbuffer_image out;
    out.__init(std::move(__alloc), sample_type, dim, topleft, sizes, strides, params);
    return out;
  }


  auto __ndbuffer_image<void, -1>::from_buffer(std::byte* buffer, sample_type_id sample_type, int dim,
                                               const int sizes[], const std::ptrdiff_t strides[], bool copy)
      -> __ndbuffer_image
  {
    int topleft[PYLENE_NDBUFFER_MAX_DIM] = {0};

    return __ndbuffer_image<void, -1>::from_buffer(buffer, sample_type, dim, topleft, sizes, strides, copy);
  }


  auto __ndbuffer_image<void, -1>::__from_legacy_image(std::byte* buffer, sample_type_id sample_type, int dim,
                                                       const int topleft[], const int sizes[],
                                                       const std::ptrdiff_t strides[], int border) -> __ndbuffer_image
  {
    bool copy = false;
    auto ima = __ndbuffer_image<void, -1>::from_buffer(buffer, sample_type, dim, topleft, sizes, strides, copy);
    for (int i = 0; i < dim; ++i)
    {
      ima.m_axes[i].vbox_begin = ima.m_axes[i].domain_begin - border;
      ima.m_axes[i].vbox_end   = ima.m_axes[i].domain_end + border;
    }
    return ima;
  }

  sample_type_id __ndbuffer_image<void, -1>::sample_type() const noexcept
  {
    return m_sample_type;
  }


  int __ndbuffer_image<void, -1>::pdim() const noexcept
  {
    return m_pdim;
  }


  int __ndbuffer_image<void, -1>::width() const noexcept
  {
    return m_axes[0].domain_end - m_axes[0].domain_begin;
  }


  int __ndbuffer_image<void, -1>::height() const noexcept
  {
    return m_axes[1].domain_end - m_axes[1].domain_begin;
  }


  int __ndbuffer_image<void, -1>::depth() const noexcept
  {
    return m_axes[2].domain_end - m_axes[2].domain_begin;
  }


  int __ndbuffer_image<void, -1>::size(int k) const noexcept
  {
    return (k >= m_pdim) ? 1 : (m_axes[k].domain_end - m_axes[k].domain_begin);
  }


  int __ndbuffer_image<void, -1>::border() const noexcept
  {
    if (m_pdim == 0)
      return 0;

    int border = std::max(m_axes[0].domain_begin - m_axes[0].vbox_begin, m_axes[0].vbox_end - m_axes[0].domain_end);
    for (int k = 1; k < m_pdim; ++k)
    {
      int x = std::max(m_axes[k].domain_begin - m_axes[k].vbox_begin, m_axes[k].vbox_end - m_axes[k].domain_end);
      if (x < border)
        border = x;
    }
    return border;
  }


  void __ndbuffer_image<void, -1>::inflate_domain(int delta)
  {
    if (delta > this->border())
      throw std::runtime_error("The border is not wide enough");

    for (int k = 0; k < m_pdim; ++k)
    {
      m_axes[k].domain_begin -= delta;
      m_axes[k].domain_end += delta;
      assert(m_axes[k].domain_begin <= m_axes[k].domain_end);
      assert(m_axes[k].vbox_begin <= m_axes[k].domain_begin);
      assert(m_axes[k].domain_end <= m_axes[k].vbox_end);
    }
  }



  const void* __ndbuffer_image<void, -1>::operator()(experimental::ConstPointRef p) const noexcept
  {
    assert(p.dim() == m_pdim);
    assert(Impl::is_point_in_domain(this, p.data()));
    return Impl::get_pointer(this, p.data());
  }


  void* __ndbuffer_image<void, -1>::operator()(experimental::ConstPointRef p) noexcept
  {
    assert(p.dim() == m_pdim);
    assert(Impl::is_point_in_domain(this, p.data()));
    return Impl::get_pointer(this, p.data());
  }


  const void* __ndbuffer_image<void, -1>::at(experimental::ConstPointRef p) const noexcept
  {
    assert(p.dim() == m_pdim);
    assert(Impl::is_point_valid(this, p.data()));
    return Impl::get_pointer(this, p.data());
  }


  void* __ndbuffer_image<void, -1>::at(experimental::ConstPointRef p) noexcept
  {
    assert(p.dim() == m_pdim);
    assert(Impl::is_point_valid(this, p.data()));
    return Impl::get_pointer(this, p.data());
  }


  const void* __ndbuffer_image<void, -1>::operator[](index_type i) const noexcept
  {
    return Impl::get_pointer(this, i);
  }


  void* __ndbuffer_image<void, -1>::operator[](index_type i) noexcept
  {
    return Impl::get_pointer(this, i);
  }


  auto __ndbuffer_image<void, -1>::index_of_point(experimental::ConstPointRef p) const noexcept->index_type
  {
    assert(p.dim() == m_pdim);
    assert(Impl::is_point_valid(this, p.data()));
    return Impl::get_index(this, p.data());
  }


  auto __ndbuffer_image<void, -1>::delta_index(experimental::ConstPointRef p) const noexcept -> index_type
  {
    assert(p.dim() == m_pdim);
    return Impl::get_index(this, p.data());
  }


  auto __ndbuffer_image<void, -1>::point_at_index(index_type i) const noexcept -> point_type
  {
    point_type coords(m_pdim);
    Impl::get_point(this, i, coords.data());
    return coords;
  }



  std::byte* __ndbuffer_image<void, -1>::buffer() const noexcept
  {
    int coords[PYLENE_NDBUFFER_MAX_DIM];
    for (int k = 0; k < m_pdim; ++k)
      coords[k] = m_axes[k].domain_begin;
    return static_cast<std::byte*>(Impl::get_pointer(this, coords));
  }


  std::ptrdiff_t __ndbuffer_image<void, -1>::byte_stride(int dim) const noexcept
  {
    mln_precondition(m_sample_type != sample_type_id::OTHER);
    return m_axes[dim].stride * get_sample_type_id_traits(m_sample_type).size();
  }


  std::ptrdiff_t __ndbuffer_image<void, -1>::stride(int dim) const noexcept
  {
    return m_axes[dim].stride;
  }



  auto __ndbuffer_image<void, -1>::__select(int dim, const int begin_coords[], const int end_coords[]) const -> __ndbuffer_image
  {
    assert(Impl::is_point_valid(this->__info(), begin_coords) && "Out of bounds coordinates");

    __ndbuffer_image other  = *this;
    std::ptrdiff_t offset = 0;
    for (int k = 0; k < m_pdim; ++k)
    {
      other.m_axes[k].domain_begin = begin_coords[k];
      other.m_axes[k].domain_end   = end_coords[k];
      offset += begin_coords[k] * m_axes[k].stride;
    }
    other.m_pdim = dim;
    other.m_buffer += offset * get_sample_type_id_traits(m_sample_type).size();

    return other;
  }


  auto __ndbuffer_image<void, -1>::clip(experimental::ConstBoxRef roi) const ->  __ndbuffer_image
  {
    if (roi.dim() != m_pdim)
      throw std::runtime_error("Dimension of the image and the roi differ.");

    auto d = this->domain();
    if (!d.includes(roi))
      throw std::runtime_error("The roi must be included in the domain.");

    int begin[PYLENE_NDBUFFER_MAX_DIM];
    int end[PYLENE_NDBUFFER_MAX_DIM];
    for (int k = 0; k < m_pdim; ++k)
    {
      begin[k] = roi.tl(k);
      end[k]   = roi.br(k);
    }
    return __select(m_pdim, begin, end);
  }


  auto __ndbuffer_image<void, -1>::row(int y) const ->  __ndbuffer_image
  {
    if (m_pdim > 2)
      throw std::runtime_error("Dimension of the image must not exceed 2.");

    if (!(m_axes[1].domain_begin <= y && y < m_axes[1].domain_end))
      throw std::runtime_error("Invalid row number.");

    int begin[PYLENE_NDBUFFER_MAX_DIM] = {m_axes[0].domain_begin, y};
    int end[PYLENE_NDBUFFER_MAX_DIM]   = {m_axes[0].domain_end, y + 1};
    for (int k = 2; k < m_pdim; ++k)
    {
      begin[k] = 0;
      end[k]   = 1;
    }
    return __select(1, begin, end);
  }


  auto __ndbuffer_image<void, -1>::slice(int z) const -> __ndbuffer_image
  {
    if (m_pdim > 3)
      throw std::runtime_error("Dimension of the image must not exceed 3.");

    if (!(m_axes[2].domain_begin <= z && z < m_axes[2].domain_end))
      throw std::runtime_error("Invalid slice number.");

    int begin[PYLENE_NDBUFFER_MAX_DIM] = {m_axes[0].domain_begin, m_axes[1].domain_begin, z};
    int end[PYLENE_NDBUFFER_MAX_DIM]   = {m_axes[0].domain_end, m_axes[1].domain_end, z + 1};
    for (int k = 3; k < m_pdim; ++k)
    {
      begin[k] = 0;
      end[k]   = 1;
    }
    return __select(2, begin, end);
  }


} // namespace mln
