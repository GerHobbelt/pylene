#pragma once

#include "plugin.hpp"

namespace ano::io::internal
{

  class freeimage_reader_plugin final : public plugin_reader
  {
  public:
    ~freeimage_reader_plugin() final;
    void open(const char* filename) final;
    void close() final;
  };

  class freeimage_writer_plugin final : public plugin_writer
  {
  public:
    ~freeimage_writer_plugin() final;
    void open(const char* filename, sample_type_id sample_type, int nfim, const int dims[]) final;
    void close() final;

  private:
    bool _close();
  };

} // namespace ano::io::internal
