#ifndef MLN_IO_PLUGIN_HPP
# define MLN_IO_PLUGIN_HPP

# include <iosfwd>
# include <string>
# include <mln/io/ioexception.hpp>
# include <mln/io/internal/demangle.hpp>
# include <mln/core/domain/box.hpp>
/// \file
/// \brief File that contains base classes for Plugin Readers et Writters

namespace mln
{

  class Plugin
  {
  public:

    /// \brief Initialize the plugin.
    virtual void initialize();

    //virtual bool support_streaming() const = 0;
  };

  class PluginReader : public Plugin
  {
  public:
    virtual void open(std::istream& s) = 0;
    virtual void read_next_pixel(void* out) = 0;
    virtual std::function<void(void*)> get_read_next_pixel_method() const = 0;

    virtual std::type_index get_value_type_id() const = 0;
  };

  class PluginReader2D : public PluginReader
  {
  public:
    virtual box2d get_domain() const = 0;
    virtual void  read_next_line(void* out) = 0;
    virtual std::function<void(void*)> get_read_next_line_method() const = 0;
  };


  class PluginWriter : public Plugin
  {
  public:
    virtual void write_next_pixel(void* src) = 0;
    virtual std::function<void(void*)> get_write_next_pixel_method() const = 0;
    virtual bool can_write(std::type_index type) const = 0;
    virtual void set_value_type(std::type_index) = 0;
  };

  class PluginWriter2D : public PluginWriter
  {
  public:
    virtual void set_domain(const box2d& b) = 0;
    virtual void write_next_line(void* src) = 0;
    virtual std::function<void(void*)> get_write_next_line_method() const = 0;
  };

  /**********************************/
  /***   Implementation          ****/
  /**********************************/

  inline
  void Plugin::initialize()
  {
  }

}

#endif // ! MLN_IO_PLUGIN_HPP