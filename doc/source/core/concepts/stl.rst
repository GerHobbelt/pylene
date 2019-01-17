=============
STL (details)
=============



.. contents:: Table of Contents
    :local:



.. cpp:namespace:: mln::concepts::stl



Core language
-------------



Same
####

.. cpp:concept:: template <typename T, typename U> Same

    - cf. `reference documentation for Same <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Same>`_


DerivedFrom
###########

.. cpp:concept:: template <typename Derived, typename Base> DerivedFrom

    - cf. `reference documentation for DerivedFrom <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/DerivedFrom>`_


ConvertibleTo
#############

.. cpp:concept:: template <typename From, typename To> ConvertibleTo

    - cf. `reference documentation for ConvertibleTo <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/ConvertibleTo>`_


CommonReference
###############

.. cpp:concept:: template <typename T, typename U> CommonReference

    - cf. `reference documentation for CommonReference <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/CommonReference>`_


Common
######

.. cpp:concept:: template<typename T, typename U> Common

    - cf. `reference documentation for Common <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Common>`_


Integral
########

.. cpp:concept:: template <typename T> Integral

    - cf. `reference documentation for Integral <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Integral>`_


SignedIntegral
##############

.. cpp:concept:: template <typename T> SignedIntegral

    - cf. `reference documentation for SignedIntegral <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/SignedIntegral>`_


UnsignedIntegral
################

.. cpp:concept:: template <typename T> UnsignedIntegral

    - cf. `reference documentation for UnsignedIntegral <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/UnsignedIntegral>`_


Assignable
##########

.. cpp:concept:: template <typename Lhs, typename Rhs> Assignable

    - cf. `reference documentation for Assignable <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Assignable>`_


Swappable
#########

.. cpp:concept:: template <typename T> Swappable

    - cf. `reference documentation for Swappable <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Swappable>`_


SwappableWith
#############

.. cpp:concept:: template <typename T, typename U> SwappableWith

    - cf. `reference documentation for SwappableWith <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Swappable>`_


Destructible
############

.. cpp:concept:: template <typename T> Destructible

    - cf. `reference documentation for Destructible <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Destructible>`_


Constructible
#############

.. cpp:concept:: template <typename T, typename... Args> Constructible

    - cf. `reference documentation for Constructible <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Constructible>`_


DefaultConstructible
####################

.. cpp:concept:: template <typename T> DefaultConstructible

    - cf. `reference documentation for DefaultConstructible <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/DefaultConstructible>`_


MoveConstructible
#################

.. cpp:concept:: template <typename T> MoveConstructible

    - cf. `reference documentation for MoveConstructible <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/MoveConstructible>`_


CopyConstructible
#################

.. cpp:concept:: template <typename T> CopyConstructible

    - cf. `reference documentation for CopyConstructible <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/CopyConstructible>`_



Object
------



Movable
#######

.. cpp:concept:: template <typename T> Movable

    - cf. `reference documentation for Movable <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Movable>`_


Copyable
########

.. cpp:concept:: template <typename T> Copyable

    - cf. `reference documentation for Copyable <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Copyable>`_


Semiregular
###########

.. cpp:concept:: template <typename T> Semiregular

    - cf. `reference documentation for Semiregular <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Semiregular>`_


Regular
#######

.. cpp:concept:: template <typename T> Regular

    - cf. `reference documentation for Regular <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Regular>`_



Comparison
----------



Boolean
#######

.. cpp:concept:: template <typename B> Boolean

    - cf. `reference documentation for Boolean <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Boolean>`_


\__WeaklyEqualityComparableWith
###############################

.. cpp:concept:: template <typename T, typename U> __WeaklyEqualityComparableWith

    - cf. `reference documentation for __WeaklyEqualityComparableWith <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/WeaklyEqualityComparableWith>`_


EqualityComparable
##################

.. cpp:concept:: template <typename T> EqualityComparable

    - cf. `reference documentation for EqualityComparable <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/EqualityComparable>`_


EqualityComparableWith
######################

.. cpp:concept:: template <typename T, typename U> EqualityComparableWith

    - cf. `reference documentation for EqualityComparableWith <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/EqualityComparable>`_


StrictTotallyOrdered
####################

.. cpp:concept:: template <typename T> StrictTotallyOrdered

    - cf. `reference documentation for StrictTotallyOrdered <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/StrictTotallyOrdered>`_


StrictTotallyOrderedWith
########################

.. cpp:concept:: template <typename T, typename U> StrictTotallyOrderedWith

    - cf. `reference documentation for StrictTotallyOrderedWith <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/StrictTotallyOrdered>`_



Callable
--------



Invocable
#########

.. cpp:concept:: template <typename F, typename... Args> Invocable

    - cf. `reference documentation for Invocable <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Invocable>`_


RegularInvocable
################

.. cpp:concept:: template <typename F, typename... Args> RegularInvocable

    - cf. `reference documentation for RegularInvocable <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Invocable>`_


Predicate
#########

.. cpp:concept:: template <typename F, typename... Args> Predicate

    - cf. `reference documentation for Predicate <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Predicate>`_


Relation
########

.. cpp:concept:: template <typename R, typename T, typename U> Relation

    - cf. `reference documentation for Relation <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/Relation>`_


StrictWeakOrder
###############

.. cpp:concept:: template <typename R, typename T, typename U> StrictWeakOrder

    - cf. `reference documentation for StrictWeakOrder <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/StrictWeakOrder>`_



Random Number Generator
-----------------------



UniformRandomNumberGenerator
############################

.. cpp:concept:: template <class G> UniformRandomNumberGenerator

    - cf. `reference documentation for UniformRandomNumberGenerator <https://en.cppreference.com/w/cpp/experimental/ranges/concepts/UniformRandomNumberGenerator>`_



Iterator
--------



Readable
########

.. cpp:concept:: template <typename In> Readable

    - cf. `reference documentation for Readable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/Readable>`_


Writable
########

.. cpp:concept:: template <typename Out, typename T> Writable

    - cf. `reference documentation for Writable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/Writable>`_


WeaklyIncrementable
###################

.. cpp:concept:: template <typename I> WeaklyIncrementable

    - cf. `reference documentation for WeaklyIncrementable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/WeaklyIncrementable>`_


Incrementable
#############

.. cpp:concept:: template <typename I> Incrementable

    - cf. `reference documentation for Incrementable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/Incrementable>`_


Iterator
########

.. cpp:concept:: template <typename I> Iterator

    - cf. `reference documentation for Iterator <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/Iterator>`_


Sentinel
########

.. cpp:concept:: template <typename S, typename I> Sentinel

    - cf. `reference documentation for Sentinel <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/Sentinel>`_


SizedSentinel
#############

.. cpp:concept:: template <typename S, typename I> SizedSentinel

    - cf. `reference documentation for SizedSentinel <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/SizedSentinel>`_


InputIterator
#############

.. cpp:concept:: template <typename I> InputIterator

    - cf. `reference documentation for InputIterator <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/InputIterator>`_


OutputIterator
##############

.. cpp:concept:: template <typename I, typename T> OutputIterator

    - cf. `reference documentation for OutputIterator <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/OutputIterator>`_


ForwardIterator
###############

.. cpp:concept:: template <typename I> ForwardIterator

    - cf. `reference documentation for ForwardIterator <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/ForwardIterator>`_


BidirectionalIterator
#####################

.. cpp:concept:: template <typename I> BidirectionalIterator

    - cf. `reference documentation for BidirectionalIterator <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/BidirectionalIterator>`_


RandomAccessIterator
####################

.. cpp:concept:: template <typename I> RandomAccessIterator

    - cf. `reference documentation for RandomAccessIterator <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/RandomAccessIterator>`_



Indirect Callable
-----------------



IndirectUnaryInvocable
######################

.. cpp:concept:: template <typename F, typename I> IndirectUnaryInvocable

    - cf. `reference documentation for IndirectUnaryInvocable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/IndirectUnaryInvocable>`_


IndirectRegularUnaryInvocable
#############################

.. cpp:concept:: template <typename F, typename I> IndirectRegularUnaryInvocable

    - cf. `reference documentation for IndirectRegularUnaryInvocable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/IndirectUnaryInvocable>`_


IndirectUnaryPredicate
######################

.. cpp:concept:: template <typename F, typename I> IndirectUnaryPredicate

    - cf. `reference documentation for IndirectUnaryPredicate <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/IndirectUnaryPredicate>`_


IndirectRelation
################

.. cpp:concept:: template <typename F, typename I1, typename I2 = I1> IndirectRelation

    - cf. `reference documentation for IndirectRelation <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/IndirectRelation>`_


IndirectStrictWeakOrder
#######################

.. cpp:concept:: template <typename F, typename I1, typename I2 = I1> IndirectStrictWeakOrder

    - cf. `reference documentation for IndirectStrictWeakOrder <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/IndirectStrictWeakOrder>`_



Common algorithm requirements
-----------------------------



IndirectlyMovable
#################

.. cpp:concept:: template <typename In, typename Out> IndirectlyMovable

    - cf. `reference documentation for IndirectlyMovable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/IndirectlyMovable>`_


IndirectlyMovableStorable
#########################

.. cpp:concept:: template <typename In, typename Out> IndirectlyMovableStorable

    - cf. `reference documentation for IndirectlyMovableStorable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/IndirectlyMovableStorable>`_


IndirectlyCopyable
##################

.. cpp:concept:: template <typename In, typename Out> IndirectlyCopyable

    - cf. `reference documentation for IndirectlyCopyable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/IndirectlyCopyable>`_


IndirectlyCopyableStorable
##########################

.. cpp:concept:: template <typename In, typename Out> IndirectlyCopyableStorable

    - cf. `reference documentation for IndirectlyCopyableStorable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/IndirectlyCopyableStorable>`_


IndirectlySwappable
###################

.. cpp:concept:: template <typename I1, typename I2 = I1> IndirectlySwappable

    - cf. `reference documentation for IndirectlySwappable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/IndirectlySwappable>`_


IndirectlyComparable
####################

.. cpp:concept:: template <typename I1, typename I2, typename R = equal_to, typename P1 = identity, typename P2 = identity> IndirectlyComparable 

    - cf. `reference documentation for IndirectlyComparable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/IndirectlyComparable>`_


Permutable
##########

.. cpp:concept:: template <typename I> Permutable

    - cf. `reference documentation for Permutable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/Permutable>`_


Mergeable
#########

.. cpp:concept:: template <typename I1, typename I2, typename Out, typename R = less, typename P1 = identity, typename P2 = identity> Mergeable

    - cf. `reference documentation for Mergeable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/Mergeable>`_


Sortable
########

.. cpp:concept:: template <typename I, typename R = less, typename P = identity> Sortable

    - cf. `reference documentation for Sortable <https://en.cppreference.com/w/cpp/experimental/ranges/iterator/Sortable>`_



Range
-----



Range
#####

.. cpp:concept:: template <typename T> Range

    - cf. `reference documentation for Range <https://en.cppreference.com/w/cpp/experimental/ranges/range/Range>`_


SizedRange
##########

.. cpp:concept:: template <typename T> SizedRange

    - cf. `reference documentation for SizedRange <https://en.cppreference.com/w/cpp/experimental/ranges/range/SizedRange>`_


View
####

.. cpp:concept:: template <typename T> View

    - cf. `reference documentation for View <https://en.cppreference.com/w/cpp/experimental/ranges/range/View>`_


BoundedRange
############

.. cpp:concept:: template <typename T> BoundedRange

    - cf. `reference documentation for BoundedRange <https://en.cppreference.com/w/cpp/experimental/ranges/range/BoundedRange>`_


InputRange
##########

.. cpp:concept:: template <typename T> InputRange

    - cf. `reference documentation for InputRange <https://en.cppreference.com/w/cpp/experimental/ranges/range/InputRange>`_


OutputRange
###########

.. cpp:concept:: template <typename R, typename T> OutputRange

    - cf. `reference documentation for OutputRange <https://en.cppreference.com/w/cpp/experimental/ranges/range/OutputRange>`_


ForwardRange
############

.. cpp:concept:: template <typename T> ForwardRange

    - cf. `reference documentation for ForwardRange <https://en.cppreference.com/w/cpp/experimental/ranges/range/ForwardRange>`_


BidirectionalRange
##################

.. cpp:concept:: template <typename T> BidirectionalRange

    - cf. `reference documentation for BidirectionalRange <https://en.cppreference.com/w/cpp/experimental/ranges/range/BidirectionalRange>`_


RandomAccessRange
#################

.. cpp:concept:: template <typename T> RandomAccessRange

    - cf. `reference documentation for RandomAccessRange <https://en.cppreference.com/w/cpp/experimental/ranges/range/RandomAccessRange>`_
