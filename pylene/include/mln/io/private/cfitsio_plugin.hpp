#pragma once

#include <mln/io/private/plugin.hpp>

namespace mln::io::fits::internal
{
  class cfitsio_reader_plugin final : public mln::io::internal::plugin_reader
  {
  public:
    cfitsio_reader_plugin() = delete;
    cfitsio_reader_plugin(int ind);
    ~cfitsio_reader_plugin() final;
    void open(const char* filename) final;
    void close() final;

  private:
    const int m_image_index;
  };
} // namespace mln::io::fist::internal