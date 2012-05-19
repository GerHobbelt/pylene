#ifndef MLN_IO_IMSAVE_HPP
# define MLN_IO_IMSAVE_HPP

# include <FreeImage.h>
# include <mln/io/typeinfo.hpp>
# include <mln/core/grays.hpp>
# include <boost/static_assert.hpp>


namespace mln
{

  namespace io
  {

    namespace internal
    {

      template <typename Image>
      void imsave(const Image& ima, const char* path, raw_image_tag)
      {
        BOOST_STATIC_ASSERT((Image::ndim == 2));
        typedef typename Image::value_type V;

        FreeImage_Initialise();
        FREE_IMAGE_TYPE fit;

        switch (type_info<V>::id_)
          {
          case MLN_UINT8:  fit = FIT_BITMAP; break;
          case MLN_UINT16: fit = FIT_UINT16; break;
          case MLN_INT16:  fit = FIT_INT16; break;
          case MLN_UINT32: fit = FIT_UINT32; break;
          case MLN_INT32:  fit = FIT_INT32; break;
          case MLN_FLOAT:  fit = FIT_FLOAT; break;
          case MLN_DOUBLE: fit = FIT_DOUBLE; break;
          default:
            throw MLNIOException("Unhandled image type.");
          }

        int h = ima.domain().shape()[0], w = ima.domain().shape()[1];
        FIBITMAP* dib = FreeImage_AllocateT(fit, w, h);

        const size_t* strides = ima.strides();
        const char* ptr = (const char*) &ima.at(0);
        for(int y = h-1; y >= 0; --y, ptr += strides[0]) {
          BYTE *bits = FreeImage_GetScanLine(dib, y);
          std::memcpy(bits, ptr, w * sizeof(V));
        }

        //FreeImage_DeInitialise();
        FreeImage_Save(FIF_TIFF, dib, path);
        FreeImage_Unload(dib);
      }
    }


  template <typename Image>
  void imsave(const Image& ima, const char* path)
  {
    internal::imsave(ima, path, typename image_traits<Image>::category ());
  }



  } // end of namespace mln::io
} // end of namespace mln



#endif //!MLN_IO_IMSAVE_HPP
