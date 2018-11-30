#pragma once


#include <mln/core/rangev3/private/multidimensional_range.hpp>
#include <mln/core/rangev3/private/reversible_range.hpp>
#include <mln/core/rangev3/range_traits.hpp>
#include <mln/core/utils/blank.hpp>

#include <mln/core/concept/new/concepts.hpp>

#include <range/v3/view/transform.hpp>

namespace mln::ranges
{
  namespace detail
  {
    template <typename Rng, typename Fun>
    struct iter_transform_view_public : ::ranges::view_adaptor<iter_transform_view_public<Rng, Fun>, Rng>
    {
      ::ranges::semiregular_t<Fun> fun_;
    };
  } // namespace detail

  template <typename Rng, typename Fun>
  struct iter_transform_view : ::ranges::iter_transform_view<Rng, Fun>
  {
  private:
    // Very bad way to access the private member
    auto fun() const { return reinterpret_cast<const detail::iter_transform_view_public<Rng, Fun>*>(this)->fun_; }

  public:
    using ::ranges::iter_transform_view<Rng, Fun>::iter_transform_view;

    template <typename U = void, typename = std::enable_if_t<is_multidimensional_range_v<Rng>, U>>
    auto rows() const
    {
      return ::ranges::view::transform(this->base().rows(),
                                       [fun_ = fun()](auto row) { return ::ranges::view::iter_transform(row, fun_); });
    }

    template <typename U = void, typename = std::enable_if_t<is_reversible_range_v<Rng>, U>>
    auto reversed() const
    {
      return iter_transform_view<decltype(this->base().reversed()), Fun>(this->base().reversed(), fun());
    }
  };

  template <typename Rng, typename Fun>
  struct transform_view : iter_transform_view<Rng, ::ranges::indirected<Fun>>
  {
    transform_view() = default;
    transform_view(Rng rng, Fun fun)
      : iter_transform_view<Rng, ::ranges::indirected<Fun>>{std::move(rng), ::ranges::indirect(std::move(fun))}
    {
    }
  };


  namespace view
  {

    struct transform_fn
    {
    private:
      friend ::ranges::view::view_access;
      template <typename Fun>
      static auto bind(transform_fn transform, Fun fun)
          RANGES_DECLTYPE_AUTO_RETURN(::ranges::make_pipeable(std::bind(transform, std::placeholders::_1,
                                                                        ::ranges::protect(std::move(fun)))))

              ;

    public:
      // Don't forget to update view::for_each whenever this set
      // of concepts changes
      template <typename Rng, typename Fun>
      using Concept =
          ::meta::and_<::ranges::InputRange<Rng>, ::ranges::CopyConstructible<Fun>,
                       ::ranges::Invocable<Fun&, ::ranges::range_reference_t<Rng>>,
                       ::meta::not_<::ranges::Same<
                           void, ::ranges::concepts::Invocable::result_t<Fun&, ::ranges::range_reference_t<Rng>>>>>;

      template <typename Rng, typename Fun, CONCEPT_REQUIRES_(Concept<Rng, Fun>())>
#ifdef PYLENE_CONCEPT_TS_ENABLED
          // clang-format off
      requires mln::concepts::stl::InputRange<Rng> &&
               mln::concepts::stl::CopyConstructible<Fun> &&
               mln::concepts::stl::Invocable<Fun&, ::ranges::range_reference_t<Rng>> &&
               not mln::concepts::stl::Same<void, std::invoke_result_t<Fun&, ::ranges::range_reference_t<Rng>>>
#endif // PYLENE_CONCEPT_TS_ENABLED
      transform_view<::ranges::view::all_t<Rng>, Fun> operator()(Rng&& rng, Fun fun) const
      // clang-format on
      {
        return {::ranges::view::all(static_cast<Rng&&>(rng)), std::move(fun)};
      }

#ifndef RANGES_DOXYGEN_INVOKED
      template <typename Rng, typename Fun, CONCEPT_REQUIRES_(!Concept<Rng, Fun>())>
      void operator()(Rng&&, Fun) const
      {
        CONCEPT_ASSERT_MSG(::ranges::InputRange<Rng>(),
                           "The object on which view::transform operates must be a model of the "
                           "InputRange concept.");
        CONCEPT_ASSERT_MSG(::ranges::CopyConstructible<Fun>(),
                           "The function passed to view::transform must be CopyConstructible.");
        CONCEPT_ASSERT_MSG(::ranges::Invocable<Fun&, ::ranges::range_reference_t<Rng>>(),
                           "The function passed to view::transform must be callable with an argument "
                           "of the range's reference type.");
        CONCEPT_ASSERT_MSG(
            meta::not_<::ranges::Same<
                void, ::ranges::concepts::Invocable::result_t<Fun&, ::ranges::range_reference_t<Rng>>>>(),
            "The function passed to view::transform must return non-void when called "
            "with an argument of the range's reference type.");
      }

#endif
    };

    /// \relates transform_fn
    /// \ingroup group-views
    RANGES_INLINE_VARIABLE(::ranges::view::view<transform_fn>, transform)
  } // namespace view

} // namespace mln::ranges

RANGES_SATISFY_BOOST_RANGE(mln::ranges::iter_transform_view)
RANGES_SATISFY_BOOST_RANGE(mln::ranges::transform_view)
