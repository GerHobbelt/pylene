#ifndef MLN_IO_FREE_IMAGE_READER_HPP
# define MLN_IO_FREE_IMAGE_READER_HPP

# include <cstring>

# include <mln/core/grays.hpp>
# include <mln/core/colors.hpp>
# include <mln/core/domain/box.hpp>

//# include <mln/io/typeinfo.hpp>
# include <mln/io/reader.hpp>

# include <FreeImage.h>




/// FIXME: Handle Color correctly !


namespace mln
{
  namespace io
  {

    class freeimage_reader : public Reader
    {
    public:
      typedef box2d	domain_type;

      freeimage_reader();

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
	FreeImage_Initialise();
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
	  case FIT_INT16:		return typeid(int16);
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
      if (FreeImage_GetColorType(dib) != FIC_RGB)
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
