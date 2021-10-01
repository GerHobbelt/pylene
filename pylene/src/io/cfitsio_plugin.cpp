#include <mln/core/image/ndimage.hpp>

#include <mln/io/fits/imread.hpp>
#include <mln/io/private/cfitsio_plugin.hpp>
#include <mln/io/private/io.hpp>

#include <fitsio.h>
#include <fmt/format.h>

#include <stdexcept>
#include <tuple>

namespace mln::io::fits
{
  namespace internal
  {
    namespace
    {
      std::pair<mln::sample_type_id, int> get_type_info(int type)
      {
        switch (type)
        {
        case BYTE_IMG:
          return {mln::sample_type_id::UINT8, TBYTE};
        case SHORT_IMG:
          return {mln::sample_type_id::INT16, TSHORT};
        case LONG_IMG:
          return {mln::sample_type_id::INT32, TINT};
        case LONGLONG_IMG:
          return {mln::sample_type_id::INT64, TLONGLONG};
        case FLOAT_IMG:
          return {mln::sample_type_id::FLOAT, TFLOAT};
        case DOUBLE_IMG:
          return {mln::sample_type_id::DOUBLE, TDOUBLE};
          break;
        default:
          throw std::runtime_error("Unhandled data type");
        }
        return {};
      }

      struct impl_cfitsio_t : mln::io::internal::plugin_base::impl_t
      {
        fitsfile* file = nullptr;
        int       datatype;

        void read_next_line(std::byte* __restrict buffer) final
        {
          int anynul;
          int nullpix = 0;
          int status = 0;
          fits_read_img(file, datatype, m_line * m_dims[0] + 1, m_dims[0], &nullpix, buffer, &anynul, &status);
          if (status)
          {
            char msg[80];
            fits_get_errstatus(status, msg);
            throw std::runtime_error(fmt::format("Unable to read the image ({})", msg));
          }
          m_line++;
        }

        void write_next_line(const std::byte* /* buffer */) final { std::abort(); }

      private:
        int m_line = 0;
      };
    } // namespace

    cfitsio_reader_plugin::cfitsio_reader_plugin(int ind)
      : m_image_index(ind)
    {
    }

    cfitsio_reader_plugin::~cfitsio_reader_plugin() { this->close(); }

    void cfitsio_reader_plugin::open(const char* filename)
    {
      int status = 0;

      // Open the file
      fitsfile* file;
      fits_open_file(&file, filename, READONLY, &status);
      if (status)
        throw std::runtime_error(fmt::format("Unable to read the file {}", filename));

      // Go to the index of the image
      fits_movrel_hdu(file, m_image_index, nullptr, &status);
      if (status)
        throw std::runtime_error(fmt::format("Could not find the image at index {}", m_image_index));

      // Check if the HDU is an image
      int hdu_type;
      fits_get_hdu_type(file, &hdu_type, &status);
      if (hdu_type != IMAGE_HDU)
        throw std::runtime_error(fmt::format("HDU at index {} is not an image", m_image_index));

      // Get the number of dimension
      int ndim;
      fits_get_img_dim(file, &ndim, &status);
      if (ndim == 0 || ndim > 4)
        throw std::runtime_error(fmt::format("Unhandled image number of dimension (Got {}, expected in [1 - 4])", ndim));

      // Get the dimensions
      long dims[4];
      fits_get_img_size(file, ndim, dims, &status);

      // Get the type info
      int type;
      fits_get_img_type(file, &type, &status);
      const auto [sample_type, datatype] = get_type_info(type);

      auto impl    = std::make_unique<impl_cfitsio_t>();
      impl->file   = file;
      impl->m_ndim = ndim;
      for (int i = 0; i < ndim; i++)
        impl->m_dims[i] = dims[i];
      impl->m_sample_type_id = sample_type;
      impl->datatype = datatype;

      this->m_impl = std::move(impl);
    }

    void cfitsio_reader_plugin::close()
    {
      auto* impl = static_cast<impl_cfitsio_t*>(this->m_impl.get());
      if (impl && impl->file)
      {
        int status;
        fits_close_file(impl->file, &status);
        impl->file = nullptr;
      }
    }
  } // namespace internal

  mln::ndbuffer_image imread(const std::string& filename, int ind)
  {
    mln::ndbuffer_image out;
    imread(filename, out, ind);
    return out;
  }

  void imread(const std::string& filename, mln::ndbuffer_image& out, int ind)
  {
    internal::cfitsio_reader_plugin p(ind);
    mln::io::internal::load(&p, filename.c_str(), out);
  }
} // namespace mln::io::fits