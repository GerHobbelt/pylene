#pragma once

#include "plugin.hpp"

namespace mln::io::oiio::internal
{
  class openimageio_reader_plugin final : public mln::io::internal::plugin_reader
  {
  public:
    ~openimageio_reader_plugin() final;
    void open(const char* filename) final;
    void close() final;
  };

  class openimageio_writer_plugin final : public mln::io::internal::plugin_writer
  {
  public:
    ~openimageio_writer_plugin() final;
    void open(const char* filename, sample_type_id sample_type, int ndim, const int dims[]) final;
    void close() final;
  };
} // namespace mln::io::oiio::internal