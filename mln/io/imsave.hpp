#ifndef MLN_IO_IMSAVE_HPP
# define MLN_IO_IMSAVE_HPP

# include <iosfwd>
# include <fstream>
# include <FreeImage.h>
# include <mln/core/grays.hpp>
# include <mln/core/colors.hpp>
# include <mln/core/trace.hpp>
# include <mln/io/ioexception.hpp>

namespace mln
{

  namespace io
  {

    namespace internal
    {

      struct ostream_wrapper
      {
        ostream_wrapper(std::ostream& os)
          : m_original_stream(os)
        {
          std::ostream::pos_type pos = os.tellp();
          m_support_streaming = (pos != std::ostream::pos_type(-1));

          if (m_support_streaming) {
            m_cs = &os;
            m_offset = pos;
          } else {
            m_cs = new std::stringstream(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
            m_offset = 0;
          }
        }

        ~ostream_wrapper()
        {
          if (not m_support_streaming)
            {
              m_cs->rdbuf()->pubseekpos(0);
              m_original_stream << m_cs->rdbuf();
              delete m_cs;
            }
        }

        static
        unsigned write(void* buffer, unsigned size, unsigned count, fi_handle osw_)
        {
          ostream_wrapper* osw = (ostream_wrapper*) osw_;
          osw->m_cs->write((char*)buffer, size * count);
          return (osw->m_cs) ? count : 0;
        }

        static
        int seek(fi_handle osw_, long offset, int origin)
        {
          ostream_wrapper* osw = (ostream_wrapper*) osw_;
          if (origin == SEEK_SET) offset += osw->m_offset;
          osw->m_cs->seekp(offset, (std::ios_base::seekdir) origin);
          return (osw->m_cs) ? 0 : -1;
        }

        static
        long tell(fi_handle osw_)
        {
          ostream_wrapper* osw = (ostream_wrapper*) osw_;
          return osw->m_cs->tellp() - osw->m_offset;
        }


      private:
        std::ostream&                   m_original_stream;
        std::ostream*                   m_cs;
        std::ostream::off_type          m_offset;
        bool                            m_support_streaming;
      };

      template <typename Image>
      void imsave(const Image& ima, std::ostream& os, raw_image_tag)
      {
        static_assert(Image::ndim == 2, "This plugin does not support images with dimension > 2");
        typedef mln_value(Image) V;

        FreeImage_Initialise();
        FREE_IMAGE_TYPE fit;

        if (std::is_same<V, bool>::value or std::is_same<V, uint8>::value or std::is_same<V, rgb8>::value)
          fit = FIT_BITMAP;
        else if (std::is_same<V, rgb<float> >::value)
          fit = FIT_RGBF;
        else if (std::is_same<V, uint16>::value)
          fit = FIT_UINT16;
        else if (std::is_same<V, int16>::value)
          fit = FIT_INT16;
        else if (std::is_same<V, uint32>::value)
          fit = FIT_UINT32;
        else if (std::is_same<V, int32>::value)
          fit = FIT_INT32;
        else if (std::is_same<V, float>::value)
          fit = FIT_FLOAT;
        else if (std::is_same<V, double>::value)
          fit = FIT_DOUBLE;
        else
          throw MLNIOException("Unhandled image type.");


        int h = ima.domain().shape()[0], w = ima.domain().shape()[1];
        FIBITMAP* dib;
        if (std::is_same<V, rgb8>::value)
          dib = FreeImage_AllocateT(fit, w, h, sizeof(V) * 8, 0xFF000000, 0x00FF00, 0xFF000000);
        else if (std::is_same<V, bool>::value) {
          dib = FreeImage_AllocateT(fit, w, h, 1);
          RGBQUAD *pal = FreeImage_GetPalette(dib);
          mln_assertion(pal != NULL);
          pal[1].rgbRed = 0xFF;
          pal[1].rgbGreen = 0XFF;
          pal[1].rgbBlue = 0xFF;
          mln_assertion(FreeImage_GetColorType(dib) == FIC_MINISBLACK);
        } else
          dib = FreeImage_AllocateT(fit, w, h, sizeof(V) * 8);


        const size_t* strides = ima.strides();
        const char* ptr = (const char*) &ima(ima.domain().pmin);
        unsigned bpp = sizeof(V);
        for(int y = h-1; y >= 0; --y, ptr += strides[0]) {
          BYTE *bits = FreeImage_GetScanLine(dib, y);
          if (std::is_same<V, rgb8>::value or std::is_same<V, rgb<float> >::value)
            {
              for (int j = 0; j < w; ++j) {
                *(bits + j * bpp + 0) = *(ptr + j * bpp + 2);
                *(bits + j * bpp + 1) = *(ptr + j * bpp + 1);
                *(bits + j * bpp + 2) = *(ptr + j * bpp + 0);
              }
            }
          else if (std::is_same<V, bool>::value)
            {
              for (int j = 0, z = 0; j < w; ++z)
                for (int b = 7; b >= 0; --b, ++j)
                  bits[z] |= ptr[j] << b;
            }
          else
            std::memcpy(bits, ptr, w * sizeof(V));
        }

        //FreeImage_DeInitialise();
        FreeImageIO fio = {
          NULL,
          ostream_wrapper::write,
          ostream_wrapper::seek,
          ostream_wrapper::tell
        };


        ostream_wrapper osw(os);
        fi_handle handle = (fi_handle) (&osw);
        bool res = FreeImage_SaveToHandle(FIF_TIFF, dib, &fio, handle, 0);
        if (!res)
          throw MLNIOException("Unable to save the image.");

        FreeImage_Unload(dib);
      }
    }

    template <typename Image>
    void imsave(const Image& ima, std::ostream& os)
    {
      mln_entering("mln::io::imsave")

      internal::imsave(ima, os, typename image_traits<Image>::category ());
      os.flush();

      mln_exiting();
    }


    template <typename Image>
    void imsave(const Image& ima, const char* path)
    {
      if (std::strcmp(path, "-") == 0)
        imsave(ima, std::cout);
      else
        {
          std::ofstream os(path, std::ios_base::binary);
          imsave(ima, os);
          os.close();
        }
    }


  } // end of namespace mln::io
} // end of namespace mln



#endif //!MLN_IO_IMSAVE_HPP
