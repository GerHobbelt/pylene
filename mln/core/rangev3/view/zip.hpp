#pragma once

#include <mln/core/rangev3/private/multi_view_facade.hpp>
#include <mln/core/rangev3/private/multidimensional_range.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/view/zip.hpp>

#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace mln::ranges::view
{

  namespace details
  {

    template <typename... Rngs>
    inline constexpr bool is_multidimensional = std::conjunction_v<mln::ranges::is_multidimensional_range<Rngs>...>;

    struct __nothing
    {
    };

  } // namespace details

  template <typename... Rngs>
  struct zip_view : public ::ranges::view_facade<zip_view<Rngs...>, ::ranges::finite>,
                    public std::conditional_t<details::is_multidimensional<Rngs...>, multidimensional_range_base,
                                              details::__nothing>
  {
  private:
    friend ::ranges::range_access;

    std::tuple<Rngs...> rngs_;

    struct cursor;

    struct sentinel
    {
    private:
      friend struct cursor;
      std::tuple<::ranges::sentinel_t<Rngs>...> ends_;

    public:
      sentinel() = default;
      template <typename... R>
      sentinel(R&... rngs) : ends_(std::end(rngs)...) {}
    };

    struct cursor
    {
    private:
      std::tuple<::ranges::iterator_t<Rngs>...> begins_;

    public:
      cursor() = default;
      template <typename... R>
      explicit cursor(R&... rngs) : begins_(std::begin(rngs)...) {}

      auto read() const
      {
        return std::apply([](const auto&... rng_it) { return std::make_tuple(*rng_it...); }, begins_);
      }

      constexpr bool equal(const sentinel& s) const { return std::get<0>(begins_) == std::get<0>(s.ends_); }

      bool equal(cursor const& rhs) const { return std::get<0>(begins_) == std::get<0>(rhs.begins_); }

      void next()
      {
        std::apply([](auto&... rng_it) { (++rng_it, ...); }, begins_);
      }

      void prev()
      {
        std::apply([](auto&... rng_it) { (--rng_it, ...); }, begins_);
      }
    };

    cursor begin_cursor() { return std::make_from_tuple<cursor>(rngs_); }
    sentinel end_cursor() { return std::make_from_tuple<sentinel>(rngs_); }
    cursor begin_cursor() const { return std::make_from_tuple<cursor>(rngs_); }
    sentinel end_cursor() const { return std::make_from_tuple<sentinel>(rngs_); }

  public:
    zip_view() = default;
    constexpr explicit zip_view(Rngs... rngs) : rngs_(std::move(rngs)...) {}

    auto rows() const
    {
      // ret is a range range of tuple of row
      return std::apply(
          [](auto&... rng) {
            // \p rng.rows is a range of lines
            return ::ranges::view::zip_with(
                [](auto&&... row) {
                  // transform a tuple of line into a line of tuple
                  return ::ranges::view::zip(row...);
                },
                rng.rows()...);
          },
          rngs_);
    }
  };

template <typename... Rngs>
auto zip(Rngs... rngs)
{
  return zip_view<Rngs...>(std::move(rngs)...);
}

} // namespace mln::ranges::view
