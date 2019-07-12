#pragma once

#include <cstddef>
#include <memory>
#include <any>

#include <mln/core/image_format.hpp>

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
  class __ndbuffer_image_data<void> : public ndbuffer_image_data
  {
    std::allocator<std::byte> m_allocator;

  public:
    __ndbuffer_image_data(std::size_t n, sample_type_id sample_type);
    ~__ndbuffer_image_data() final;
  };

  template <class T>
  class __ndbuffer_image_data : public ndbuffer_image_data
  {
    std::allocator<T> m_allocator;

  public:
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
