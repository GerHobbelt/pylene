#pragma once

#include <mln/core/concept/new/images.hpp>
#include <mln/core/concept/object.hpp>

#include <mln/core/image/experimental/ndimage_fwd.hpp>
#include <mln/core/box.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/image_format.hpp>
#include <mln/io/private/plugin.hpp>

#include <range/v3/algorithm/copy.hpp>

namespace mln::io::internal
{
  template <class I>
  void save2d(mln::experimental::Image<I>& input, plugin_writer* p, const char* filename);

  template <class I>
  void save2d(mln::experimental::Image<I>&& input, plugin_writer* p, const char* filename);

  template <class I>
  void save2d(const mln::experimental::Image<I>& input, plugin_writer* p, const char* filename);

  // Specialization for types convertible to mln::ndbuffer_image
  inline void save2d(const mln::ndbuffer_image& input, plugin_writer* p, const char* filename);


  // Generic ndimension algorithm for buffer encoded image
  void load(plugin_reader* p, const char* filename, mln::ndbuffer_image& output);
  void save(const mln::ndbuffer_image& input, plugin_writer* p, const char* filename);
}


namespace mln::io::internal
{
  namespace impl
  {

    template <class Image>
    void save2d(Image&& input, plugin_writer* p, const char* filename)
    {
      using I = std::remove_reference_t<Image>;
      using V = image_value_t<I>;

      static_assert(mln::is_a<I, mln::experimental::Image>());
      static_assert(std::is_same<image_domain_t<I>, mln::experimental::box2d>() &&
                    "The domain must be a regular box2d");

      mln::experimental::box2d domain = input.domain();
      int                      height = domain.height();
      int                      width  = domain.width();
      sample_type_id           tid    = sample_type_traits<V>::id();

      // Read header
      int dims[2] = {width, height};
      p->open(filename, tid, 2, dims);

      auto buffer = std::make_unique<V[]>(width);

      // Fill content
      {
        auto vals = input.new_values();
        auto rows = mln::ranges::rows(vals);
        for (auto&& r : rows)
        {
          ::ranges::copy(::ranges::begin(r), ::ranges::end(r), buffer.get());
          p->write_next_line(reinterpret_cast<std::byte*>(buffer.get()));
        }
      }

      // Close handle
      p->close();
    }

  } // namespace impl

  template <class I>
  void save2d(mln::experimental::Image<I>&& input, plugin_writer* p, const char* filename)
  {
    impl::save2d(static_cast<I&&>(input), p, filename);
  }

  template <class I>
  void save2d(mln::experimental::Image<I>& input, plugin_writer* p, const char* filename)
  {
    impl::save2d(static_cast<I&>(input), p, filename);
  }


  template <class I>
  void save2d(const mln::experimental::Image<I>& input, plugin_writer* p, const char* filename)
  {
    impl::save2d(static_cast<const I&>(input), p, filename);
  }

  void save2d(const mln::ndbuffer_image& input, plugin_writer* p, const char* filename)
  {
    save(input, p, filename);
  }

} // namespace mln::io::internal
