#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <tuple>
#include <type_traits>


namespace mln
{


  template <class... Images>
  class zip_view : public New_Image<zip_view<Images...>>
  {
    using tuple_t = std::tuple<Images...>;
    using I0      = std::tuple_element_t<0, tuple_t>;


    tuple_t m_images;

  public:
    /// Type definitions
    /// \{
    using reference     = std::tuple<image_reference_t<Images>...>;
    using value_type    = std::tuple<image_value_t<Images>...>;
    using point_type    = image_point_t<I0>;
    using concrete_type = image_ch_value_t<I0, value_type>;
    using domain_type   = typename I0::domain_type;

    template <typename V>
    using ch_value_type = image_ch_value_t<I0, V>;
    /// \}


    /// Traits & Image Properties
    /// \{
    using accessible         = typename std::conjunction<typename Images::accessible...>::type;
    using indexable          = std::false_type;                    // Preservative behavior
    using extension_category = mln::extension::none_extension_tag; // Preservative behavior
    /// \}

    static_assert(std::conjunction_v<std::is_same<image_point_t<Images>, point_type>...>,
                  "The point type of all images to be zipped must be the same.");


  public:
    /// Pixel type definitions
    /// \{
    struct new_pixel_type : Pixel<new_pixel_type>
    {
      using reference  = zip_view::reference;
      using value_type = zip_view::value_type;
      using point_type = zip_view::point_type;

      new_pixel_type() = default;
      new_pixel_type(image_pixel_t<Images>... pixels)
        : m_pixels{std::move(pixels)...}
      {
      }

      point_type point() const { return std::get<0>(m_pixels).point(); }
      reference  val() const
      {
        auto build_value = [](auto&&... pixels) -> reference { return {pixels.val()...}; };
        return std::apply(build_value, m_pixels);
      }

      void advance(point_type q)
      {
        auto g = [q](auto&&... pixels) { (pixels.advance(q), ...); };
        std::apply(g, m_pixels);
      }

    private:
      std::tuple<image_pixel_t<Images>...> m_pixels;
    };
    /// \}

    zip_view() = default;

    zip_view(Images... images)
      : m_images{std::move(images)...}
    {
    }

    auto domain() const { return std::get<0>(m_images).domain(); }

    auto new_values()
    {
      auto g_new_values = [](auto&&... images) { return ranges::view::zip(images.new_values()...); };
      return std::apply(g_new_values, m_images);
    }

    auto new_pixels()
    {
      auto g_new_pixels = [](auto&&... images) {
        return ranges::view::zip_with([](auto&&... pixels) { return new_pixel_type(pixels...); },
                                      images.new_pixels()...);
      };
      return std::apply(g_new_pixels, m_images);
    }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> operator()(point_type p)
    {
      mln_precondition(all_domain_has(p));
      return this->at(p);
    }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> at(point_type p)
    {
      auto g = [p](auto&&... images) { return std::forward_as_tuple(images.at(p)...); };
      return std::apply(g, m_images);
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel(point_type p)
    {
      mln_precondition(all_domain_has(p));
      return this->new_pixel_at(p);
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel_at(point_type p)
    {
      auto g = [p](auto&&... images) { return new_pixel_type(images.new_pixel_at(p)...); };
      return std::apply(g, m_images);
    }

  private:
    template <typename dummy = bool>
    std::enable_if_t<accessible::value, dummy> all_domain_has(point_type p)
    {
      auto g_has = [p](auto&&... images) { return (images.has(p) && ...); };
      return std::apply(g_has, m_images);
    }
  };


  namespace view
  {
    template <class... Images>
    zip_view<Images...> zip(Images... images)
    {
      static_assert(std::conjunction_v<is_a<Images, New_Image>...>, "All zip arguments must be images.");

      return zip_view<Images...>(std::move(images)...);
    }
  } // namespace view

} // namespace mln
