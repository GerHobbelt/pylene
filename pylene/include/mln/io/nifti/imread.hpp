#pragma once

#include <mln/core/trace.hpp>
#include <mln/io/loader.hpp>
#include <mln/io/nifti/nifti_plugin.hpp>


namespace mln
{

  namespace io
  {

    namespace nifti
    {

      template <typename I>
      void imread(const std::string& path, Image<I>& out, bool permissive = false);

      /******************************************/
      /****          Implementation          ****/
      /******************************************/

      template <typename I>
      void imread(const std::string& path, Image<I>& out, bool permissive)
      {
        mln_entering("mln::io::nifti::imread");
        nifti_reader_plugin plugin;
        Loader2D<I>         loader;
        loader.load(path, out, &plugin, permissive);
        mln_exiting();
      }
    } // namespace nifti
  }   // namespace io
} // namespace mln
