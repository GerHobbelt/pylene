#include <mln/io/private/freeimage_plugin.hpp>
#include <fmt/core.h>
#include <stdexcept>

#include <stdexcept>

#include <FreeImage.h>

namespace mln::io::internal
{

  namespace
  {

    sample_type_id get_sample_type(FREE_IMAGE_TYPE type, int bits_per_pixel)
    {
      switch (type)
      {
      case FIT_BITMAP:
        break;
      case FIT_UINT16:
        return sample_type_id::UINT16;
      case FIT_INT16:
        return sample_type_id::INT16;
      case FIT_UINT32:
        return sample_type_id::UINT32;
      case FIT_INT32:
        return sample_type_id::INT32;
      case FIT_FLOAT:
        return sample_type_id::FLOAT;
      case FIT_DOUBLE:
        return sample_type_id::DOUBLE;
      case FIT_RGB16:
      case FIT_RGBA16:
      case FIT_RGBF:
      case FIT_RGBAF:
      default:
        return sample_type_id::OTHER;
      };

      switch (bits_per_pixel)
      {
/*      case 32:
        return sample_type_id::RGBA8; */
      case 24:
        return sample_type_id::RGB8;
      case 8:
        return sample_type_id::UINT8;
      case 1:
        return sample_type_id::BOOL;
      default:
        return sample_type_id::OTHER;
      };
    }


    struct sample_type_info
    {
      FREE_IMAGE_TYPE fit;
      int             bit_per_pixels;
    };

    sample_type_info get_sample_type_info(sample_type_id sid)
    {
      switch (sid)
      {
      case sample_type_id::INT16: return { FIT_INT16, 16};
      case sample_type_id::UINT16: return { FIT_UINT16, 16};
      case sample_type_id::INT32: return { FIT_INT32, 32};
      case sample_type_id::UINT32: return { FIT_UINT32, 32};
      case sample_type_id::FLOAT: return { FIT_FLOAT, 32};
      case sample_type_id::DOUBLE: return { FIT_DOUBLE, 64};
        // Bitmaps
      case sample_type_id::BOOL: return { FIT_BITMAP, 1};
      case sample_type_id::UINT8: return { FIT_BITMAP, 8};
      case sample_type_id::RGB8: return { FIT_BITMAP, 24};
      case sample_type_id::UINT64:
      case sample_type_id::INT64:
      case sample_type_id::INT8:
      case sample_type_id::OTHER: return {FIT_UNKNOWN, 0};
      }
      return {FIT_UNKNOWN, 0};
    }




    struct impl_base_t : plugin_base::impl_t
    {
      const char*       m_filename;
      FIBITMAP*         m_dib;
      FREE_IMAGE_FORMAT m_fif;
      RGBQUAD*          m_palette;
      int               m_y;
      int               m_bpp; // Number of **bits** per pixels
    };

    struct impl_memcpy_t : impl_base_t
    {
      void read_next_line(std::byte* buffer) final
      {
        BYTE* lineptr = FreeImage_GetScanLine(m_dib, m_y);
        std::memcpy(buffer, lineptr, m_dims[0] * m_bpp / 8);
        m_y--;
      }

      void write_next_line(const std::byte* buffer) final
      {
        BYTE* lineptr = FreeImage_GetScanLine(m_dib, m_y);
        std::memcpy(lineptr, buffer, m_dims[0] * m_bpp / 8);
        m_y--;
      }
    };


    // Read bit-by-bit - 0 is black
    struct impl_bit_t : impl_base_t
    {
      void read_next_line(std::byte* __restrict buffer) final
      {
        std::uint8_t* lineptr = (std::uint8_t*) FreeImage_GetScanLine(m_dib, m_y);
        for (int x = 0, t = 0; x < m_dims[0]; x += 8, t++)
        {
          buffer[x]     = std::byte((lineptr[t] & 0x80) != 0);
          buffer[x + 1] = std::byte((lineptr[t] & 0x40) != 0);
          buffer[x + 2] = std::byte((lineptr[t] & 0x20) != 0);
          buffer[x + 3] = std::byte((lineptr[t] & 0x10) != 0);
          buffer[x + 4] = std::byte((lineptr[t] & 0x08) != 0);
          buffer[x + 5] = std::byte((lineptr[t] & 0x04) != 0);
          buffer[x + 6] = std::byte((lineptr[t] & 0x02) != 0);
          buffer[x + 7] = std::byte((lineptr[t] & 0x01) != 0);
        }
        m_y--;
      }

      void write_next_line(const std::byte* __restrict buffer_) final
      {
        const std::uint8_t* buffer  = (const std::uint8_t*)buffer_;
        std::uint8_t* lineptr = (std::uint8_t*)FreeImage_GetScanLine(m_dib, m_y);

        int x, t;
        for (x = 0, t = 0; x + 8 < m_dims[0]; x += 8, t++)
          lineptr[t] = buffer[x + 0] << 7 | buffer[x + 1] << 6 | buffer[x + 2] << 5 | buffer[x + 3] << 4 |
                       buffer[x + 4] << 3 | buffer[x + 5] << 2 | buffer[x + 6] << 1 | buffer[x + 7] << 0;

        int remainder = m_dims[0] - x;
        if (remainder > 0)
        {
          lineptr[t] = 0;
          switch (remainder)
          {
          case 7: lineptr[t] |= buffer[x+6] << 1; [[ fallthrough ]];
          case 6: lineptr[t] |= buffer[x+5] << 2; [[ fallthrough ]];
          case 5: lineptr[t] |= buffer[x+4] << 3; [[ fallthrough ]];
          case 4: lineptr[t] |= buffer[x+3] << 4; [[ fallthrough ]];
          case 3: lineptr[t] |= buffer[x+2] << 5; [[ fallthrough ]];
          case 2: lineptr[t] |= buffer[x+1] << 6; [[ fallthrough ]];
          case 1: lineptr[t] |= buffer[x+0] << 7;
          }
        }
        m_y--;
      }
    };

    // Read bit-by-bit - 0 is white
    struct impl_bit_inv_t : impl_base_t
    {
      void read_next_line(std::byte* __restrict buffer) final
      {
        std::uint8_t* lineptr = (std::uint8_t*) FreeImage_GetScanLine(m_dib, m_y);
        for (int x = 0, t = 0; x < m_dims[0]; x += 8, t++)
        {
          buffer[x]     = std::byte((lineptr[t] & 0x80) == 0);
          buffer[x + 1] = std::byte((lineptr[t] & 0x40) == 0);
          buffer[x + 2] = std::byte((lineptr[t] & 0x20) == 0);
          buffer[x + 3] = std::byte((lineptr[t] & 0x10) == 0);
          buffer[x + 4] = std::byte((lineptr[t] & 0x08) == 0);
          buffer[x + 5] = std::byte((lineptr[t] & 0x04) == 0);
          buffer[x + 6] = std::byte((lineptr[t] & 0x02) == 0);
          buffer[x + 7] = std::byte((lineptr[t] & 0x01) == 0);
        }
        m_y--;
      }

      // This method should not be called
      void write_next_line(const std::byte* /*buffer*/) final { std::abort(); }
    };


    // BITMAP with a color lookup table
    // Read a index image - 8 bits indexes
    struct impl_palette_8bit_t : impl_base_t
    {
      void read_next_line(std::byte* __restrict buffer) final
      {
        uint8_t* lineptr = (uint8_t*) FreeImage_GetScanLine(m_dib, m_y);
        for (int x = 0; x < m_dims[0]; ++x)
        {
          buffer[3 * x + 0] = std::byte(m_palette[lineptr[x]].rgbRed);
          buffer[3 * x + 1] = std::byte(m_palette[lineptr[x]].rgbGreen);
          buffer[3 * x + 2] = std::byte(m_palette[lineptr[x]].rgbBlue);
        }
        m_y--;
      }

      void write_next_line(const std::byte* /* buffer */) final { std::abort(); }
    };

    // BITMAP as RGB24
    struct impl_rgb24_t : impl_base_t
    {
      void read_next_line(std::byte* __restrict buffer) final
      {
        std::byte* lineptr = (std::byte*) FreeImage_GetScanLine(m_dib, m_y);
        for (int x = 0; x < m_dims[0]; ++x)
        {
          buffer[3 * x + 0] = lineptr[3 * x + FI_RGBA_RED];
          buffer[3 * x + 1] = lineptr[3 * x + FI_RGBA_GREEN];
          buffer[3 * x + 2] = lineptr[3 * x + FI_RGBA_BLUE];
        }
        m_y--;
      }

      void write_next_line(const std::byte* __restrict buffer) final
      {
        std::byte* lineptr = (std::byte*) FreeImage_GetScanLine(m_dib, m_y);
        for (int x = 0; x < m_dims[0]; ++x)
        {
          lineptr[3 * x + FI_RGBA_RED]   = buffer[3 * x + 0];
          lineptr[3 * x + FI_RGBA_GREEN] = buffer[3 * x + 1];
          lineptr[3 * x + FI_RGBA_BLUE]  = buffer[3 * x + 2];
        }
        m_y--;
      }
    };

    // BITMAP as RGB24
    struct impl_rgba32_t : impl_base_t
    {
      void read_next_line(std::byte* __restrict buffer) final
      {
        std::byte* lineptr = (std::byte*) FreeImage_GetScanLine(m_dib, m_y);
        for (int x = 0; x < m_dims[0]; ++x)
        {
          buffer[4 * x + 0] = lineptr[4 * x + FI_RGBA_RED];
          buffer[4 * x + 1] = lineptr[4 * x + FI_RGBA_GREEN];
          buffer[4 * x + 2] = lineptr[4 * x + FI_RGBA_BLUE];
          buffer[4 * x + 3] = lineptr[4 * x + FI_RGBA_ALPHA];
        }
        m_y--;
      }

      void write_next_line(const std::byte* __restrict buffer) final
      {
        std::byte* lineptr = (std::byte*) FreeImage_GetScanLine(m_dib, m_y);
        for (int x = 0; x < m_dims[0]; ++x)
        {
          lineptr[4 * x + FI_RGBA_RED]   = buffer[4 * x + 0];
          lineptr[4 * x + FI_RGBA_GREEN] = buffer[4 * x + 1];
          lineptr[4 * x + FI_RGBA_BLUE]  = buffer[4 * x + 2];
          lineptr[4 * x + FI_RGBA_ALPHA] = buffer[4 * x + 3];
        }
        m_y--;
      }
    };
  }

  void freeimage_reader_plugin::open(const char* filename)
  {
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename);

    if (fif == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(fif))
      throw std::runtime_error("File format not supported");

    FIBITMAP* dib = FreeImage_Load(fif, filename, 0);

    if (!dib)
      throw std::runtime_error("Unable to read the file");

    FREE_IMAGE_TYPE type           = FreeImage_GetImageType(dib);
    int             bits_per_pixel = FreeImage_GetBPP(dib);
    int             colortype      = FreeImage_GetColorType(dib);


    sample_type_id sid = get_sample_type(type, bits_per_pixel);
    std::unique_ptr<impl_base_t> impl;

    if (sid == sample_type_id::OTHER)
      goto error;


    // we simply memcpy the lines for:
    // * non-bitmap types
    // * 8-bit bitmap type with a palette MIN_IS_BLACK
    if (type != FIT_BITMAP || (bits_per_pixel == 8 && colortype == FIC_MINISBLACK))
      impl = std::make_unique<impl_memcpy_t>();
    else if (type == FIT_BITMAP && bits_per_pixel == 1 && colortype == FIC_MINISBLACK)
      impl = std::make_unique<impl_bit_t>();
    else if (type == FIT_BITMAP && bits_per_pixel == 1 && colortype == FIC_MINISWHITE)
      impl = std::make_unique<impl_bit_inv_t>();
    else if (type == FIT_BITMAP && bits_per_pixel == 8)
      impl = std::make_unique<impl_palette_8bit_t>();
    else if (type == FIT_BITMAP && bits_per_pixel == 24 && colortype == FIC_RGB)
      impl = std::make_unique<impl_rgb24_t>();
    else if (type == FIT_BITMAP && bits_per_pixel == 32 && colortype == FIC_RGBALPHA)
      impl = std::make_unique<impl_rgba32_t>();
    else
      goto error;

    impl->m_dib            = dib;
    impl->m_fif            = fif;
    impl->m_ndim           = 2;
    impl->m_dims[0]        = FreeImage_GetWidth(dib);
    impl->m_dims[1]        = FreeImage_GetHeight(dib);
    impl->m_sample_type_id = sid;
    impl->m_bpp            = bits_per_pixel;
    impl->m_palette        = FreeImage_GetPalette(dib);
    impl->m_y              = impl->m_dims[1] - 1;

    m_impl = std::move(impl);
    return;

  error:
    const char* cstr_ctype;
    switch (colortype)
    {
    case FIC_MINISBLACK:
      cstr_ctype = "FIC_MINISBLACK";
      break;
    case FIC_MINISWHITE:
      cstr_ctype = "FIC_MINISWHITE";
      break;
    case FIC_PALETTE:
      cstr_ctype = "FIC_PALETTE";
      break;
    case FIC_RGB:
      cstr_ctype = "FIC_RGB";
      break;
    case FIC_RGBALPHA:
      cstr_ctype = "FIC_RGBALPHA";
      break;
    case FIC_CMYK:
      cstr_ctype = "FIC_CMYK";
      break;
    };

    fmt::print(stderr, "Unhandled input type {} with type={} bpp={} colortype={}.", filename,
               FreeImage_GetFormatFromFIF(fif), bits_per_pixel, cstr_ctype);


    throw std::runtime_error("Unhandled input type.");
  }

  void freeimage_reader_plugin::close()
  {
    auto impl = static_cast<impl_base_t*>(m_impl.get());
    if (impl && impl->m_dib)
    {
      FreeImage_Unload(impl->m_dib);
      impl->m_dib = nullptr;
    }
  }

  freeimage_reader_plugin::~freeimage_reader_plugin()
  {
    this->close();
  }

  void freeimage_writer_plugin::open(const char* filename, sample_type_id sample_type, int ndim, const int dims[])
  {
    if (ndim != 2)
      throw std::runtime_error(fmt::format("Unsupported number of dimension (ndim={}, should be 2).", ndim));

    const int width  = dims[0];
    const int height = dims[1];

    std::unique_ptr<impl_base_t> impl;

    FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename);
    if (fif == FIF_UNKNOWN || !FreeImage_FIFSupportsWriting(fif))
      throw std::runtime_error("File format not supported for writing.");

    auto [fit, bits_per_pixel] = get_sample_type_info(sample_type);

    if (fit == FIT_UNKNOWN)
      throw std::runtime_error("Sample type not supported.");

    switch (sample_type)
    {
    case sample_type_id::BOOL:
      impl = std::make_unique<impl_bit_t>();
      break;
    case sample_type_id::UINT8:
    case sample_type_id::UINT16:
    case sample_type_id::UINT32:
    case sample_type_id::INT16:
    case sample_type_id::INT32:
    case sample_type_id::FLOAT:
    case sample_type_id::DOUBLE:
      impl = std::make_unique<impl_memcpy_t>();
      break;
    case sample_type_id::RGB8:
      impl = std::make_unique<impl_rgb24_t>();
      break;
    case sample_type_id::INT8:
    case sample_type_id::UINT64:
    case sample_type_id::INT64:
    case sample_type_id::OTHER:
      std::abort();
    }


    unsigned red_mask   = FI_RGBA_RED_MASK;
    unsigned green_mask = FI_RGBA_GREEN_MASK;
    unsigned blue_mask  = FI_RGBA_BLUE_MASK;

    FIBITMAP* dib;
    if (fit == FIT_BITMAP)
      dib = FreeImage_AllocateT(fit, width, height, bits_per_pixel, red_mask, green_mask, blue_mask);
    else
      dib = FreeImage_AllocateT(fit, width, height);

    if (!dib)
      throw std::runtime_error(
        fmt::format("Unable to allocate image (type={}, bpp={}).", FreeImage_GetFormatFromFIF(fif), bits_per_pixel));

    // Set the palette
    RGBQUAD* palette = FreeImage_GetPalette(dib);
    if (sample_type == sample_type_id::BOOL)
    {
      palette[0] = {0u, 0u, 0u, 0u};
      palette[1] = {255u, 255u, 255u, 0u};
    }
    else if (sample_type == sample_type_id::UINT8)
    {
      for (unsigned i = 0; i < 256; ++i)
        palette[i] = {(BYTE)i, (BYTE)i, (BYTE)i, 0u};
    }

    impl->m_ndim           = 2;
    impl->m_dims[0]        = width;
    impl->m_dims[1]        = height;
    impl->m_sample_type_id = sample_type;
    impl->m_bpp            = bits_per_pixel;
    impl->m_dib            = dib;
    impl->m_fif            = fif;
    impl->m_palette        = palette;
    impl->m_y              = height - 1;
    impl->m_filename       = filename;
    m_impl                 = std::move(impl);
  }


  bool freeimage_writer_plugin::_close()
  {
    auto impl = static_cast<impl_base_t*>(m_impl.get());
    if (impl && impl->m_dib)
    {
      auto res = FreeImage_Save(impl->m_fif, impl->m_dib, impl->m_filename, 0);
      FreeImage_Unload(impl->m_dib);
      impl->m_dib = nullptr;
      return res;
    }
    return true;
  }
  void freeimage_writer_plugin::close()
  {
    if (!this->_close())
      throw std::runtime_error("Unable to save the image.");
  }

  freeimage_writer_plugin::~freeimage_writer_plugin()
  {
    this->_close();
  }
}
