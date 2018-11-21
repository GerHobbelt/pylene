=================
Indexes (details)
=================


.. contents:: Table of Contents
    :local:



.. cpp:namespace:: mln::concepts


Index
#####

.. cpp:concept:: template <typename Idx> Index

    #. Refines the :cpp:concept:`Integral <stl::Integral>` concept.

    **Valid Expressions**

    All expressions valid with the following C++ integral types :
        - `bool`
        - `char`
        - `unsigned char`
        - `char16_t`
        - `char32_t`
        - `wchar_t`
        - `unsigned wchar_t`
        - `short`
        - `unsigned short`
        - `int`
        - `unsigned int`
        - `long`
        - `unsigned long`
        - `long long`
        - `unsigned long long`
        - or any implementation-defined extended integer types, including any signed, unsigned, and cv-qualified variants.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/indexes.hpp
       :language: cpp
       :lines: 11-12
