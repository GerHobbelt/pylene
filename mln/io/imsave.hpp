#ifndef MLN_IO_IMSAVE_HPP
# define MLN_IO_IMSAVE_HPP

# include <mln/core/trace.hpp>
# include <mln/io/saver.hpp>
# include <mln/io/freeimage_plugin.hpp>
# include <fstream>

namespace mln
{

  namespace io
  {

    template <typename I>
    void imsave(const Image<I>& ima, std::ostream& os);

    template <typename I>
    void imsave(const Image<I>& ima, const std::string& path);

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <typename I>
    void imsave(const Image<I>& ima, std::ostream& os)
    {
      mln_entering("mln::io::imsave");

      freeimage_writer_plugin plugin(os);
      Saver2D<I> saver;
      saver.save(ima, &plugin, false);

      mln_exiting();
    }


    template <typename I>
    void imsave(const Image<I>& ima, const std::string& path)
    {
      if (path == "-")
        imsave(ima, std::cout);
      else
        {
          std::ofstream os(path, std::ios::binary);
          imsave(ima, os);
          os.close();
        }
    }


  } // end of namespace mln::io
} // end of namespace mln



#endif //!MLN_IO_IMSAVE_HPP
