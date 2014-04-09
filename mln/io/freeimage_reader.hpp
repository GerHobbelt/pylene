#ifndef MLN_IO_FREE_IMAGE_READER_HPP
# define MLN_IO_FREE_IMAGE_READER_HPP

# include <cstring>

# include <mln/core/grays.hpp>
# include <mln/core/colors.hpp>
# include <mln/core/domain/box.hpp>

//# include <mln/io/typeinfo.hpp>
# include <mln/io/reader.hpp>

# include <iostream>
# include <FreeImage.h>




/// FIXME: Handle Color correctly !


namespace mln
{
  namespace io
  {

    namespace internal
    {

      struct istream_wrapper
      {
        istream_wrapper(std::istream& is)
          : m_original_stream(is)
        {
          std::istream::pos_type pos = is.tellg();
          m_support_streaming = (pos != std::istream::pos_type(-1));

          if (m_support_streaming) {
            m_cs = &is;
            m_offset = pos;
          } else {
            m_cs = new std::stringstream(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
            is >> m_cs->rdbuf();
            m_offset = 0;
          }
        }

        ~istream_wrapper()
        {
          if (not m_support_streaming)
            delete m_cs;
        }

        static
        unsigned read(void* buffer, unsigned size, unsigned count, fi_handle isw_)
        {
          istream_wrapper* isw = (istream_wrapper*) isw_;
          isw->m_cs->read((char*)buffer, size * count);
          return (isw->m_cs) ? count : 0;
        }

        static
        int seek(fi_handle isw_, long offset, int origin)
        {
          istream_wrapper* isw = (istream_wrapper*) isw_;
          if (origin == SEEK_SET) offset += isw->m_offset;
          isw->m_cs->seekg(offset, (std::ios_base::seekdir) origin);
          return (isw->m_cs) ? 0 : -1;
        }

        static
        long tell(fi_handle isw_)
        {
          istream_wrapper* isw = (istream_wrapper*) isw_;
          return isw->m_cs->tellg() - isw->m_offset;
        }

      private:
        std::istream&                   m_original_stream;
        std::istream*                   m_cs;
        std::istream::off_type          m_offset;
        bool                            m_support_streaming;
      };

    }


    class freeimage_reader : public Reader
    {
    public:
      typedef box2d	domain_type;

      freeimage_reader();

      virtual void load(std::istream& s);
      virtual void load(const char* filename);
      virtual void close();
      virtual void read_next_pixel(void* out);
      virtual void read_next_line(void* out);
      virtual int get_ndim() const;
      virtual std::type_index get_value_type_id() const;
      const domain_type& get_domain() const;


      virtual void initialize();

    private:
      static bool init_;
      domain_type domain;
      FIBITMAP* dib;
      char* ptr;
      unsigned pitch;
      int x, y;
      int bpp;
    };

    bool freeimage_reader::init_ = false;


    inline
    freeimage_reader::freeimage_reader()
      : dib (NULL), x (0), y (0)
    {
    }

    inline
    void
    freeimage_reader::initialize()
    {
      if (!init_){
        init_ = true;
        //FreeImage_SetOutputMessage(freeimage_reader::printErr);
        //FreeImage_Initialise();
      }
    }

    inline
    void
    freeimage_reader::load(const char* filename)
    {
      mln_precondition(init_);
      mln_precondition(dib == NULL);

      FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
      fif = FreeImage_GetFileType(filename, 0);
      if (fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(filename);

      if (fif == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(fif))
        throw MLNIOException("File format not supported");

      dib = FreeImage_Load(fif, filename, 0);
      domain.pmin[0] = 0;
      domain.pmin[1] = 0;
      domain.pmax[0] = FreeImage_GetHeight(dib);
      domain.pmax[1] = FreeImage_GetWidth(dib);
      pitch = FreeImage_GetPitch(dib);
      ptr = (char*)FreeImage_GetBits(dib) + pitch * (domain.pmax[0]-1);
      bpp = FreeImage_GetBPP(dib) / 8;
      mln_postcondition(dib != NULL);
    }

    inline
    void
    freeimage_reader::load(std::istream& is)
    {
      mln_precondition(init_);
      mln_precondition(dib == NULL);


      internal::istream_wrapper isw(is);
      fi_handle handle = (fi_handle) &isw;

      FreeImageIO       fio = {
        internal::istream_wrapper::read,
        NULL,
        internal::istream_wrapper::seek,
        internal::istream_wrapper::tell
      };


      FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
      fif = FreeImage_GetFileTypeFromHandle(&fio, handle);

      if (fif == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(fif))
        throw MLNIOException("File format not supported");

      dib = FreeImage_LoadFromHandle(fif, &fio, handle, 0);

      domain.pmin[0] = 0;
      domain.pmin[1] = 0;
      domain.pmax[0] = FreeImage_GetHeight(dib);
      domain.pmax[1] = FreeImage_GetWidth(dib);
      pitch = FreeImage_GetPitch(dib);
      ptr = (char*)FreeImage_GetBits(dib) + pitch * (domain.pmax[0]-1);
      bpp = FreeImage_GetBPP(dib) / 8;
      mln_postcondition(dib != NULL);
    }


    inline
    int freeimage_reader::get_ndim() const
    {
      return 2;
    }

    inline
    const typename freeimage_reader::domain_type&
    freeimage_reader::get_domain() const
    {
      return domain;
    }

    inline
    std::type_index
    freeimage_reader::get_value_type_id() const
    {
      mln_precondition(init_);
      mln_precondition(dib != NULL);
      FREE_IMAGE_TYPE type = FreeImage_GetImageType(dib);
      int bppp = FreeImage_GetBPP(dib);
      int colortype = FreeImage_GetColorType(dib);


      switch (type)
        {
          case FIT_BITMAP:	break;
          case FIT_UINT16:	return typeid(uint16);
          case FIT_INT16:	return typeid(int16);
          case FIT_UINT32:  return typeid(uint32);
          case FIT_INT32:   return typeid(int32);
          case FIT_FLOAT:	return typeid(float);
          case FIT_DOUBLE:  return typeid(double);
          default:		goto error;
        }

      switch (bppp)
        {
          case 1:  return typeid(bool);
          case 8:  return typeid(uint8);
          case 16: return typeid(uint16);
          case 24: switch (colortype)
            {
              case FIC_RGB: return typeid(rgb8);
              default: goto error;
            }
        }

    error: return typeid(void);
    }

    inline
    void freeimage_reader::read_next_line(void* out_)
    {
      char* out = (char*)out_;
      mln_precondition(init_);
      mln_precondition(dib != NULL);
      mln_precondition(x < (int) FreeImage_GetHeight(dib));
      int bppp = FreeImage_GetBPP(dib);

      if (bppp == 1) // Case of bool images
	{
	  for (int y = 0, z = 0; y < domain.pmax[1]; y += 8, z += 1)
	    for (int b = 0; b < 8; ++b)
	      out[y+b] = ((ptr[z] & (0x80 >> b)) != 0);
	}
      else if (FreeImage_GetColorType(dib) != FIC_RGB)
	std::memcpy(out, ptr, domain.pmax[1] * bpp);
      else
	for (int y = 0; y < domain.pmax[1]; ++y) {
	  *(out + y * bpp + 0) = *(ptr + y * bpp + 2);
	  *(out + y * bpp + 1) = *(ptr + y * bpp + 1);
	  *(out + y * bpp + 2) = *(ptr + y * bpp + 0);
	}
      ++x;
      ptr -= pitch;
    }

    inline
    void freeimage_reader::read_next_pixel(void* out)
    {
      mln_assertion(false);
      (void)out;
    }

    inline
    void freeimage_reader::close()
    {
      mln_precondition(init_);
      mln_precondition(dib != NULL);
      FreeImage_Unload(dib);
      dib = NULL;
    }

  }
}



#endif /* !MLN_IO_FREE_IMAGE_READER_HPP */
