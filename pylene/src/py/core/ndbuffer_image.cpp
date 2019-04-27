#include <mln/core/assert.hpp>

#include <py/core/image_format.hpp>
#include <py/core/ndbuffer_image.hpp>
#include <py/core/private/ndbuffer_image_data.hpp>

#include <cstring>


namespace
{
  constexpr int PYLENE_NDBUFFER_MAX_DIM = 16;
}

namespace mln
{


  ndbuffer_image::ndbuffer_image()
    : m_sample_type{sample_type_id::OTHER}
    , m_pdim{0}
  {
  }

  ndbuffer_image::ndbuffer_image(sample_type_id sample_type, Box domain, const image_build_params& params)
  {
    resize(sample_type, domain, params);
  }

  ndbuffer_image::ndbuffer_image(sample_type_id sample_type, Box domain) { resize(sample_type, domain); }

  ndbuffer_image::ndbuffer_image(sample_type_id sample_type, int width, int height, int depth)
  {
    resize(sample_type, width, height, depth);
  }
  ndbuffer_image::ndbuffer_image(sample_type_id sample_type, int width, const image_build_params& params)
  {
    resize(sample_type, width, params);
  }
  ndbuffer_image::ndbuffer_image(sample_type_id sample_type, int width, int height, const image_build_params& params)
  {
    resize(sample_type, width, height, params);
  }
  ndbuffer_image::ndbuffer_image(sample_type_id sample_type, int width, int height, int depth,
                                 const image_build_params& params)
  {
    resize(sample_type, width, height, depth, params);
  }

  ndbuffer_image::ndbuffer_image(const ndbuffer_image& other, const image_build_params& params)
  {
    resize(other, params);
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

  void ndbuffer_image::__init(alloc_fun_t __allocate, sample_type_id sample_type, int dim, int topleft[], int sizes[],
                              std::ptrdiff_t strides[], const image_build_params& params)
  {
    mln_precondition(topleft != nullptr);
    mln_precondition(sizes != nullptr);
    mln_precondition(dim > 0);

    for (int i = 0; i < dim; ++i)
      mln_precondition(sizes[i] > 0);

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
        m_axes[k].stride = (sizes[k-1] + 2 * border) * m_axes[k-1].stride;
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
    m_buffer = __allocate(sample_type, size, params, m_data); // dynamic behavior

    // Set buffer to (0,0) position
    std::ptrdiff_t x = -m_axes[0].vbox_begin;
    for (int k = 1; k < m_pdim; ++k)
      x += -m_axes[k].vbox_begin * m_axes[k].stride;
    m_buffer += x * get_sample_type_id_traits(sample_type).size();
  }


  void ndbuffer_image::resize(sample_type_id sample_type, int dim, int topleft[], int sizes[],
                              const image_build_params& params)
  {
    __init(__allocate, sample_type, dim, topleft, sizes, nullptr, params);
  }


  void ndbuffer_image::resize(sample_type_id sample_type, int dim, int sizes[], const image_build_params& params)
  {
    int topleft[PYLENE_NDBUFFER_MAX_DIM] = {0};
    resize(sample_type, dim, topleft, sizes, params);
  }

  void ndbuffer_image::resize(sample_type_id sample_type, int width, int height, int depth,
                              const image_build_params& params)
  {
    int topleft[PYLENE_NDBUFFER_MAX_DIM] = {0};
    int sizes[PYLENE_NDBUFFER_MAX_DIM]   = {width, height, depth};

    int dim = 1;
    if (depth != 1)
      dim = 3;
    else if (height != 1)
      dim = 2;
    else if (width != 1)
      dim = 1;
    resize(sample_type, dim, topleft, sizes, params);
  }

  void ndbuffer_image::resize(sample_type_id sample_type, int width, int height, const image_build_params& params)
  {
    resize(sample_type, width, height, 1, params);
  }

  void ndbuffer_image::resize(sample_type_id sample_type, int width, const image_build_params& params)
  {
    resize(sample_type, width, 1, 1, params);
  }

  void ndbuffer_image::resize(sample_type_id sample_type, int width, int height, int depth)
  {
    resize(sample_type, width, height, depth, image_build_params{});
  }

  void ndbuffer_image::resize(sample_type_id sample_type, Box domain, const image_build_params& params)
  {
    int sizes[PYLENE_NDBUFFER_MAX_DIM];
    int dim = domain.get_real_dim();
    for (int i = 0; i < dim; ++i)
      sizes[i] = domain.end[i] - domain.begin[i];

    resize(sample_type, dim, domain.begin.data, sizes, params);
  }

  void ndbuffer_image::resize(sample_type_id sample_type, Box domain)
  {
    resize(sample_type, domain, image_build_params{});
  }


  void ndbuffer_image::resize(const ndbuffer_image& other, image_build_params params)
  {
    if (params.border == -1)
      params.border = other.border();

    int d = other.m_pdim;
    int topleft[PYLENE_NDBUFFER_MAX_DIM];
    int sizes[PYLENE_NDBUFFER_MAX_DIM];
    for (int i = 0; i < d; ++i)
    {
      topleft[i] = other.m_axes[i].domain_begin;
      sizes[i] = other.m_axes[i].domain_end - other.m_axes[i].domain_begin;
    }

    resize(other.m_sample_type, d, topleft, sizes, std::move(params));
  }


  ndbuffer_image ndbuffer_image::from_buffer(std::byte* buffer, sample_type_id sample_type, int dim, int topleft[],
                                             int sizes[], std::ptrdiff_t strides[], bool copy)
  {
    image_build_params params;
    params.border = 0; // From external data, no border

    alloc_fun_t __alloc;
    if (copy)
      __alloc = [&buffer](sample_type_id st, std::size_t n, const image_build_params&,
                          std::shared_ptr<internal::ndbuffer_image_data>& data) {
        data = std::make_shared<internal::__ndbuffer_image_data<void>>(n, st);
        std::memcpy(data->m_buffer, buffer, n * get_sample_type_id_traits(st).size());
        return buffer;
      };
    else
      __alloc = [&buffer](...) { return buffer; };


    ndbuffer_image out;
    out.__init(__alloc, sample_type, dim, topleft, sizes, strides, params);
    return out;
  }


  ndbuffer_image ndbuffer_image::from_buffer(std::byte* buffer, sample_type_id sample_type, int dim, int sizes[],
                                             std::ptrdiff_t strides[], bool copy)
  {
    int topleft[PYLENE_NDBUFFER_MAX_DIM] = {0};
    return ndbuffer_image::from_buffer(buffer, sample_type, dim, topleft, sizes, strides, copy);
  }

  int ndbuffer_image::pdim() const { return m_pdim; }

  int ndbuffer_image::width() const {return m_axes[0].domain_end - m_axes[0].domain_begin; }

  int ndbuffer_image::height() const { return m_axes[1].domain_end - m_axes[1].domain_begin; }

  int ndbuffer_image::depth() const { return m_axes[2].domain_end - m_axes[2].domain_begin; }

  int ndbuffer_image::size(int k) const { return (k >= m_pdim) ? 1 : (m_axes[k].domain_end - m_axes[k].domain_begin); }

  int ndbuffer_image::border() const
  {
    int border = std::max(m_axes[0].domain_begin - m_axes[0].vbox_begin, m_axes[0].vbox_end - m_axes[0].domain_end);
    for (int k = 1; k < m_pdim; ++k)
    {
      int x = std::max(m_axes[k].domain_begin - m_axes[k].vbox_begin, m_axes[k].vbox_end - m_axes[k].domain_end);
      if (x < border)
        border = x;
    }
    return border;
  }


  sample_type_id ndbuffer_image::sample_type() const { return m_sample_type; }


  std::byte* ndbuffer_image::__at(const int coords[]) const
  {
    std::ptrdiff_t x = coords[0];
    for (int k = 1; k < m_pdim; ++k)
      x += coords[k] * m_axes[k].stride;
    return m_buffer + (x * get_sample_type_id_traits(m_sample_type).size());
  }

  std::byte* ndbuffer_image::buffer() const
  {
    int coords[PYLENE_NDBUFFER_MAX_DIM];
    for (int k = 0; k < m_pdim; ++k)
      coords[k] = m_axes[k].domain_begin;
    return this->__at(coords);
  }

  std::ptrdiff_t ndbuffer_image::byte_stride(int dim) const
  {
    mln_precondition(m_sample_type != sample_type_id::OTHER);
    return m_axes[dim].stride * get_sample_type_id_traits(m_sample_type).size();
  }

  std::ptrdiff_t ndbuffer_image::stride(int dim) const { return m_axes[dim].stride; }

} // namespace mln
