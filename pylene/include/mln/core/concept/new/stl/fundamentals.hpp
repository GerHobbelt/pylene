#pragma once

#include <range/v3/utility/common_type.hpp>

#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>


namespace mln::concepts::stl::core
{
  using ::ranges::common_reference;
  using ::ranges::common_reference_t;


  // Same
  namespace detail
  {

    // clang-format off

    template <typename T, typename U>
    concept bool SameHelper = std::is_same_v<T, U>;

    // clang-format on

  } // namespace detail

  inline namespace core_language
  {

    // clang-format off

    template <typename T, typename U>
    concept bool Same =
      detail::SameHelper<T, U> &&
      detail::SameHelper<U, T>;


    // DerivedFrom
    template <typename Derived, typename Base>
    concept bool DerivedFrom =
      std::is_base_of_v<Base, Derived> &&
      std::is_convertible_v<const volatile Derived*, const volatile Base*>;


    // ConvertibleTo
    template <typename From, typename To>
    concept bool ConvertibleTo =
    std::is_convertible_v<From, To> &&
    requires(From (&f)()) {
      static_cast<To>(f());
    };


    // CommonReference
    template <typename T, typename U>
    concept bool CommonReference =
      Same<common_reference_t<T, U>, common_reference_t<U, T>> &&
      ConvertibleTo<T, common_reference_t<T, U>> &&
      ConvertibleTo<U, common_reference_t<T, U>>;


    // Common
    template <typename T, typename U>
    concept bool Common =
      Same<std::common_type_t<T, U>, std::common_type_t<U, T>> &&
      ConvertibleTo<T, std::common_type_t<T, U>> &&
      ConvertibleTo<U, std::common_type_t<T, U>> &&
      CommonReference<std::add_lvalue_reference_t<const T>, std::add_lvalue_reference_t<const U>> &&
      CommonReference<
        std::add_lvalue_reference_t<std::common_type_t<T, U>>,
        common_reference_t<std::add_lvalue_reference_t<const T>, std::add_lvalue_reference_t<const U>>>;


    // Integral
    template <typename T>
    concept bool Integral = std::is_integral_v<T>;


    // SignedIntegral
    template <typename T>
    concept bool SignedIntegral =
      Integral<T> &&
      std::is_signed_v<T>;


    // UnsignedIntegral
    template <typename T>
    concept bool UnsignedIntegral =
      Integral<T> &&
      !SignedIntegral<T>;


    // Assignable
    template <typename Lhs, typename Rhs>
    concept bool Assignable =
      std::is_lvalue_reference_v<Lhs> &&
      CommonReference<const std::remove_reference_t<Lhs>&, const std::remove_reference_t<Rhs>&> &&
      requires(Lhs lhs, Rhs&& rhs) {
        { lhs = std::forward<Rhs>(rhs) } -> Lhs;
      };


    // Swappable
    template <typename T>
    concept bool Swappable = std::is_swappable_v<T>;


    // Swappablewith
    template <typename T, typename U>
    concept bool SwappableWith =
      std::is_swappable_with_v<T, T> &&
      std::is_swappable_with_v<U, U> &&
      CommonReference<const std::remove_reference_t<T>&, const std::remove_reference_t<U>&> &&
      std::is_swappable_with_v<T, U> &&
      std::is_swappable_with_v<U, T>;


    // Destructible
    template <typename T>
    concept bool Destructible = std::is_nothrow_destructible_v<T>;


    // Constructible
    template <typename T, typename... Args>
    concept bool Constructible =
      Destructible<T> &&
      std::is_constructible_v<T, Args...>;


    // DefaultConstructible
    template <typename T>
    concept bool DefaultConstructible = Constructible<T>;


    // MoveConstructible
    template <typename T>
    concept bool MoveConstructible =
      Constructible<T, T> &&
      ConvertibleTo<T, T>;


    // CopyConstructible
    template <typename T>
    concept bool CopyConstructible =
      MoveConstructible<T> &&
      Constructible<T, T&> &&
      ConvertibleTo<T&, T> &&
      Constructible<T, const T&> &&
      ConvertibleTo<const T&, T> &&
      Constructible<T, const T> &&
      ConvertibleTo<const T, T>;

    // clang-format on

  } // namespace core_language


  inline namespace object
  {

    // clang-format off

    // Movable
    template <typename T>
    concept bool Movable =
      std::is_object_v<T> &&
      MoveConstructible<T> &&
      Assignable<T&, T> &&
      Swappable<T>;

    // clang-format on

  } // namespace object


  inline namespace comparison
  {

    // clang-format off

    // Boolean
    template <typename B>
    concept bool Boolean =
      //object::Movable<common::remove_cvref_t<B>> &&
      ConvertibleTo<const std::remove_reference_t<B>&, bool> &&
      requires( const std::remove_reference_t<B>& b1,
                const std::remove_reference_t<B>& b2,
                const bool a) {
        { !b1 }       ->  ConvertibleTo<bool>;
        { b1 && a }   ->  Same<bool>;
        { b1 || a }   ->  Same<bool>;
        { b1 && b2 }  ->  Same<bool>;
        { a && b2 }   ->  Same<bool>;
        { b1 || b2 }  ->  Same<bool>;
        { a || b2 }   ->  Same<bool>;
        { b1 == b2 }  ->  ConvertibleTo<bool>;
        { b1 == a }   ->  ConvertibleTo<bool>;
        { a == b2 }   ->  ConvertibleTo<bool>;
        { b1 != b2 }  ->  ConvertibleTo<bool>;
        { b1 != a }   ->  ConvertibleTo<bool>;
        { a != b2 }   ->  ConvertibleTo<bool>;
      };


    // __WeaklyEqualityComparableWith
    template <typename T, typename U>
    concept bool __WeaklyEqualityComparableWith = // exposition only
      requires( const std::remove_reference_t<T>& t,
                const std::remove_reference_t<U>& u) {
        {t == u} -> Boolean;
        {t != u} -> Boolean;
        {u == t} -> Boolean;
        {u != t} -> Boolean;
      };


    // EqualityComparable
    template <typename T>
    concept bool EqualityComparable = __WeaklyEqualityComparableWith<T, T>;


    // EqualityComparableWith
    template <typename T, typename U>
    concept bool EqualityComparableWith =
      EqualityComparable<T> &&
      EqualityComparable<U> &&
      core_language::CommonReference<
        const std::remove_reference_t<T>&,
        const std::remove_reference_t<U>&> &&
      EqualityComparable<
        common_reference_t<
          const std::remove_reference_t<T>&,
          const std::remove_reference_t<U>&>> &&
      __WeaklyEqualityComparableWith<T, U>;


    // StrictTotallyOrdered
    template <typename T>
    concept bool StrictTotallyOrdered =
      EqualityComparable<T> &&
      requires( const std::remove_reference_t<T>& a,
                const std::remove_reference_t<T>& b) {
        { a < b }   -> Boolean;
        { a > b }   -> Boolean;
        { a <= b }  -> Boolean;
        { a >= b }  -> Boolean;
      };


    // StrictTotallyOrderedWith
    template <typename T, typename U>
    concept bool StrictTotallyOrderedWith =
      StrictTotallyOrdered<T> &&
      StrictTotallyOrdered<U> &&
      CommonReference<
        const std::remove_reference_t<T>&,
        const std::remove_reference_t<U>&> &&
      StrictTotallyOrdered<
        common_reference_t<
          const std::remove_reference_t<T>&,
          const std::remove_reference_t<U>&>> &&
      EqualityComparableWith<T, U> &&
      requires( const std::remove_reference_t<T>& t,
                const std::remove_reference_t<U>& u) {
        { t < u }   -> Boolean;
        { t > u }   -> Boolean;
        { t <= u }  -> Boolean;
        { t >= u }  -> Boolean;
        { u < t }   -> Boolean;
        { u > t }   -> Boolean;
        { u <= t }  -> Boolean;
        { u >= t }  -> Boolean;
      };

    // clang-format on

  } // namespace comparison


  inline namespace object
  {

    // clang-format off

    // Copyable
    template <typename T>
    concept bool Copyable =
      CopyConstructible<T> &&
      Movable<T> &&
      Assignable<T&, const T&>;


    // Semiregular
    template <typename T>
    concept bool Semiregular =
      Copyable<T> &&
      DefaultConstructible<T>;


    // Regular
    template <typename T>
    concept bool Regular =
      Semiregular<T> &&
      EqualityComparable<T>;

    // clang-format on

  } // namespace object


  inline namespace callable
  {

    // clang-format off

    // Invocable
    template <typename F, typename... Args>
    concept bool Invocable =
      requires(F&& f, Args&&... args) {
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        /* not required to be equality preserving */
      };


    // InvocableR
    template <typename R, typename F, typename... Args>
    concept bool InvocableR =
      Invocable<F, Args...> &&
      Same<R, std::invoke_result_t<F, Args...>>;


    // RegularInvocable
    template <typename F, typename... Args>
    concept bool RegularInvocable = Invocable<F, Args...>;


    // RegularInvocableR
    template <typename R, typename F, typename... Args>
    concept bool RegularInvocableR =  InvocableR<R, F, Args...>;


    // Predicate
    template <typename F, typename... Args>
    concept bool Predicate =
      RegularInvocable<F, Args...> &&
      Boolean<std::invoke_result_t<F, Args...>>;


    // Relation
    template <typename R, typename T, typename U>
    concept bool Relation =
      Predicate<R, T, T> &&
      Predicate<R, U, U> &&
      CommonReference<
        const std::remove_reference_t<T>&,
        const std::remove_reference_t<U>&> &&
      Predicate<R,
        common_reference_t<
          const std::remove_reference_t<T>&,
          const std::remove_reference_t<U>&>,
        common_reference_t<
          const std::remove_reference_t<T>&,
          const std::remove_reference_t<U>&>> &&
      Predicate<R, T, U> &&
      Predicate<R, U, T>;


    // StrictWeakOrder
    template <typename R, typename T, typename U>
    concept bool StrictWeakOrder = Relation<R, T, U>;

    // clang-format on

  } // namespace callable


  inline namespace iterator
  {

    // clang-format off


    // Readable
    template <typename In>
    concept bool Readable =
      requires {
        typename In::value_type;
        typename In::reference;
        typename In::rvalue_reference;
      } &&
      CommonReference<typename In::reference&&, typename In::value_type&> &&
      CommonReference<typename In::reference&&, typename In::rvalue_reference&&> &&
      CommonReference<typename In::rvalue_reference&&, const typename In::value_type&>;


    // Writable
    template <typename Out, typename T>
    concept bool Writable =
      requires {
        typename Out::reference;
      } &&
      requires(Out&& o, T&& t) {
        *o = std::forward<T>(t);
        *std::forward<Out>(o) = std::forward<T>(t);
        const_cast<const typename Out::reference&&>(*o) = std::forward<T>(t);
        const_cast<const typename Out::reference&&>(*std::forward<Out>(o)) = std::forward<T>(t);
      };


    // WeaklyIncrementable
    template <typename I>
    concept bool WeaklyIncrementable =
      Semiregular<I> &&
      requires {
        typename I::difference_type;
      } &&
      SignedIntegral<typename I::difference_type> &&
      requires(I i) {
        { ++i } ->  Same<I&>;  /* not required to be equality preserving */
        { i++ }                /* not required to be equality preserving */
      };


    // Incrementable
    template <typename I>
    concept bool Incrementable =
      Regular<I> &&
      WeaklyIncrementable<I> &&
      requires(I i) {
        { i++ } ->  Same<I&&>;
      };


    // Iterator
    template <typename I>
    concept bool Iterator =
      WeaklyIncrementable<I> &&
      requires(I i) {
        { *i }  ->  auto&&;
      };


    // Sentinel
    template <typename S, typename I>
    concept bool Sentinel =
      Semiregular<S> &&
      Iterator<I> &&
      __WeaklyEqualityComparableWith<S, I>;


    // SizedSentinel
    template<typename S, typename I>
    inline constexpr bool disable_sized_sentinel = false; /* can be specialized */

    template <typename S, typename I>
    concept bool SizedSentinel =
      Sentinel<S, I> &&
      !disable_sized_sentinel<std::remove_cv_t<S>, std::remove_cv_t<I>> &&
      requires(const I& i, const S& s) {
        { s - i } ->  Same<typename I::difference_type&&>;
        { i - s } ->  Same<typename I::difference_type&&>;
      };


    // InputIterator
    template <typename I>
    concept bool InputIterator =
      Iterator<I> &&
      Readable<I> &&
      requires {
        typename I::iterator_category;
      } &&
      DerivedFrom<typename I::iterator_category, std::input_iterator_tag>;


    // OutputIterator
    template <typename I, typename T>
    concept bool OutputIterator =
      Iterator<I> &&
      Writable<I, T> &&
      requires(I i, T&& t) {
        *i++ = std::forward<T>(t);  /* not required to be equality preserving */
      };


    // ForwardIterator
    template <typename I>
    concept bool ForwardIterator =
      InputIterator<I> &&
      DerivedFrom<typename I::iterator_category, std::forward_iterator_tag> &&
      Incrementable<I> &&
      Sentinel<I, I>;


    // BidirectionalIterator
    template <typename I>
    concept bool BidirectionalIterator =
      ForwardIterator<I> &&
      DerivedFrom<typename I::iterator_category, std::bidirectional_iterator_tag> &&
      requires(I i) {
        { --i } ->  Same<I&>;
        { i-- } ->  Same<I&&>;
      };


    // RandomAccessIterator
    template <typename I>
    concept bool RandomAccessIterator =
      BidirectionalIterator<I> &&
      DerivedFrom<typename I::iterator_category, std::random_access_iterator_tag> &&
      StrictTotallyOrdered<I> &&
      SizedSentinel<I, I> &&
      requires(I i, const I j, const typename I::difference_type n) {
        { i += n }  ->  Same<I&>;
        { j + n }   ->  Same<I&&>;
        { n + j }   ->  Same<I&&>;
        { i -= n }  ->  Same<I&>;
        { j - n }   ->  Same<I&&>;
        { j[n] }    ->  Same<typename I::reference>;
      };

    // clang-format on

  } // namespace iterator

} // namespace mln::concepts::stl::core
