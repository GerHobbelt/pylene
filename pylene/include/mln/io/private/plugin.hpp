#pragma once

#include <cstddef>
#include <memory>

#include <mln/core/image_format.hpp>

namespace mln::io::internal
{
  class plugin2d_base
  {
  public:
    virtual ~plugin2d_base() = default;

    struct impl_t
    {
      int            m_width;
      int            m_height;
      sample_type_id m_sample_type_id;

      virtual ~impl_t()                               = default;
      virtual void read_next_line(std::byte* buffer)  = 0;
      virtual void write_next_line(const std::byte* buffer) = 0;
    };

  protected:
    std::unique_ptr<impl_t> m_impl;
  };


  class plugin2d_reader : public plugin2d_base
  {
  public:
    virtual void open(const char* filename) = 0;
    virtual void close()                    = 0;

    int            width() const { return m_impl->m_width; }
    int            height() const { return m_impl->m_height; }
    sample_type_id get_sample_type_id() const { return m_impl->m_sample_type_id; }
    void           read_next_line(std::byte* buffer) { return m_impl->read_next_line(buffer); }
  };

  class plugin2d_writer : public plugin2d_base
  {
  public:
    virtual void open(const char* filename, sample_type_id sample_type, int width, int height) = 0;
    virtual void close()                                                                       = 0;

    void write_next_line(const std::byte* buffer) { return m_impl->write_next_line(buffer); }
  };


} // namespace mln::io::internal
