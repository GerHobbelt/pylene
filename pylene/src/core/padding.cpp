#include <mln/core/extension/padding.hpp>
#include <cassert>

namespace mln
{

  mdspan_padder<void>::mdspan_padder(void* buffer, int dim, const int sizes[], const std::ptrdiff_t strides[],
                                     const int borders[][2]) noexcept
    : m_buffer{(char*)buffer}
    , m_dim{dim}
  {
    assert(dim < _max_dim && "dim is currently limited to _max_dim");

    for (int k = 0; k < dim; ++k)
    {
      m_sizes[k]        = sizes[k];
      m_byte_strides[k] = strides[k];
      m_borders[k][0]   = borders[k][0];
      m_borders[k][1]   = borders[k][1];
      assert(m_borders[k][0] >= 0);
      assert(m_borders[k][1] >= 0);
      assert((m_borders[k][0] + m_borders[k][1]) < (int)sizes[k]);
    }
    for (int k = dim; k < _max_dim; ++k)
    {
      m_sizes[k]        = 1;
      m_byte_strides[k] = m_byte_strides[k-1];
      m_borders[k][0]   = 0;
      m_borders[k][1]   = 0;
    }
  }

  void mdspan_padder<void>::_pad(e_padding_mode padding, void* value) const noexcept
  {
    if (padding == PAD_ZERO)
      value = nullptr;
    this->_pad(m_dim, m_buffer, padding, value);
  }

  void mdspan_padder<void>::_pad(int dim, char* buffer, e_padding_mode padding, void* value) const noexcept
  {
    assert(dim <= m_dim);

    const auto n = m_sizes[dim - 1];
    const int  a = m_borders[dim - 1][0];
    const int  b = n - m_borders[dim - 1][1];
    const int  w0 = b - a;


    if (dim == 1) {
      this->pad_horizontally_2d(buffer, padding, value);
      return;
    }

    // Pad each element of the lower dim
    if (dim == 2)
    {
      this->pad_horizontally_2d(buffer, padding, value);
    }
    else
    {
      for (int z = a; z < b; ++z)
        this->_pad(dim-1, buffer + m_byte_strides[dim-1] * z, padding, value);
    }

    // Pad upper border
    // Pad lower border
    auto mirror = [w0, a](int x) {
      int n = w0 * 2;
      x     = (x - a) % n;               // x in (-n:n)
      x     = (x >= 0) ? x : x + n;       // x in [0:n)
      x     = (x < w0) ? x : (n - x - 1); // x in [0,w0)
      return a + x;
    };
    auto periodize = [w0, a](int x) {
      x = (x - a) % w0;
      x = (x >= 0) ? x : x + w0;
      return a + x;
    };


    switch (padding)
    {
    case PAD_ZERO:
    case PAD_CONSTANT:
      for (int z = 0; z < a; ++z)
        this->memset(dim - 1, buffer + m_byte_strides[dim - 1] * z, value);
      for (int z = b; z < n; ++z)
        this->memset(dim - 1, buffer + m_byte_strides[dim - 1] * z, value);
      break;
    case PAD_REPLICATE:
      for (int z = 0; z < a; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * a);
      for (int z = b; z < n; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * (b - 1));
      break;
    case PAD_MIRROR:
      for (int z = 0; z < a; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * mirror(z));
      for (int z = b; z < n; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * mirror(z));
      break;
    case PAD_WRAP:
      for (int z = 0; z < a; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * periodize(z));
      for (int z = b; z < n; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * periodize(z));
      break;
    }
  }



  void mdspan_padder<void>::memset(int dim, char* buffer, void* value) const noexcept
  {
    assert(dim < m_dim);
    if (dim == 1)
    {
      this->memset(buffer, m_sizes[dim-1], value);
      return;
    }

    for (int z = 0; z < m_sizes[dim-1]; ++z)
      this->memset(dim - 1, buffer + m_byte_strides[dim - 1] * z, value);
  }

  void mdspan_padder<void>::memcpy(int dim, char* dst, char* src) const noexcept
  {
    assert(dim < m_dim);
    if (dim == 1)
    {
      this->memcpy(dst, src, m_sizes[dim-1]);
      return;
    }

    for (int z = 0; z < m_sizes[dim - 1]; ++z)
      this->memcpy(dim - 1, dst + m_byte_strides[dim - 1] * z, src + m_byte_strides[dim - 1] * z);
  }


} // namespace mln
