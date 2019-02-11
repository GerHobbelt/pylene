ND Points
#########

Include :file:`<mln/core/point.hpp>`

.. cpp:namespace:: mln

.. image:: /figures/core/ndpoint.svg
    :width: 100%

Class hierarchy diagrams for ndpoints. They all implement the :cpp:class:`__PointInterface` but differ in the storage and the compile-time number of dimension:

* `ndpoint` provide points type with value semantics (they own the coordinates array). `Point` uses a dynamic array while `point*N*d` use a static array storage
* `nbpointref` provide points type with reference semantics (they borrow the coordinates array). `PointRef` and `ConstPointRef` supports dynamic array bound while `pointNd_ref` and `const_pointNd_ref` provide compile-time bound checking.


.. cpp:class:: template <int ndim, class T> ndpoint

    `ndpoint` represents a n-dimensional points (coordinates) over a grid. The number of dimensions can be known at
    compile time or dynamic ``ndim = -1``. It is a container (own the value).

    .. cpp:function:: ndpoint() = default

        Default constructor. 

    .. cpp:function:: template <class P> ndpoint(const P& other)

        Converting constructor from any point implementing the :cpp:class:`__PointInterface`. This overload is enabled
        only if `P` is compatible with ``ndpoint`` i.e. if ``P::value_type`` is convertible to ``T`` and ``ndim == (-1
        || other.ndim)``.

    .. cpp:function:: ndpoint(int dim)

        Construction with the number of dimensions given dynamically. Only available when ``ndim == -1``.

.. cpp:class:: template <int ndim, class T> ndpointref

    `ndpointref` represents a n-dimensional points (coordinates) over a grid. The number of dimensions can be known at
    compile time or dynamic ``ndim = -1``. It has a reference semantic and should be used as a function parameter only.


    1. .. cpp:function:: template <class P> ndpointref(P& other)
    2. .. cpp:function:: template <class P> ndpointref(const P& other)

        Converting constructor from any point implementing the :cpp:class:`__PointInterface`. This overload is enabled only if `P` is compatible with ``ndpointref``, i.e. if  ``P::value_type*`` is convertible to ``T*`` and ``ndim == (-1 || other.ndim)``.
    
    .. cpp:function:: ndpoint(int dim, T* coordinates)

        Constructor from an array in the form of a pair (pointer, size).

Aliases
=======

.. cpp:type:: Point = ndpoint<-1, int>
.. cpp:type:: point1d = ndpoint<1, int>
.. cpp:type:: point2d = ndpoint<2, int>
.. cpp:type:: point3d = ndpoint<3, int>
.. cpp:type:: PointRef =   ndpointref<-1, int>
.. cpp:type:: point1d_ref = ndpointref<1, int>
.. cpp:type:: point2d_ref = ndpointref<2, int>
.. cpp:type:: point3d_ref = ndpointref<3, int>
.. cpp:type:: ConstPointRef = ndpointref<-1, const int>
.. cpp:type:: const_point1d_ref = ndpointref<1, const int>
.. cpp:type:: const_point2d_ref = ndpointref<2, const int>
.. cpp:type:: const_point3d_ref = ndpointref<3, const int>

Read-Only Point Interface
=========================

.. cpp:class:: template <int ndim, class T> __PointInterface

    .. cpp:type:: value_type = T

    .. cpp:function:: __PointInterface(std::initializer_list<T> coordinates)

        Construction from a list of coordinates

    .. cpp:function:: const T* data() const

        Return a pointer to the coordinates array.

    .. cpp:function:: int dim() const

        return the number of dimension

    .. cpp:function:: T operator[] (int k) const

        Return the coordinate in the k-th dimension.

    .. cpp:function:: T x()
        
        Get the x-coordinate (available when ``dim() >= 1``)

    .. cpp:function:: T y()
        
        Get the y-coordinate (available when ``dim() >= 2``)

    .. cpp:function:: T z()
        
        Get the z-coordinate (available when ``dim() >= 3``)

    


.. rubric:: Point Interopability

Any two points p₁ and p₂ of types P₁ and P₂ are said *compatible* if their value types are *compatible* and if they have the same number of dimensions (``p₁.dim() == p₂.dim()``). They thus share a `common_reference` (:cpp:concept:`std::CommonReference˂P₁,P₂˃ <std::CommonReference>`).

* Conversion 
    Two *compatible* points are convertible to each other (:cpp:concept:`std::Convertible`).

    Example::

        mln::point2d p1 = {x, y};
        mln::ndpoint<2, long> p2 = p1; // Ok (int to long conversion)
        mln::Point p3 = p1;            // Ok (static to dynamic conversion)
        mln::point2d p4 = p3;          // Ok (dynamic to static conversion with run-time dim assertion)


* Comparison
    Two *compatible* points are comparable and totally ordered (:cpp:concept:`std::totally_ordered`) using the lexicographical ordering. 


* Arithmetics
    Two *compatible* points are :cpp:concept:`Addable` using usual arithmetic rules.


Mutable Point Interface
=======================

.. cpp:class:: template <int ndim, class T> __MutablePointInterface

    .. cpp:type:: value_type = T

    .. cpp:function:: __MutablePointInterface(std::initializer_list<T> coordinates)

        Construction from a list of coordinates

    .. cpp:function:: T* data()

        Return a pointer to the mutable coordinates array.

    .. cpp:function:: int dim() const

        return the number of dimension

    .. cpp:function:: T& operator[] (int k)

        Return the coordinate in the k-th dimension.

    .. cpp:function:: T& x()
        
        Get the x-coordinate (available when ``dim() >= 1``)

    .. cpp:function:: T& y()
        
        Get the y-coordinate (available when ``dim() >= 2``)

    .. cpp:function:: T& z()
        
        Get the z-coordinate (available when ``dim() >= 3``)
        
