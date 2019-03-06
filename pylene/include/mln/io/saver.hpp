#pragma once

#include <mln/core/image/image.hpp>
#include <mln/io/internal/demangle.hpp>
#include <mln/io/ioexception.hpp>
#include <mln/io/plugin.hpp>


namespace mln
{
  namespace io
  {

    template <class I>
    class Saver
    {
      // BOOST_CONCEPT_ASSERT((Image<I>));

    public:
      virtual ~Saver() = default;
      void save(const Image<I>& ima, PluginWriter* plugin, bool permissive) const;

      template <typename = std::enable_if_t<mln::is_a<I, experimental::Image>{}>>
      void save(I ima, PluginWriter* plugin, bool permissive) const;

    protected:
      virtual void m_set_domain(const I& ima, PluginWriter* plugin) const = 0;
      virtual void m_save(const I& ima, PluginWriter* plugin, bool permissive) const;
    };

    template <class I, class category = typename image_traits<I>::category>
    class Saver2D : public Saver<I>
    {
      static_assert(std::is_same<typename I::domain_type, box2d>::value, "The domain must be a box2d.");

    protected:
      void m_set_domain(const I& ima, PluginWriter* plugin) const override;
    };

    template <class I>
    class Saver2D<I, raw_image_tag> : public Saver<I>
    {
      static_assert(std::is_same<typename I::domain_type, box2d>::value, "The domain must be a box2d.");

    protected:
      void m_set_domain(const I& ima, PluginWriter* plugin) const override;
      void m_save(const I& ima, PluginWriter* plugin, bool permissive) const override;
    };

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <class I>
    void Saver<I>::save(const Image<I>& ima_, PluginWriter* plugin, bool permissive) const
    {
      const I& ima = exact(ima_);

      if (not plugin->can_write(typeid(mln_value(I))))
      {
        std::string msg = "The plugin does not support writing " + internal::demangle(typeid(mln_value(I)).name());
        throw MLNIOException(msg);
      }

      plugin->set_value_type(typeid(mln_value(I)));
      this->m_set_domain(ima, plugin);
      this->m_save(ima, plugin, permissive);
    }

    template <class I>
    template <typename>
    void Saver<I>::save(I ima, PluginWriter* plugin, bool permissive) const
    {
      if (not plugin->can_write(typeid(mln_value(I))))
      {
        std::string msg = "The plugin does not support writing " + internal::demangle(typeid(mln_value(I)).name());
        throw MLNIOException(msg);
      }

      plugin->set_value_type(typeid(mln_value(I)));
      this->m_set_domain(ima, plugin);
      this->m_save(ima, plugin, permissive);
    }

    template <class I>
    void Saver<I>::m_save(const I& ima, PluginWriter* plugin, bool permissive) const
    {
      (void)permissive;
      std::function<void(void*)> write_next_pixel = plugin->get_write_next_pixel_method();

      if constexpr (is_a<I, experimental::Image>{})
      {
        for (auto v : const_cast<I*>(&ima)->new_values())
        {
          write_next_pixel((void*)&v);
        }
      }
      else
      {
        mln_foreach (mln_value(I) v, ima.values())
        {
          write_next_pixel((void*)&v);
        }
      }
    }

    template <class I, class category>
    void Saver2D<I, category>::m_set_domain(const I& ima, PluginWriter* plugin) const
    {
      PluginWriter2D* plug = static_cast<PluginWriter2D*>(plugin);
      plug->set_domain(ima.domain());
    }

    template <class I>
    void Saver2D<I, raw_image_tag>::m_set_domain(const I& ima, PluginWriter* plugin) const
    {
      PluginWriter2D* plug = static_cast<PluginWriter2D*>(plugin);
      plug->set_domain(ima.domain());
    }

    template <class I>
    void Saver2D<I, raw_image_tag>::m_save(const I& ima, PluginWriter* plugin, bool permissive) const
    {
      (void)permissive;

      PluginWriter2D*            plug       = static_cast<PluginWriter2D*>(plugin);
      std::function<void(void*)> write_line = plug->get_write_next_line_method();

      point2d p    = ima.domain().pmin;
      point2d pmax = ima.domain().pmax;

      for (; p[0] != pmax[0]; ++p[0])
      {
        void* lineptr = (void*)&(ima(p));
        write_line(lineptr);
      }
    }

  } // namespace io
} // namespace mln
