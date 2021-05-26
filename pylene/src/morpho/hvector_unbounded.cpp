#include <mln/morpho/private/hvector_unbounded.hpp>

namespace mln::morpho::details
{
  namespace
  {
    static constexpr int kInitReservationPerLevel = 256;
  }

  hvectors_unbounded<void>::hvectors_unbounded(int nlevel, std::size_t size) { this->initialize(nlevel, size); }

  void hvectors_unbounded<void>::initialize(int nlevel, std::size_t size)
  {
    constexpr std::size_t K = alignof(std::max_align_t);

    std::size_t hdr_size      = sizeof(node_t) * nlevel;
    std::size_t data_size     = size * nlevel * kInitReservationPerLevel;
    std::size_t hdr_full_size = (hdr_size + (K - 1)) / K * K; // header with padding for alignment
    std::size_t total_size    = hdr_full_size + data_size;

    assert(hdr_full_size % K == 0 && hdr_size <= hdr_full_size && hdr_full_size < hdr_size + K);


    m_nlevels         = nlevel;
    std::byte* buffer = (std::byte*)std::malloc(total_size);

    m_lists = reinterpret_cast<node_t*>(buffer);
    for (int i = 0; i < nlevel; ++i)
    {
      m_lists[i].size     = 0;
      m_lists[i].capacity = kInitReservationPerLevel;
      m_lists[i].begin    = (void*)(buffer + hdr_full_size + size * kInitReservationPerLevel * i);
    }
  }

  void hvectors_unbounded<void>::release()
  {
    mln_precondition(m_lists != nullptr);
    for (int i = 0; i < m_nlevels; ++i)
    {
      this->destroy_n(m_lists[i].begin, m_lists[i].size);
      if (m_lists[i].capacity > kInitReservationPerLevel)
        std::free(m_lists[i].begin);
    }

    std::free(m_lists);
  }

  void hvectors_unbounded<void>::resize(int i, std::size_t size)
  {
    mln_precondition(m_lists != nullptr);
    constexpr float kResizingFactor = 2;
    int             old_capacity    = m_lists[i].capacity;
    void*           old_ptr         = m_lists[i].begin;

    m_lists[i].capacity = (int)(old_capacity * kResizingFactor);
    m_lists[i].begin    = std::malloc(size * m_lists[i].capacity);

    this->uninitialized_copy_n(old_ptr, m_lists[i].size, m_lists[i].begin);
    this->destroy_n(old_ptr, m_lists[i].size);

    if (old_capacity > kInitReservationPerLevel)
      std::free(old_ptr);
  }

} // namespace mln::morpho::details
