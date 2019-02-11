#include <mln/core/image/private/ndbuffer_image_data.hpp>


namespace mln::internal
{
  __ndbuffer_image_data<void>::__ndbuffer_image_data(std::size_t n)
  {
    m_size   = n;
    m_buffer = m_allocator.allocate(m_size);
  }

  __ndbuffer_image_data<void>::~__ndbuffer_image_data()
  {
    m_allocator.deallocate(m_buffer, m_size);
  }

} // namespace mln::internal
