#pragma once

// FIXME : reduce to needed headers
#include <range/v3/all.hpp>

#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>


namespace mln::core::concepts::stl
{


  // Same
  namespace detail
  {

    using ::ranges::begin;

    using ::ranges::disable_sized_range;
    using ::ranges::disable_sized_sentinel;
    using ::ranges::end;
    using ::ranges::equal_to;
    using ::ranges::ident;
    using ::ranges::iter_swap;
    using ::ranges::less;
    using ::ranges::projected;
    using ::ranges::size;
    using ::ranges::detail::view_predicate_;

    using ::ranges::common_reference_t;
    using ::ranges::difference_type_t;
    using ::ranges::iter_common_reference_t;
    using ::ranges::iterator_category_t;
    using ::ranges::iterator_t;
    using ::ranges::reference_t;
    using ::ranges::rvalue_reference_t;
    using ::ranges::sentinel_t;
    using ::ranges::value_type_t;


    template <typename R>
    inline constexpr auto disable_sized_range_v = disable_sized_range<R>::value;

    template <typename S, typename I>
    inline constexpr auto disable_sized_sentinel_v = disable_sized_sentinel<S, I>::value;

    template <typename T>
    inline constexpr auto view_predicate_v_ = view_predicate_<T>::value;

    template <typename T>
    struct remove_cvref
    {
      using type = std::remove_cv_t<std::remove_reference_t<T>>;
    };
    template <typename T>
    using remove_cvref_t = typename remove_cvref<T>::type;


  } // namespace detail


  inline namespace core_language
  {

    // clang-format off

    // Same
    template <typename T, typename U>
    concept bool Same = std::is_same_v<T, U>;


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
      Same<detail::common_reference_t<T, U>, detail::common_reference_t<U, T>> &&
      ConvertibleTo<T, detail::common_reference_t<T, U>> &&
      ConvertibleTo<U, detail::common_reference_t<T, U>>;


    // Common
    template <typename T, typename U>
    concept bool Common =
      Same<std::common_type_t<T, U>, std::common_type_t<U, T>> &&
      ConvertibleTo<T, std::common_type_t<T, U>> &&
      ConvertibleTo<U, std::common_type_t<T, U>> &&
      CommonReference<std::add_lvalue_reference_t<const T>, std::add_lvalue_reference_t<const U>> &&
      CommonReference<
        std::add_lvalue_reference_t<std::common_type_t<T, U>>,
        detail::common_reference_t<std::add_lvalue_reference_t<const T>, std::add_lvalue_reference_t<const U>>>;


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
        { lhs = std::forward<Rhs>(rhs) } -> Same<Lhs>&&;
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
      Movable<detail::remove_cvref_t<B>> &&
      ConvertibleTo<const std::remove_reference_t<B>&, bool> &&
      requires( const std::remove_reference_t<B>& b1,
                const std::remove_reference_t<B>& b2,
                const bool a) {
        { !b1 }       ->  ConvertibleTo<bool>&&;
        { b1 && a }   ->  Same<bool>&&;
        { b1 || a }   ->  Same<bool>&&;
        { b1 && b2 }  ->  Same<bool>&&;
        { a && b2 }   ->  Same<bool>&&;
        { b1 || b2 }  ->  Same<bool>&&;
        { a || b2 }   ->  Same<bool>&&;
        { b1 == b2 }  ->  ConvertibleTo<bool>&&;
        { b1 == a }   ->  ConvertibleTo<bool>&&;
        { a == b2 }   ->  ConvertibleTo<bool>&&;
        { b1 != b2 }  ->  ConvertibleTo<bool>&&;
        { b1 != a }   ->  ConvertibleTo<bool>&&;
        { a != b2 }   ->  ConvertibleTo<bool>&&;
      };


    // __WeaklyEqualityComparableWith
    template <typename T, typename U>
    concept bool __WeaklyEqualityComparableWith = // exposition only
      requires( const std::remove_reference_t<T>& t,
                const std::remove_reference_t<U>& u) {
        {t == u} -> Boolean&&;
        {t != u} -> Boolean&&;
        {u == t} -> Boolean&&;
        {u != t} -> Boolean&&;
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
        detail::common_reference_t<
          const std::remove_reference_t<T>&,
          const std::remove_reference_t<U>&>> &&
      __WeaklyEqualityComparableWith<T, U>;


    // StrictTotallyOrdered
    template <typename T>
    concept bool StrictTotallyOrdered =
      EqualityComparable<T> &&
      requires( const std::remove_reference_t<T>& a,
                const std::remove_reference_t<T>& b) {
        { a < b }   -> Boolean&&;
        { a > b }   -> Boolean&&;
        { a <= b }  -> Boolean&&;
        { a >= b }  -> Boolean&&;
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
        detail::common_reference_t<
          const std::remove_reference_t<T>&,
          const std::remove_reference_t<U>&>> &&
      EqualityComparableWith<T, U> &&
      requires( const std::remove_reference_t<T>& t,
                const std::remove_reference_t<U>& u) {
        { t < u }   -> Boolean&&;
        { t > u }   -> Boolean&&;
        { t <= u }  -> Boolean&&;
        { t >= u }  -> Boolean&&;
        { u < t }   -> Boolean&&;
        { u > t }   -> Boolean&&;
        { u <= t }  -> Boolean&&;
        { u >= t }  -> Boolean&&;
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
        detail::common_reference_t<
          const std::remove_reference_t<T>&,
          const std::remove_reference_t<U>&>,
        detail::common_reference_t<
          const std::remove_reference_t<T>&,
          const std::remove_reference_t<U>&>> &&
      Predicate<R, T, U> &&
      Predicate<R, U, T>;


    // StrictWeakOrder
    template <typename R, typename T, typename U>
    concept bool StrictWeakOrder = Relation<R, T, U>;

    // clang-format on

  } // namespace callable


  inline namespace random_number_generator
  {

    // clang-format off

    template <typename G>
    concept bool UniformRandomNumberGenerator =
      Invocable<G&> &&
      UnsignedIntegral<std::invoke_result_t<G&>> &&
      requires {
        { G::min() } -> Same<std::invoke_result_t<G&>>&&;
        { G::max() } -> Same<std::invoke_result_t<G&>>&&;
      };

    // clang-format on

  } // namespace random_number_generator


  inline namespace iterator
  {

    // clang-format off

    // Readable
    template <typename In>
    concept bool Readable =
      requires {
        typename detail::value_type_t<In>;
        typename detail::reference_t<In>;
        typename detail::rvalue_reference_t<In>;
      } &&
      CommonReference<detail::reference_t<In>&&, detail::value_type_t<In>&> &&
      CommonReference<detail::reference_t<In>&&, detail::rvalue_reference_t<In>&&> &&
      CommonReference<detail::rvalue_reference_t<In>&&, const detail::value_type_t<In>&>;


    // Writable
    template <typename Out, typename T>
    concept bool Writable =
      requires {
        typename detail::reference_t<Out>;
      } &&
      requires(Out&& o, T&& t) {
        *o = std::forward<T>(t);
        *std::forward<Out>(o) = std::forward<T>(t);
        const_cast<const detail::reference_t<Out>&&>(*o) = std::forward<T>(t);
        const_cast<const detail::reference_t<Out>&&>(*std::forward<Out>(o)) = std::forward<T>(t);
        /* none of the four expressions above are required to be equality-preserving */
      };


    // WeaklyIncrementable
    template <typename I>
    concept bool WeaklyIncrementable =
      Semiregular<I> &&
      requires {
        typename detail::difference_type_t<I>;
      } &&
      SignedIntegral<detail::difference_type_t<I>> &&
      requires(I i) {
        { ++i } ->  Same<I>&;  /* not required to be equality preserving */
        { i++ }                /* not required to be equality preserving */
      };


    // Incrementable
    template <typename I>
    concept bool Incrementable =
      Regular<I> &&
      WeaklyIncrementable<I> &&
      requires(I i) {
        { i++ } ->  Same<I>&&;
      };


    // Iterator
    template <typename I>
    concept bool Iterator =
      WeaklyIncrementable<I> &&
      requires(I i) {
        { *i }  ->  auto&&; // Requires: i is dereferenceable
      };


    // Sentinel
    template <typename S, typename I>
    concept bool Sentinel =
      Semiregular<S> &&
      Iterator<I> &&
      __WeaklyEqualityComparableWith<S, I>;


    // SizedSentinel
    template <typename S, typename I>
    concept bool SizedSentinel =
      Sentinel<S, I> &&
      !detail::disable_sized_sentinel_v<std::remove_cv_t<S>, std::remove_cv_t<I>> &&
      requires(const I& i, const S& s) {
        { s - i } ->  Same<detail::difference_type_t<I>>&&;
        { i - s } ->  Same<detail::difference_type_t<I>>&&;
      };


    // InputIterator
    template <typename I>
    concept bool InputIterator =
      Iterator<I> &&
      Readable<I> &&
      requires {
        typename detail::iterator_category_t<I>;
      } &&
      DerivedFrom<detail::iterator_category_t<I>, std::input_iterator_tag>;


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
      DerivedFrom<detail::iterator_category_t<I>, std::forward_iterator_tag> &&
      Incrementable<I> &&
      Sentinel<I, I>;


    // BidirectionalIterator
    template <typename I>
    concept bool BidirectionalIterator =
      ForwardIterator<I> &&
      DerivedFrom<detail::iterator_category_t<I>, std::bidirectional_iterator_tag> &&
      requires(I i) {
        { --i } ->  Same<I>&;
        { i-- } ->  Same<I>&&;
      };


    // RandomAccessIterator
    template <typename I>
    concept bool RandomAccessIterator =
      BidirectionalIterator<I> &&
      DerivedFrom<detail::iterator_category_t<I>, std::random_access_iterator_tag> &&
      StrictTotallyOrdered<I> &&
      SizedSentinel<I, I> &&
      requires(I i, const I j, const detail::difference_type_t<I> n) {
        { i += n }  ->  Same<I>&;
        { j + n }   ->  Same<I>&&;
        { n + j }   ->  Same<I>&&;
        { i -= n }  ->  Same<I>&;
        { j - n }   ->  Same<I>&&;
        { j[n] }    ->  Same<detail::reference_t<I>>;
      };

    // clang-format on

  } // namespace iterator


  inline namespace indirect_callable
  {
    // clang-format off

    // IndirectUnaryInvocable
    template <typename F, typename I>
    concept bool IndirectUnaryInvocable =
      Readable<I> &&
      CopyConstructible<F> &&
      Invocable<F&, detail::value_type_t<I>&> &&
      Invocable<F&, detail::reference_t<I>> &&
      Invocable<F&, detail::iter_common_reference_t<I>> &&
      CommonReference<
        std::invoke_result_t<F&, detail::value_type_t<I>&>,
        std::invoke_result_t<F&, detail::reference_t<I>&&>>;


    // IndirectRegularUnaryInvocable
    template <typename F, typename I>
    concept bool IndirectRegularUnaryInvocable =
      Readable<I> &&
      CopyConstructible<F> &&
      RegularInvocable<F&, detail::value_type_t<I>&> &&
      RegularInvocable<F&, detail::reference_t<I>> &&
      RegularInvocable<F&, detail::iter_common_reference_t<I>> &&
      CommonReference<
        std::invoke_result_t<F&, detail::value_type_t<I>&>,
        std::invoke_result_t<F&, detail::reference_t<I>&&>>;


    // IndirectUnaryPredicate
    template <typename F, typename I>
    concept bool IndirectUnaryPredicate =
      Readable<I> &&
      CopyConstructible<F> &&
      Predicate<F&, detail::value_type_t<I>&> &&
      Predicate<F&, detail::reference_t<I>> &&
      Predicate<F&, detail::iter_common_reference_t<I>>;


    // IndirectRelation
    template <typename F, typename I1, typename I2 = I1>
    concept bool IndirectRelation =
      Readable<I1> &&
      Readable<I2> &&
      CopyConstructible<F> &&
      Relation<F&, detail::value_type_t<I1>&, detail::value_type_t<I2>&> &&
      Relation<F&, detail::value_type_t<I1>&, detail::reference_t<I2>> &&
      Relation<F&, detail::reference_t<I1>, detail::value_type_t<I2>&> &&
      Relation<F&, detail::reference_t<I1>, detail::reference_t<I2>> &&
      Relation<F&, detail::iter_common_reference_t<I1>, detail::iter_common_reference_t<I2>>;


    // IndirectStrictWeakOrder
    template <typename F, typename I1, typename I2 = I1>
    concept bool IndirectStrictWeakOrder =
      Readable<I1> &&
      Readable<I2> &&
      CopyConstructible<F> &&
      StrictWeakOrder<F&, detail::value_type_t<I1>&, detail::value_type_t<I2>&> &&
      StrictWeakOrder<F&, detail::value_type_t<I1>&, detail::reference_t<I2>> &&
      StrictWeakOrder<F&, detail::reference_t<I1>, detail::value_type_t<I2>&> &&
      StrictWeakOrder<F&, detail::reference_t<I1>, detail::reference_t<I2>> &&
      StrictWeakOrder<F&, detail::iter_common_reference_t<I1>, detail::iter_common_reference_t<I2>>;

    // clang-format on

  } // namespace indirect_callable


  namespace common_algorithm
  {

    // clang-format off
    
    // IndirectlyMovable
    template <typename In, typename Out>
    concept bool IndirectlyMovable =
      Readable<In> &&
      Writable<Out, detail::rvalue_reference_t<In>>;


    // IndirectlyMovableStorable
    template <typename In, typename Out>
    concept bool IndirectlyMovableStorable =
      IndirectlyMovable<In, Out> &&
      Writable<Out, detail::value_type_t<In>> &&
      Movable<detail::value_type_t<In>> &&
      Constructible<detail::value_type_t<In>, detail::rvalue_reference_t<In>> &&
      Assignable<detail::value_type_t<In>&, detail::rvalue_reference_t<In>>;


    // IndirectlyCopyable
    template <typename In, typename Out>
    concept bool IndirectlyCopyable =
      Readable<In> &&
      Writable<Out, detail::reference_t<In>>;


    // IndirectlyCopyableStorable
    template <typename In, typename Out>
    concept bool IndirectlyCopyableStorable =
      IndirectlyCopyable<In, Out> &&
      Writable<Out, const detail::value_type_t<In>&> &&
      Copyable<detail::value_type_t<In>> &&
      Constructible<detail::value_type_t<In>, detail::reference_t<In>> &&
      Assignable<detail::value_type_t<In>&, detail::reference_t<In>>;

    
    // IndirectlySwappable
    template <typename I1, typename I2 = I1>
    concept bool IndirectlySwappable =
      Readable<I1> && Readable<I2> &&
      requires(I1&& i1, I2&& i2) {
        detail::iter_swap(std::forward<I1>(i1), std::forward<I2>(i2));
        detail::iter_swap(std::forward<I2>(i2), std::forward<I1>(i1));
        detail::iter_swap(std::forward<I1>(i1), std::forward<I1>(i1));
        detail::iter_swap(std::forward<I2>(i2), std::forward<I2>(i2));
      };

    
    // IndirectlyComparable
    template <typename I1, typename I2, typename R = detail::equal_to,
              typename P1 = detail::ident, typename P2 = detail::ident>
    concept bool IndirectlyComparable =
      IndirectRelation<R, detail::projected<I1, P1>, detail::projected<I2, P2>>;


    // Permutable
    template <typename I>
    concept bool Permutable =
      ForwardIterator<I> &&
      IndirectlyMovableStorable<I, I> &&
      IndirectlySwappable<I, I>;
    

    // Mergeable
    template <typename I1, typename I2, typename Out, typename R = detail::less,
              typename P1 = detail::ident, typename P2 = detail::ident>
    concept bool Mergeable =
      InputIterator<I1> &&
      InputIterator<I2> &&
      WeaklyIncrementable<Out> &&
      IndirectlyCopyable<I1, Out> &&
      IndirectlyCopyable<I2, Out> &&
      IndirectStrictWeakOrder<R, detail::projected<I1, P1>, detail::projected<I2, P2>>;


    // Sortable
    template <typename I, typename R = detail::less, typename P = detail::ident>
    concept bool Sortable =
      Permutable<I> &&
      IndirectStrictWeakOrder<R, detail::projected<I, P>>;

    // clang-format on

  } // namespace common_algorithm


  inline namespace ranges
  {

    // clang-format off

    // Range
    template <typename T>
    concept bool Range =
      requires(T&& t) {
        detail::begin(t); /* not necessarily equality-preserving (see below) */
        detail::end(t);
      };


    // SizedRange
    template <typename T>
    concept bool SizedRange =
      Range<T> &&
      !detail::disable_sized_range_v<std::remove_cv_t<std::remove_reference_t<T>>> &&
      requires(T& t) {
        { detail::size(t) } -> ConvertibleTo<detail::difference_type_t<detail::iterator_t<T>>>;
      };


    // View
    template <typename T>
    concept bool View =
      Range<T> &&
      Semiregular<T> && 
      detail::view_predicate_v_<T>;


    // BoundedRange
    template <typename T>
    concept bool BoundedRange =
      Range<T> &&
      Same<detail::iterator_t<T>, detail::sentinel_t<T>>;


    // InputRange
    template <typename T>
    concept bool InputRange =
      Range<T> &&
      InputIterator<detail::iterator_t<T>>;


    // OutputRange
    template <typename R, typename T>
    concept bool OutputRange =
      Range<R> &&
      OutputIterator<detail::iterator_t<R>, T>;


    // ForwardRange
    template <typename T>
    concept bool ForwardRange =
      InputRange<T> &&
      ForwardIterator<detail::iterator_t<T>>;


    // BidirectionalRange
    template <typename T>
    concept bool BidirectionalRange =
      ForwardRange<T> &&
      BidirectionalIterator<detail::iterator_t<T>>;


    // RandomAccessRange
    template <typename T>
    concept bool RandomAccessRange =
      BidirectionalRange<T> &&
      RandomAccessIterator<detail::iterator_t<T>>;

    // clang-format on

  } // namespace ranges

} // namespace mln::core::concepts::stl
