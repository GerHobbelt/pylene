Boxes
######

Include :file:`<mln/core/box.hpp>`

.. cpp:namespace:: mln 

.. image:: /figures/core/box.svg
    :width: 100%

Class hierarchy diagrams for boxes. They all implement the :cpp:class:`__BoxInterface` but differ in the storage and the compile-time number of dimension:

* `ndbox` provide box types with value semantics (they own the coordinates array). `Box` uses a dynamic array while `box*N*d` use a static array storage
* `nbboxref` provide box types with reference semantics (they borrow the coordinates array). `BoxRef` and `ConstBoxRef` supports dynamic array bound while `boxNd_ref` and `const_boxNd_ref` provide compile-time bound checking.


.. cpp:class:: template <int ndim> ndbox

    `ndbox` represents a n-dimensional box over a grid. The number of dimensions can be known at
    compile time or dynamic ``ndim = -1``. It is a container (own the value).

    .. cpp:function:: ndbox() = default

        Default constructor. Creates an empty box.

    .. cpp:function:: template <class B> ndbox(const B& other)

        Converting constructor from any box implementing the :cpp:class:`__BoxInterface`. This overload is enabled only if `B` is compatible with ``ndbox``  and ``ndim == (-1 || other.ndim)``.

    .. cpp:function:: ndbox(ConstBoxRef p1, ConstBoxRef p2)

        Creates a box starting at `p1` and ending at `p2` (excluded). The dimension of `p1` and `p2` must match and should match `ndim` if the box has static storage.
    
    .. cpp:function:: ndbox(int width)

        Creates a 1D box with the given width starting at 0.
        Only available when ``ndim == -1`` or ``ndim = 1``.

    .. cpp:function:: ndbox(int width, int height)

        Creates a 2D box with the given (width, height) starting at (0,0). 
        Only available when ``ndim == -1`` or ``ndim = 2``.
    
    .. cpp:function:: ndbox(int width, int height, int depth)

        Creates a 3D box with the given (width, height, depth) starting at (0,0,0).
        Only available when ``ndim == -1`` or ``ndim = 3``.

    .. cpp:function:: ndbox(int width, int height, int depth, int duration)

        Creates a 4D box with the given (width, height, depth, duration) starting at (0,0,0,0).
        Only available when ``ndim == -1`` or ``ndim = 4``.



    
.. cpp:class:: template <int ndim> ndboxref

    `ndboxref` represents a n-dimensional box over a grid. The number of dimensions can be known at
    compile time or dynamic ``ndim = -1``. It has a **constant** reference semantic and should be used as a function parameter only.


    .. cpp:function:: ndboxref(const B& other)

        Converting constructor from any box implementing the :cpp:class:`__BoxInterface`. This overload is enabled only if `B` is compatible with ``ndboxref``, i.e. if ``ndim == (-1 || other.ndim)``.

Aliases
=======

.. cpp:type:: Box = ndbox<-1>
.. cpp:type:: box1d = ndbox<1>
.. cpp:type:: box2d = ndbox<2>
.. cpp:type:: box3d = ndbox<3>
.. cpp:type:: ConstBoxRef = ndboxref<-1>
.. cpp:type:: const_box1d_ref = ndboxref<1>
.. cpp:type:: const_box2d_ref = ndboxref<2>
.. cpp:type:: const_box3d_ref = ndboxref<3>

FIXME!

Read-Only box Interface
=========================

.. cpp:class:: template <int ndim, class T> __boxInterface

    .. cpp:type:: coord_type = T

    .. cpp:function:: int dim() const

        return the number of dimension

    .. cpp:function: constexpr std::size_t size() const noexcept

        Return the number of elements in the box

    .. cpp:function:: constexpr int size(int k) const noexcept

        Return the number of elements in the k-th dimension of the box

    .. cpp:function:: constexpr bool empty() const noexcept

        Return true if the box is empty

    .. cpp:function:: constexpr int width() const noexcept

        Returns the width of the box

    .. cpp:function:: constexpr int height() const noexcept

        Returns the height of the box (available for ``dim() >= 2``)

    .. cpp:function:: constexpr int depth() const noexcept

        Returns the depth of the box (available for ``dim() >= 3``)


    .. cpp:function:: constexpr coord_type tl(int k) const noexcept

        Returns the k-th coordinate of the top-left corner point


    .. cpp:function:: constexpr coord_type br(int k) const noexcept

        Returns the k-th coordinate of the bottom-right corner point (past-the-end)


    


.. rubric:: box Interopability

Any two boxs p₁ and p₂ of types P₁ and P₂ are said *compatible* if their value types are *compatible* and if they have the same number of dimensions (``p₁.dim() == p₂.dim()``). They thus share a `common_reference` (:cpp:concept:`std::CommonReference˂P₁,P₂˃ <std::CommonReference>`).

* Conversion 
    Two *compatible* boxs are convertible to each other (:cpp:concept:`std::Convertible`).

    Example::

        mln::box2d p1 = {x, y};
        mln::ndbox<2, long> p2 = p1; // Ok (int to long conversion)
        mln::box p3 = p1;            // Ok (static to dynamic conversion)
        mln::box2d p4 = p3;          // Ok (dynamic to static conversion with run-time dim assertion)



Mutable box Interface
=======================

FIXME

.. cpp:class:: template <int ndim, class T> __MutableboxInterface

    .. cpp:type:: coord_type = T

    .. cpp:function:: T* data()

        Return a boxer to the mutable coordinates array.



        