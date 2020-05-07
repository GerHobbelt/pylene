#pragma once

#include <cstddef>
#include <memory>

namespace mln::internal
{
  // Polymorphic buffer holder
  struct ndbuffer_image_data
  {
    std::size_t m_size;
    std::byte*  m_buffer;

    virtual ~ndbuffer_image_data() = default;

  protected:
    ndbuffer_image_data() = default;
  };

  template <class T>
  class __ndbuffer_image_data;

  template <>
  class __ndbuffer_image_data<void> final : public ndbuffer_image_data
  {
    std::allocator<std::byte> m_allocator;

  public:

    /// \param n Number of **bytes** to allocate
    __ndbuffer_image_data(std::size_t n);
    ~__ndbuffer_image_data() final;
  };

  template <class T>
  class __ndbuffer_image_data final : public ndbuffer_image_data
  {
    std::allocator<T> m_allocator;

  public:

    /// \param n Number of **elements** to allocate
    __ndbuffer_image_data(std::size_t n)
    {
      this->m_size   = n;
      this->m_buffer = reinterpret_cast<std::byte*>(m_allocator.allocate(n));
      std::uninitialized_default_construct_n(reinterpret_cast<T*>(m_buffer), m_size);
    }

    __ndbuffer_image_data(std::size_t n, T val)
    {
      this->m_size   = n;
      this->m_buffer = reinterpret_cast<std::byte*>(m_allocator.allocate(n));
      std::uninitialized_fill_n(reinterpret_cast<T*>(this->m_buffer), m_size, val);
    }

    ~__ndbuffer_image_data() final
    {
      std::destroy_n(reinterpret_cast<T*>(this->m_buffer), m_size);
      m_allocator.deallocate(reinterpret_cast<T*>(this->m_buffer), m_size);
    }
  };

} // namespace mln::internal
