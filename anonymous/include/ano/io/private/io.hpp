#pragma once

#include <ano/core/box.hpp>
#include <ano/core/concepts/image.hpp>
#include <ano/core/image/ndimage_fwd.hpp>
#include <ano/core/image_format.hpp>
#include <ano/core/range/rows.hpp>
#include <ano/io/private/plugin.hpp>

#include <range/v3/algorithm/copy.hpp>

namespace ano::io::internal
{
  template <class I>
  void save2d(ano::details::Image<I>& input, plugin_writer* p, const char* filename);

  template <class I>
  void save2d(ano::details::Image<I>&& input, plugin_writer* p, const char* filename);

  template <class I>
  void save2d(const ano::details::Image<I>& input, plugin_writer* p, const char* filename);

  // Specialization for types convertible to ano::ndbuffer_image
  inline void save2d(const ano::ndbuffer_image& input, plugin_writer* p, const char* filename);


  // Generic ndimension algorithm for buffer encoded image
  void load(plugin_reader* p, const char* filename, ano::ndbuffer_image& output);
  void save(const ano::ndbuffer_image& input, plugin_writer* p, const char* filename);
} // namespace ano::io::internal


namespace ano::io::internal
{
  namespace impl
  {

    template <class Image>
    void save2d(Image&& input, plugin_writer* p, const char* filename)
    {
      using I = std::remove_reference_t<Image>;
      using V = image_value_t<I>;

      static_assert(ano::is_a<I, ano::details::Image>());
      static_assert(std::is_same<image_domain_t<I>, ano::box2d>() && "The domain must be a regular box2d");

      ano::box2d     domain = input.domain();
      int            height = domain.height();
      int            width  = domain.width();
      sample_type_id tid    = sample_type_traits<V>::id();

      // Read header
      int dims[2] = {width, height};
      p->open(filename, tid, 2, dims);

      auto buffer = std::make_unique<V[]>(width);

      // Fill content
      {
        auto vals = input.values();
        auto rows = ano::ranges::rows(vals);
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
  void save2d(ano::details::Image<I>&& input, plugin_writer* p, const char* filename)
  {
    impl::save2d(static_cast<I&&>(input), p, filename);
  }

  template <class I>
  void save2d(ano::details::Image<I>& input, plugin_writer* p, const char* filename)
  {
    impl::save2d(static_cast<I&>(input), p, filename);
  }


  template <class I>
  void save2d(const ano::details::Image<I>& input, plugin_writer* p, const char* filename)
  {
    impl::save2d(static_cast<const I&>(input), p, filename);
  }

  void save2d(const ano::ndbuffer_image& input, plugin_writer* p, const char* filename)
  {
    save(input, p, filename);
  }

} // namespace ano::io::internal
