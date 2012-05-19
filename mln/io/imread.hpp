#ifndef MLN_IO_IMREAD_HH
# define MLN_IO_IMREAD_HH

# include <string>

# include <mln/core/macros.hpp>
# include <mln/core/image_traits.hpp>
# include <mln/io/reader.hpp>
# include <mln/io/ioexception.hpp>
# include <mln/io/registration.hpp>
# include <mln/io/freeimage_reader.hpp>

namespace mln
{
  namespace io
  {

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
      void imread(const char* path, Reader r, Image& out, raw_image_tag)
      {
	r.initialize();
	r.load(path);

	typedef typename Image::value_type V;
	const type_info_base* rinfo = registration_map_[r.get_value_type_id()];
	if (Image::ndim != r.get_ndim())
	  throw MLNIOException("Dimensions incompatibles");
	else if (type_info<V>::id_ != rinfo->id()) {
	  std::string ex = "Value types incompatibles: ";
	  (ex += "expected: ") += rinfo->name();
	  (ex += " but got ") += type_info<V>::name_;
	  throw MLNIOException(ex);
	}

	out.resize(r.get_domain(), out.border());
	imread_raw(r, out);
	r.close();
      }
    }

    template <typename Image, typename Reader = freeimage_reader>
    void imread(const char* path, Image& out, Reader r = Reader())
    {
      internal::imread(path, r, out, typename image_traits<Image>::category ());
    }


  } /*end of namespace mln::io */
} /* end of namespace mln */
# endif
