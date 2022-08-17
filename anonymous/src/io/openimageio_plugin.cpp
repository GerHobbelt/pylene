#include <ano/core/image/ndbuffer_image.hpp>
#include <ano/io/oiio/imread.hpp>
#include <ano/io/private/io.hpp>
#include <ano/io/private/openimageio_plugin.hpp>

#include <OpenImageIO/imageio.h>
#include <fmt/format.h>

#include <stdexcept>

namespace ano::io::oiio
{
  namespace internal
  {
    namespace
    {
      sample_type_id get_sample_type(const OIIO::TypeDesc& desc)
      {
        using namespace OIIO;
        switch (desc.basetype)
        {
        case TypeDesc::UINT8:
          return sample_type_id::UINT8;
        case TypeDesc::UINT16:
          return sample_type_id::UINT16;
        case TypeDesc::UINT32:
          return sample_type_id::UINT32;
        case TypeDesc::UINT64:
          return sample_type_id::UINT64;
        case TypeDesc::INT8:
          return sample_type_id::INT8;
        case TypeDesc::INT16:
          return sample_type_id::INT16;
        case TypeDesc::INT32:
          return sample_type_id::INT32;
        case TypeDesc::INT64:
          return sample_type_id::INT64;
        case TypeDesc::FLOAT:
          return sample_type_id::FLOAT;
        case TypeDesc::DOUBLE:
          return sample_type_id::DOUBLE;
        default:
          return sample_type_id::OTHER;
        }
      }

      struct desc_info
      {
        OIIO::TypeDesc descr;
        int            bpp;
        int            nchan = 1;
      };

      desc_info get_descriptor(sample_type_id st)
      {
        using namespace OIIO;
        switch (st)
        {
        case sample_type_id::UINT8:
          return {TypeDesc::UINT8, 8};
        case sample_type_id::UINT16:
          return {TypeDesc::UINT16, 16};
        case sample_type_id::UINT32:
          return {TypeDesc::UINT32, 32};
        case sample_type_id::UINT64:
          return {TypeDesc::UINT64, 64};
        case sample_type_id::INT8:
          return {TypeDesc::INT8, 8};
        case sample_type_id::INT16:
          return {TypeDesc::INT16, 16};
        case sample_type_id::INT32:
          return {TypeDesc::INT32, 32};
        case sample_type_id::INT64:
          return {TypeDesc::INT64, 64};
        case sample_type_id::FLOAT:
          return {TypeDesc::FLOAT, 32};
        case sample_type_id::DOUBLE:
          return {TypeDesc::DOUBLE, 64};
        case sample_type_id::BOOL: // This should be ignore in case of PBM
          return {TypeDesc::UINT8, 1};
        case sample_type_id::RGB8:
          return {TypeDesc::UINT8, 8, 3};
        default:
          throw std::invalid_argument("Unhandled input type");
        }
      }

      bool is_rgb8(int nchannels, auto desc)
      {
        return nchannels == 3 && desc == OIIO::TypeDesc::UINT8;
      }
    } // namespace

    // Base Implementation
    struct impl_base_t : public ano::io::internal::plugin_base::impl_t
    {
      const char*                        m_filename;
      std::unique_ptr<OIIO::ImageInput>  m_inp;
      std::unique_ptr<OIIO::ImageOutput> m_out;
      OIIO::TypeDesc                     m_desc;
      int                                m_y = 0;
    };

    // Memcpy based implementation
    struct impl_memcpy_t : public impl_base_t
    {
      void read_next_line(std::byte* buffer) final
      {
        assert(m_inp);
        m_inp->read_scanline(m_y, 0, m_desc, buffer);
        m_y++;
      }

      void write_next_line(const std::byte* buffer) final
      {
        assert(m_out);
        m_out->write_scanline(m_y, 0, m_desc, buffer);
        m_y++;
      }
    };

    struct impl_bit_t : public impl_base_t
    {
      impl_bit_t(int scan_size)
        : m_scanline(std::make_unique<std::uint8_t[]>(scan_size))
      {
      }

      void read_next_line(std::byte* buffer)
      {
        assert(m_inp);
        m_inp->read_scanline(m_y, 0, m_desc, buffer);
        std::uint8_t* buf = (std::uint8_t*)buffer;
        for (int x = 0; x < m_dims[0]; x++)
          buf[x] = buf[x] ? 1 : 0;
        m_y++;
      }

      void write_next_line(const std::byte* buffer)
      {
        assert(m_out);
        for (int x = 0; x < m_dims[0]; x++)
          m_scanline.get()[x] = ((bool*)buffer[x]) ? -1 : 0;
        m_out->write_scanline(m_y, 0, OIIO::TypeDesc::UINT8, m_scanline.get());
        m_y++;
      }

    private:
      std::unique_ptr<std::uint8_t[]> m_scanline;
    };

    // Used for reading only
    struct inv_bit_t : public impl_base_t
    {
      void read_next_line(std::byte* buffer_)
      {
        assert(m_inp);
        m_inp->read_scanline(m_y, 0, m_desc, buffer_);
        bool* buffer = reinterpret_cast<bool*>(buffer_);
        for (int x = 0; x < m_dims[0]; x++)
          buffer[x] = !buffer[x];
        m_y++;
      }

      void write_next_line(const std::byte*) { std::abort(); }
    };

    openimageio_reader_plugin::~openimageio_reader_plugin()
    {
      this->close();
    }

    void openimageio_reader_plugin::open(const char* filename)
    {
      using namespace OIIO;
      auto inp = ImageInput::open(filename);
      if (!inp)
        throw std::runtime_error(fmt::format("Could not open {} ({})", filename, geterror()));
      const auto& spec   = inp->spec();
      const auto& format = spec.format;

      // Number of dimension
      int ndim = 2;
      if (spec.depth > 1)
        // ndim++;
        throw std::runtime_error("Only 2D images are handled by the OIIO plugin reader");

      // Channels handling
      auto desc      = static_cast<TypeDesc::BASETYPE>(format.basetype);
      auto st        = get_sample_type(desc);
      int  nchannels = spec.nchannels;
      int  bpp       = -1;
      {
        // Handle the conversion between float and int for bpp, which is not handled by oiio
        auto bps = spec["oiio:BitsPerSample"];
        if (bps.type().basetype == TypeFloat.basetype)
          bpp = static_cast<int>(bps.get<float>(-1));
        else
          bpp = bps.get<int>(-1);
        // If no bpp info, maybe a BMP
        if (bpp < 0)
          bpp = spec.get_int_attribute("bmp:bitsperpixel", -1);
        // Else, we rely only on the OIIO TypeDesc
      }

      if (is_rgb8(nchannels, desc))
        st = sample_type_id::RGB8;
      else if (nchannels != 1)
        throw std::invalid_argument(fmt::format("Invalid value format. Only handle univariate or RGB (24 bits) images "
                                                "(Number of channels: {}, Value type: {})",
                                                nchannels, format.c_str()));

      if (bpp == 1)
        st = sample_type_id::BOOL;

      std::unique_ptr<impl_base_t> impl;
      if (bpp == 1 && std::string_view(inp->format_name()) == "pnm")
        impl = std::make_unique<inv_bit_t>();
      else if (bpp == 1)
        impl = std::make_unique<impl_bit_t>(0);
      else
        impl = std::make_unique<impl_memcpy_t>();

      impl->m_filename       = filename;
      impl->m_desc           = desc;
      impl->m_sample_type_id = st;
      impl->m_ndim           = ndim;
      impl->m_dims[0]        = spec.width;
      impl->m_dims[1]        = spec.height;
      impl->m_dims[2]        = spec.depth;
      impl->m_inp            = std::move(inp);

      m_impl = std::move(impl);
    }

    void openimageio_reader_plugin::close()
    {
      auto impl = static_cast<impl_base_t*>(m_impl.get());
      if (impl && impl->m_inp)
      {
        impl->m_inp->close();
        impl->m_inp = nullptr;
      }
    }

    openimageio_writer_plugin::~openimageio_writer_plugin()
    {
      this->close();
    }

    void openimageio_writer_plugin::open(const char* filename, sample_type_id sample_type, int ndim, const int dims[])
    {
      using namespace OIIO;
      if (ndim != 2)
        throw std::invalid_argument(
            fmt::format("Invalid number of dimensions for writing (Got {}, expected: 2)", ndim));
      auto out = ImageOutput::create(filename);
      if (!out)
        throw std::runtime_error(fmt::format("Unable to open output {} ({})", filename, geterror()));

      // Implementation (Only for memcpy for now)
      auto [descr, bpp, nchan] = get_descriptor(sample_type);
      std::unique_ptr<impl_base_t> impl;
      if (bpp == 1)
        impl = std::make_unique<impl_bit_t>(dims[0]);
      else
        impl = std::make_unique<impl_memcpy_t>();

      impl->m_ndim           = ndim;
      impl->m_dims[0]        = dims[0];
      impl->m_dims[1]        = dims[1];
      impl->m_sample_type_id = sample_type;
      if (ndim > 2)
        impl->m_dims[2] = dims[2];
      impl->m_desc = descr;
      ImageSpec spec{};
      if (bpp > 0)
        spec.attribute("oiio:BitsPerSample", bpp);
      spec.width     = dims[0];
      spec.height    = dims[1];
      spec.depth     = 1; // ndim > 2 ? dims[2] : 1;
      spec.nchannels = nchan;
      spec.format    = descr;
      out->open(filename, spec);
      impl->m_out = std::move(out);

      m_impl = std::move(impl);
    }

    void openimageio_writer_plugin::close()
    {
      auto impl = static_cast<impl_base_t*>(m_impl.get());
      if (impl && impl->m_out)
      {
        impl->m_out->close();
        impl->m_out = nullptr;
      }
    }
  } // namespace internal

  ano::ndbuffer_image imread(const std::string& filename)
  {
    ano::ndbuffer_image out;
    imread(filename, out);
    return out;
  }

  void imread(const std::string& filename, ano::ndbuffer_image& out)
  {
    internal::openimageio_reader_plugin p;
    ano::io::internal::load(&p, filename.c_str(), out);
  }

} // namespace ano::io::oiio