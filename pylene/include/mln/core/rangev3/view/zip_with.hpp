#pragma once

#include <mln/core/rangev3/private/multi_view_facade.hpp>
#include <mln/core/rangev3/private/multidimensional_range.hpp>
#include <mln/core/utils/blank.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view_facade.hpp>

#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace mln::ranges::view
{

  // Forward declaration of zip_with
  template <typename Fun, typename... Rngs>
  auto zip_with(Fun f, Rngs&&... rngs);

  namespace details
  {

    template <class Fun>
    struct row_zipper
    {
      ::ranges::semiregular_t<Fun> f_;

      row_zipper() = default;
      row_zipper(Fun f) : f_ {std::move(f)}
      {
      }

      template <class... TArgs>
      auto operator() (TArgs&&... rows) const
      {
        return zip_with(f_, std::forward<TArgs>(rows)...);
      }
    };

  }


  template <typename Fun, typename... Rngs>
  struct zip_with_view : ::ranges::view_facade<zip_with_view<Fun, Rngs...>, ::ranges::finite>,
                         std::conditional_t<std::conjunction_v<is_multidimensional_range<Rngs>...>,
                                            multidimensional_range_base, mln::details::blank>
  {
  private:
    friend ::ranges::range_access;

    ::ranges::semiregular_t<Fun> f_;
    std::tuple<Rngs...> rngs_;

    struct cursor;

    struct sentinel
    {
    private:
      friend struct cursor;
      std::tuple<::ranges::sentinel_t<Rngs>...> ends_;

    public:
      sentinel() = default;
      sentinel(::ranges::sentinel_t<Rngs>... rngs) : ends_(rngs...) {}
    };

    struct cursor
    {
    private:
      using fun_ref_ = ::ranges::semiregular_ref_or_val_t<Fun, true>;
      fun_ref_ fun_;
      std::tuple<::ranges::iterator_t<Rngs>...> begins_;

    public:
      cursor() = default;
      explicit cursor(fun_ref_ f, ::ranges::iterator_t<Rngs>... its) : fun_(f), begins_(its...) {}

      auto read() const
      {
        auto f = [this](const auto&... it) { return this->fun_(*it...); };
        return std::apply(f, begins_);
      }

      bool equal(const sentinel& s) const
      {
        return std::get<0>(begins_) == std::get<0>(s.ends_);
        // return details::tuple_any(details::tuple_zip_two_with(begins_, s.ends_, [](auto&& lhs, auto&& rhs) {
        //   return std::forward<decltype(lhs)>(lhs) == std::forward<decltype(rhs)>(rhs);
        // }));
      }

      void next()
      {
        std::apply([](auto&... rng_it) { (++rng_it, ...); }, begins_);
      }
    };


    cursor begin_cursor()
    {
      auto f = [this] (auto&&... args) { return cursor(this->f_, ::ranges::begin(std::forward<decltype(args)>(args))...); };
      return std::apply(f, rngs_);
    }
    sentinel end_cursor()
    {
      auto f = [] (auto&&... args) { return sentinel(::ranges::end(std::forward<decltype(args)>(args))...); };
      return std::apply(f, rngs_);
    }
    cursor begin_cursor() const
    {
      auto f = [this] (auto&&... args) { return cursor(this->f_, ::ranges::begin(std::forward<decltype(args)>(args))...); };
      return std::apply(f, rngs_);
    }
    sentinel end_cursor() const
    {
      auto f = [] (auto&&... args) { return sentinel(::ranges::end(std::forward<decltype(args)>(args))...); };
      return std::apply(f, rngs_);
    }

  public:
    zip_with_view() = default;
    explicit zip_with_view(Fun f, Rngs... rngs) : f_(std::move(f)), rngs_(std::move(rngs)...) {}

    // Fixme: enable this function only if all zipped ranges are segmented
    auto rows() const
    {
      // Zip function for rows
      // Using this lambda is not regular so we cheat for optimization
      auto row_zipper = [fun = this->f_](auto&&... rows) { return zip_with(fun, std::forward<decltype(rows)>(rows)...); };
      //details::row_zipper zipper(this->f_);

      // Apply row-zipper on each range
      return std::apply([row_zipper](const auto&... rng) { return zip_with(row_zipper, rng.rows()...); }, rngs_);
    }
  };

  template <typename Fun, typename... Rngs>
  auto zip_with(Fun f, Rngs&&... rngs)
  {
    return zip_with_view<Fun, ::ranges::view::all_t<Rngs>...>(std::move(f), ::ranges::view::all(std::forward<Rngs>(rngs))...);
  }

} // namespace mln::ranges::view
