#pragma once

#include <mln/core/rangev3/private/multidimensional_range.hpp>
#include <mln/core/rangev3/private/reversible_range.hpp>

#include <mln/core/rangev3/range_traits.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <mln/core/utils/blank.hpp>

#include <range/v3/view/remove_if.hpp>

namespace mln::ranges
{

  namespace details
  {
    // Bad way to access the private member
    template <typename Rng, typename Pred>
    struct remove_if_view_access
      : ::ranges::view_adaptor<remove_if_view_access<Rng, Pred>, Rng,
                               ::ranges::is_finite<Rng>::value ? ::ranges::finite
                                                               : ::ranges::range_cardinality<Rng>::value>,
        ::ranges::box<::ranges::semiregular_t<Pred>>
    {
      ::ranges::detail::non_propagating_cache<::ranges::iterator_t<Rng>> begin_;
    };

  } // namespace details

  template <typename Rng, typename Pred>
  struct remove_if_view
    : ::ranges::remove_if_view<Rng, Pred>,
      std::conditional_t<is_multidimensional_range_v<Rng>, multidimensional_range_base, mln::details::blank>
  {
  private:
    using base_t = ::ranges::remove_if_view<Rng, Pred>;

    ::ranges::semiregular_t<Pred> get_pred() const
    {
      return reinterpret_cast<const details::remove_if_view_access<Rng, Pred>*>(this)
          ->details::template remove_if_view_access<Rng, Pred>::box::get();
    }

  public:
    using base_t::base_t;

    template <class U = void, class = std::enable_if_t<is_multidimensional_range_v<Rng>, U>>
    auto rows() const
    {
      auto f = [this](auto row) {
        return remove_if_view<decltype(row), Pred>(std::forward<decltype(row)>(row), this->get_pred());
      };
      return view::transform(this->base().rows(), f);
    }

    template <class U = void, class = std::enable_if_t<is_reversible_range_v<Rng>, U>>
    auto reversed() const
    {
      return remove_if_view<decltype(this->base().reversed()), Pred>(this->base().reversed(), this->get_pred());
    }
  };

  namespace view
  {
    struct remove_if_fn
    {
    private:
      friend ::ranges::view::view_access;
      template <typename Pred>
      static auto bind(remove_if_fn remove_if, Pred pred)
          RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(::ranges::make_pipeable(std::bind(remove_if, std::placeholders::_1,
                                                                                 ::ranges::protect(std::move(pred)))));
      // //

    public:
      template <typename Rng, typename Pred>
      using Constraint =
          ::meta::and_<::ranges::InputRange<Rng>, ::ranges::IndirectPredicate<Pred, ::ranges::iterator_t<Rng>>>;

      template <typename Rng, typename Pred, CONCEPT_REQUIRES_(Constraint<Rng, Pred>())>
      RANGES_CXX14_CONSTEXPR auto operator()(Rng&& rng, Pred pred) const
          RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(remove_if_view<::ranges::view::all_t<Rng>, Pred>{
              ::ranges::view::all(static_cast<Rng&&>(rng)), std::move(pred)})

              ; //

      template <typename Rng, typename Pred, CONCEPT_REQUIRES_(!Constraint<Rng, Pred>())>
      void operator()(Rng&&, Pred) const
      {
        CONCEPT_ASSERT_MSG(::ranges::InputRange<Rng>(), "The first argument to view::remove_if must be a model of the "
                                                        "InputRange concept");
        CONCEPT_ASSERT_MSG(::ranges::IndirectPredicate<Pred, ::ranges::iterator_t<Rng>>(),
                           "The second argument to view::remove_if must be callable with "
                           "a value of the range, and the return type must be convertible "
                           "to bool");
      }
    };

    RANGES_INLINE_VARIABLE(::ranges::view::view<remove_if_fn>, remove_if)
  } // namespace view

} // namespace mln::ranges


RANGES_SATISFY_BOOST_RANGE(mln::ranges::remove_if_view)
