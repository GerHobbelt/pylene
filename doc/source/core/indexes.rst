=======
Indexes
=======


Lowest level way to access a value inside an image.



.. contents::
   :local:


Concepts
--------


.. cpp:namespace:: mln::concepts


.. _concept-indexes-Index:

Index
#####

Let `Idx` be a type that models :cpp:concept:`Index (details) <Index>`.

Then `Idx` also models :cpp:concept:`Integral (stl) <stl::Integral>`.

.. _concept-indexes-Index-expressions:
.. rubric:: Expressions

All expressions valid with C++ integral types are also valid.




Predefined indexes
------------------

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




Concepts detail
---------------

.. toctree::
   
   concepts/indexes
