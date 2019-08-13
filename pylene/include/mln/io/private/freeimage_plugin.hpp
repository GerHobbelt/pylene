#pragma once

#include "plugin.hpp"

namespace mln::io::internal
{

  class freeimage_reader_plugin : public plugin2d_reader
  {
  public:
    ~freeimage_reader_plugin() final;
    void open(const char* filename) final;
    void close() final;
  };

  class freeimage_writer_plugin : public plugin2d_writer
  {
  public:
    ~freeimage_writer_plugin() final;
    void open(const char* filename, sample_type_id sample_type, int width, int height) final;
    void close() final;

  private:
    bool _close();
  };

}
