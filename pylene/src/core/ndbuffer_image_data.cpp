#include <mln/core/image_format.hpp>
#include <mln/core/image/experimental/private/ndbuffer_image_data.hpp>


namespace mln::internal
{
  __ndbuffer_image_data<void>::__ndbuffer_image_data(std::size_t n, sample_type_id sample_type)
  {
    auto t   = get_sample_type_id_traits(sample_type);
    m_size   = n * t.size();
    m_buffer = m_allocator.allocate(m_size);
  }

  __ndbuffer_image_data<void>::~__ndbuffer_image_data()
  {
    m_allocator.deallocate(m_buffer, m_size);
  }

} // namespace mln::internal
