#pragma once

#include <mln/core/rangev3/private/multi_view_facade.hpp>
#include <range/v3/view_facade.hpp>

#include <tuple>

namespace mln::ranges::view
{

  namespace details
  {

  } // namespace details

  template <typename Fun, typename... Rngs>
  struct zip_with_view : view_facade<zip_with_view<Fun, Rngs...>, ::ranges::finite>
  {
  private:
    Fun f_;
    std::tuple<Rngs...> rngs_;
    friend range_access;

    struct cursor;

    struct sentinel
    {
    private:
      friend struct cursor;
      std::tuple<sentinel_t<Rngs>...> ends_;

    public:
      sentinel() = default;
      sentinel(const Rng&... rngs) : ends_(std::make_tuple(::ranges::end(rngs)...)) {}
    };

    struct cursor
    {
    private:
      const Fun& f_;
      std::tuple<::ranges::iterator_t<Rngs>...> begins_;

    public:
      cursor() = default;
      explicit cursor(const Fun& f, const Rngs&... rngs)
          : f_(f), begins_(std::make_tuple(::ranges::begin(rngs)...))
      {
      }

      auto read() const
      {
        return std::apply([&f_](auto&& rng_it) { return f_(*rng_it); }, begins_);
      }

      constexpr bool equal(const sentinel& s) const
      {
        return std::apply(std::tie, begins_) == std::apply(std::tie, s.ends_);
      }
      bool equal(cursor const& rhs) const
      {
        return std::apply(std::tie, begins_) == std::apply(std::tie, rhs.begins_);
      }

      void next()
      {
        std::apply([&begins_](auto&& rng_it) { ++rng_it; }, begins_);
      }

      void prev()
      {
        std::apply([&begins_](auto&& rng_it) { --rng_it; }, begins_);
      }
    };

    cursor begin_cursor() { return std::make_from_tuple<cursor>(std::tuple_cat(std::make_tuple(f_), rngs_)); }
    sentinel end_cursor() { return std::make_from_tuple<sentinel>(rngs_); }
    cursor begin_cursor() const { return std::make_from_tuple<cursor>(std::tuple_cat(std::make_tuple(f_), rngs_)); }
    sentinel end_cursor() const { return std::make_from_tuple<sentinel>(rngs_); }

  public:
    zip_with_view() = default;
    constexpr explicit zip_with_view(Fun&& f, Rngs&&... rngs)
        : f_(std::forward<Fun>(f)), rngs_(std::make_tuple(std::forward<Rngs>(rngs)...))
    {
    }
  };

} // namespace mln::ranges::view
