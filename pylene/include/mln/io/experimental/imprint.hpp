#pragma once

#include <mln/core/box.hpp>
#include <mln/core/concept/images.hpp>
#include <mln/core/rangev3/foreach.hpp>

#include <memory>
#include <type_traits>
#include <fmt/format.h>

namespace mln::io::experimental
{
  /// Display (as characters) an image in the output stream
  ///
  /// \param image The image to display
  /// \param print_border Print the border of the image (only make senses if it has one).
  template <class I>
  void imprint(I&& image, bool print_border = false);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace internal
  {

    /// The class `formatter` is used to type-erase the image value type and
    /// enabled to use ```print(p)``` where ```print(ima(p))``` is intended.
    template <class P>
    class formatter
    {
    public:
      template <class I>
      formatter(I&& image)
      {
        new (&m_impl) impl_t<std::remove_reference_t<I>>(image);
      }

      std::size_t formatted_size(P p) const { return reinterpret_cast<const impl_base_t*>(&m_impl)->formatted_size(p); }
      void print(P p, int width) const { reinterpret_cast<const impl_base_t*>(&m_impl)->print(p, width); }

    private:
      struct impl_base_t
      {
        virtual ~impl_base_t() = default;
        virtual std::size_t formatted_size(P p) const = 0;
        virtual void print(P p, int width) const = 0;

        const void* m_ima;
      };

      template <class I>
      struct impl_t : impl_base_t
      {
        impl_t(I& x) { this->m_ima = (void*)(&x); }
        ~impl_t() final = default;
        std::size_t formatted_size(P p) const final { return fmt::formatted_size("{}", at(p)); }
        void        print(P p, int width) const final { fmt::print("{:>{}d}", at(p), width); }

        decltype(auto) at(P p) const { return const_cast<I*>(reinterpret_cast<const I*>(this->m_ima))->at(p); }
      };

      std::aligned_storage_t<sizeof(impl_base_t)> m_impl;
    };


    void imprint2d(const formatter<mln::experimental::point2d>& fmter, mln::experimental::box2d roi);
    void imprint3d(const formatter<mln::experimental::point3d>& fmter, mln::experimental::box3d roi);

    /// \group imprint overload set based on the type of the domain
    /// \{
    template <class I>
    void imprint(I&& image, const mln::experimental::box2d* roi)
    {
      imprint2d(std::forward<I>(image), *roi);
    }

    template <class I>
    void imprint(I&& image, const mln::experimental::box3d* roi)
    {
      imprint3d(std::forward<I>(image), *roi);
    }

    template <class I>
    void imprint(I&& image, ...)
    {
      mln_foreach_new(auto px, image.new_pixels())
        fmt::print("{{{},{}}}\n", px.point(), px.val());
    }
    /// \}
  } // namespace internal

  template <class I>
  void imprint(I&& image, [[maybe_unused]] bool print_border)
  {
    using U = std::remove_reference_t<I>;
    static_assert(mln::is_a<U, mln::experimental::Image>());
    static_assert(fmt::internal::has_formatter<image_value_t<U>, fmt::format_context>(), "The value type has no defined formatting.");

    auto roi = image.domain();
    if constexpr (std::is_convertible_v<image_extension_category_t<std::remove_reference_t<I>>,
                  mln::extension::border_extension_tag>)
      if (print_border)
        roi.inflate(image.border());

    internal::imprint(std::forward<I>(image), &roi);
  }

}
