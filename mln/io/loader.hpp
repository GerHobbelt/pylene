#ifndef MLN_IO_LOADER_HPP
# define MLN_IO_LOADER_HPP

# include <mln/core/image/image.hpp>
# include <mln/io/plugin.hpp>
# include <mln/io/ioexception.hpp>
# include <mln/io/internal/demangle.hpp>

namespace mln
{

  namespace io
  {

    template <class I>
    class Loader
    {
      BOOST_CONCEPT_ASSERT((Image<I>));

    public:
      void load(std::istream& s, Image<I>& ima, PluginReader* plugin,
                bool permissive);
      void load(const std::string& filename, Image<I>& ima, PluginReader* plugin,
                bool permissive);


    protected:
      void m_check_value_type_compatible() const;
      virtual bool is_value_type_convertible(std::type_index x,
                                             std::type_index y) const;
      virtual void m_resize() = 0;
      virtual void m_load();

      PluginReader* m_plugin;
      I*            m_ima;
      bool          m_permissive;
    };


    // Generic 2D loader
    template <class I, class category = typename image_traits<I>::category>
    class Loader2D : public Loader<I>
    {
      static_assert(std::is_same<typename I::domain_type, box2d>::value,
                    "The domain must be a box 2D.");

      virtual void m_resize();
    };

    // Specialization for fast images
    template <class I>
    class Loader2D<I, raw_image_tag> : public Loader<I>
    {
      static_assert(std::is_same<typename I::domain_type, box2d>::value,
                    "The domain must be a box 2D.");

      virtual void m_resize();
      virtual void m_load();
    };


    /***************************/
    /***  Implementation    ****/
    /***************************/

    template <class I>
    void
    Loader<I>::load(std::istream& s, Image<I>& ima, PluginReader* plugin,
                    bool permissive)
    {
      m_plugin = plugin;
      m_ima = & (exact(ima));
      m_permissive = permissive;

      m_plugin->initialize();
      m_plugin->open(s);

      this->m_check_value_type_compatible();
      this->m_resize();
      this->m_load();
    }

    template <class I>
    void
    Loader<I>::load(const std::string& s, Image<I>& ima, PluginReader* plugin,
                    bool permissive)
    {
      if (s == "-")
        {
          this->load(std::cin, ima, plugin, permissive);
        }
      else
        {
          std::ifstream stream(s, std::ios_base::binary);
          this->load(stream, ima, plugin, permissive);
        }
    }

    template <class I>
    inline
    bool
    Loader<I>::is_value_type_convertible(std::type_index sidx,
                                         std::type_index tidx) const
    {
      return sidx == tidx;
    }


    template <class I>
    void
    Loader<I>::m_check_value_type_compatible() const
    {
      typedef mln_value(I) V;
      std::type_index sidx = m_plugin->get_value_type_id();
      std::type_index tidx = typeid(V);

      if (sidx != tidx or
          (m_permissive and not is_value_type_convertible(sidx, tidx)))
        {
          std::string ex = "Value types incompatibles: ";
          (ex += "trying to load ") += internal::demangle(sidx.name());
          (ex += " in an image of ") += internal::demangle(tidx.name());
          throw MLNIOException(ex);
        }
    }

    template <class I>
    void
    Loader<I>::m_load()
    {
      std::type_index sidx = m_plugin->get_value_type_id();
      std::type_index tidx = typeid(mln_value(I));

      if (sidx == tidx)
        {
          std::function<void (void*)> read_next_pixel =
            m_plugin->get_read_next_pixel_method();

          mln_value(I) v;
          mln_pixter(px, *m_ima);
          mln_forall(px)
          {
            read_next_pixel((void*)&v);
            px->val() = v;
          }
        }
      else
        {
          std::string ex = "Value types incompatibles: ";
          (ex += "trying to load ") += internal::demangle(sidx.name());
          (ex += " in an image of ") += internal::demangle(tidx.name());
          throw MLNIOException(ex);
        }
    }


    template <class I, class category>
    void
    Loader2D<I, category>::m_resize()
    {
      PluginReader2D* plug = reinterpret_cast<PluginReader2D*>(this->m_plugin);
      box2d dom = plug->get_domain();
      this->m_ima->resize(dom);
    }

    template <class I>
    void
    Loader2D<I, raw_image_tag>::m_resize()
    {
      PluginReader2D* plug = reinterpret_cast<PluginReader2D*>(this->m_plugin);
      box2d dom = plug->get_domain();
      this->m_ima->resize(dom, this->m_ima->border());
    }

    template <class I>
    void
    Loader2D<I, raw_image_tag>::m_load()
    {
      PluginReader2D* plug = reinterpret_cast<PluginReader2D*>(this->m_plugin);
      box2d dom = this->m_ima->domain();
      point2d p = dom.pmin;
      point2d q = dom.pmax;

      std::function<void (void*)> read_next_line =
        plug->get_read_next_line_method();

      for (; p[0] != q[0]; ++p[0])
        {
          void* lineptr = &(*this->m_ima)(p);
          read_next_line(lineptr);
        }
    }

  }

}

#endif // ! MLN_IO_LOADER_HPP
