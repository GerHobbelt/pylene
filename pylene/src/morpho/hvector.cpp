#include <mln/morpho/experimental/private/hvector.hpp>



namespace mln::morpho::experimental::details
{


  void hvectors<void>::init(const std::size_t* cumulated_histogram, std::size_t count, std::size_t size)
  {
    std::size_t nelems = cumulated_histogram[count - 1];
    std::size_t header_size = sizeof(vector_t) * count;
    std::size_t storage_size = nelems * size;
    std::size_t a = alignof(std::max_align_t);
    std::size_t storage_offset = (header_size + a - 1) / a * a; // header_size rounded up
    std::size_t total_size = storage_offset + storage_size;

    void* data = std::malloc(total_size);
    char* storage = (char*)data + storage_offset;

    m_lists = (vector_t*)data;
    m_lists[0] = {storage, storage};
    for (std::size_t i = 1; i < count; ++i)
    {
      m_lists[i].begin = storage + cumulated_histogram[i-1] * size;
      m_lists[i].end   = m_lists[i].begin;
    }
  }


  hvectors<void>::~hvectors()
  {
    std::free(m_lists);
  }





}

