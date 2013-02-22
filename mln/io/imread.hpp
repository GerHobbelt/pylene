#ifndef MLN_IO_IMREAD_HH
# define MLN_IO_IMREAD_HH

# include <string>

# include <mln/core/macros.hpp>
# include <mln/core/image_traits.hpp>
# include <mln/io/reader.hpp>
# include <mln/io/ioexception.hpp>
//# include <mln/io/registration.hpp>
# include <mln/io/freeimage_reader.hpp>

namespace mln
{
  namespace io
  {

    /// \brief Load an image from disk.
    /// \ingroup io
    ///
    /// \param path Path to the image
    /// \param[out] out Image to be loaded
    /// \param r The plugin to use for loading
    template <typename Image, typename Reader = freeimage_reader>
    void imread(const std::string& path, Image& out, Reader r = Reader());


    /******************************************/
    /****          Implementation          ****/
    /******************************************/


    namespace internal
    {

      template <typename Image, typename Reader>
      void read_raw_image(Image& ima, const typename Image::point_type& shape, Reader& r, char* buffer, int dim)
      {
	if (dim == Image::ndim-1)
	  r.read_next_line(buffer);
	else {
	  for (int i = 0; i < shape[dim]; ++i) {
	    read_raw_image(ima, shape, r, buffer, dim+1);
	    buffer += ima.strides()[dim];
	  }
	}
      }

      template <typename Image, typename Reader>
      void imread_raw(Reader& r, Image& out)
      {
	mln_point(Image) p = out.domain().pmin;
	mln_point(Image) shape = out.domain().shape();

	char* buffer = (char*) &out(p);
	read_raw_image(out, shape, r, buffer, 0);
      }


      // template <typename Image, typename Reader>
      // void imread(const char* path, Reader& r, Image& out, image_dynamic_tag)
      // {
      //   mln_assertion(false);
      // }

      template <typename Image, typename Reader>
      void imread(const std::string& path, Reader r, Image& out, raw_image_tag)
      {
	r.initialize();
	r.load(path.c_str());

	typedef typename Image::value_type V;
        std::type_index ridx = r.get_value_type_id();
	if (Image::ndim != r.get_ndim())
	  throw MLNIOException("Dimensions incompatibles");
	else if (ridx != typeid(V)) {
	  std::string ex = "Value types incompatibles: ";
	  (ex += "expected: ") += ridx.name();
	  (ex += " but got ") += typeid(V).name();
	  throw MLNIOException(ex);
	}

	out.resize(r.get_domain(), out.border());
	imread_raw(r, out);
	r.close();
      }
    }

    template <typename Image, typename Reader>
    void imread(const std::string& path, Image& out, Reader r)
    {
      internal::imread(path, r, out, typename image_traits<Image>::category ());
    }


  } /*end of namespace mln::io */
} /* end of namespace mln */
# endif
