================
Pixels (details)
================


.. contents:: Table of Contents
    :local:



.. cpp:namespace:: mln::concepts


Pixel
#####

.. cpp:concept:: template <typename Pix> Pixel

    #. Refines the :cpp:concept:`Regular (stl) <stl::Regular>` concept.
    #. ``Pix::value_type`` models the :cpp:concept:`Value <Value>` concept, is non-const and is not a reference.
    #. ``Pix::point_type`` models the :cpp:concept:`Point <Point>` concept.
    #. ``Pix`` provide methods ``value`` and ``point``.

    **Notation**

    .. cpp:type::   value_type = Pix::value_type
    .. cpp:type::   point_type = Pix::point_type
    .. cpp:type::   image_type = Pix::image_type
    .. cpp:type::   reference = Pix::reference
    .. cpp:var::    Pix pix
    .. cpp:var::    Pix pix_cpy
    .. cpp:var::    const Pix cpix
    .. cpp:var::    const Pix t
    .. cpp:var::    const Pix u

    **Valid Expressions**

    - :cpp:expr:`pix()` returns an instance of :cpp:expr:`Pix`.
    - :cpp:expr:`cpix()` returns an instance of :cpp:expr:`const Pix`.

    - :cpp:expr:`pix_cpy(pix)` returns an instance of :cpp:expr:`Pix`.
    - :cpp:expr:`pix_cpy(cpix)` returns an instance of :cpp:expr:`Pix`.
    - :cpp:expr:`pix_cpy(move(pix))` returns an instance of :cpp:expr:`Pix`.

    - :cpp:expr:`pix_cpy = pix` returns an instance of :cpp:expr:`Pix&`.
    - :cpp:expr:`pix_cpy = cpix` returns an instance of :cpp:expr:`Pix&`.
    - :cpp:expr:`pix_cpy = move(pix)` returns an instance of :cpp:expr:`Pix&`.

    - :cpp:expr:`t == u` return-type models :cpp:concept:`template<typename B> Boolean`.
    - :cpp:expr:`t != u` return-type models :cpp:concept:`template<typename B> Boolean`.
    - :cpp:expr:`u == t` return-type models :cpp:concept:`template<typename B> Boolean`.
    - :cpp:expr:`u != t` return-type models :cpp:concept:`template<typename B> Boolean`.

    - :cpp:expr:`cpix.value()` returns :cpp:expr:`const reference`.
    - :cpp:expr:`cpix.point()` returns :cpp:expr:`const point_type`.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/pixels.hpp
       :language: cpp
       :lines: 17-32
