#pragma once

#include <cstddef>
#include <memory>

#include <ano/core/image_format.hpp>

namespace ano::io::internal
{

  class plugin_base
  {
  public:
    virtual ~plugin_base() = default;

    struct impl_t
    {
      int            m_ndim;
      int            m_dims[16];
      sample_type_id m_sample_type_id;

      virtual ~impl_t()                                     = default;
      virtual void read_next_line(std::byte* buffer)        = 0;
      virtual void write_next_line(const std::byte* buffer) = 0;
    };

  protected:
    std::unique_ptr<impl_t> m_impl;
  };


  class plugin_reader : public plugin_base
  {
  public:
    virtual void open(const char* filename) = 0;
    virtual void close()                    = 0;

    int            get_ndim() const { return m_impl->m_ndim; }
    int            get_dim(int k) const { return m_impl->m_dims[k]; }
    const int*     get_dim_array() const { return m_impl->m_dims; }
    sample_type_id get_sample_type_id() const { return m_impl->m_sample_type_id; }
    void           read_next_line(std::byte* buffer) { return m_impl->read_next_line(buffer); }
  };

  class plugin_writer : public plugin_base
  {
  public:
    virtual void open(const char* filename, sample_type_id sample_type, int ndim, const int sizes[]) = 0;
    virtual void close()                                                                             = 0;

    void write_next_line(const std::byte* buffer) { return m_impl->write_next_line(buffer); }
  };


} // namespace ano::io::internal
