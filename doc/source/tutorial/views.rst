.. _morphers:

Image views (morphers)
======================

Lazy images
-----------

|pylene| enables the computation of images *on the fly*. It means that
the values are not stored in memory but computed when needed. It
enables to perform optimizations by evaluating images only at the
every last moment, e.g. in the expression::

  image2d<uint8> f,g;
  auto h = 2*f + g;

`h` is an image that does not store the values in memory but ``h(x) =
2*f(x) + g(x)`` for each element on the fly. In this example, `h` is
not writable as ``h(x) = 3`` is not a valid expression.

:ref:`morphers` can be seen as the *table views* used in
some SQL engines. `Tables` are in-memory storage of data and `views`
are some **virtual** `tables`. Depending on the way columns are
generated in `views`, they can be writable or not. The same notion
occurs for :ref:`morphers`.


Foundamental image morphers
---------------------------

``auto g = imtransform(f, fun)``
  Return an image `g` defined by:

  * ``g(p) = fun(f(p))`` if `fun` is a function on the `value type` of `f`
  * ``g(p) = fun( (p,f(p)) )`` if `fun` is a function on the `pixel type
    ` of `f`.

  .. note::
     If the expression ``fun( f::pixel_type )`` is valid, the second
     version is chosen, the first one otherwise.

``auto g = imzip(f₁, f₂, ..., fₙ)``
  Return an image `g` where values are formed by the tuple of each
  value in `fᵢ`, i.e., ``g(p) = <f₁(p), f₂(p), ..., fₙ(p)>``

``auto g = imfilter(f, pred)``
  Return an image `g` from `f` restricted on the domain defined by:
   * :math:`\{ p \mid pred(f(p)) \}` if if `pred` is a function on the
     `value type` of `f`.
   * :math:`\{ p \mid pred( (p,f(p)) ) \}` if if `pred` is a function on the
     `pixel type` of `f`.

  .. note::
     If the expression ``pred( f::pixel_type )`` is valid, the second
     version is chosen, the first one otherwise.

``auto g = f | subdomain``
  Return an image `g` from `f` restricted to `subdomain`.




Lazy images operators
---------------------

Arithmetical, logical and comparison operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. topic:: Notations

* `f`, `g`, `h` are images of value type: `V1`, `V2`, `R`
* `s` is a scalar of type `S`

All arithemtical and logical operators on images return a view where
values will be computed on the fly. The value type of the view will be
the *common type* between operand types (note that *integer promotion*
will performed during the computation but then casted to the *common
type*). For example::

  image2d<uint8> f, g;
  auto h = f + g; // V1 = uint8, V2 = uint8, R = uint8
  auto h = f + 2; // V1 = uint8, V2 = int, R = int
  auto h = -f; // V1 = uint8, R = uint8
  auto h = - imcast<int>(f); // V1 = int, R = int
  auto h = (f == g); // V1 = uint8, V2 = int, R = bool

.. topic:: Operators

+----------------------------------------------------+--------------------------------------+
| ``-f``                                             | Element-wise negate                  |
+----------------------------------------------------+--------------------------------------+
| ``lnot(f)``                                        | Element-wise logical not             |
+----------------------------------------------------+--------------------------------------+
| ``f + g, f + s, s + f``                            | Element-wise binary addition         |
+----------------------------------------------------+--------------------------------------+
| ``f - g, f - s, s - f``                            | Element-wise binary soustraction     |
+----------------------------------------------------+--------------------------------------+
| ``f * g, s * f, f * s``                            | Element-wise binary multiplication   |
+----------------------------------------------------+--------------------------------------+
| ``f / g, s / f, f / s``                            | Element-wise binary division         |
+----------------------------------------------------+--------------------------------------+
| ``land(f,g), land(f,s), land(s,f)``                | Element-wise logical and             |
+----------------------------------------------------+--------------------------------------+
| ``lor(f,g), lor(f,s), lor(s,f)``                   | Element-wise logical or              |
+----------------------------------------------------+--------------------------------------+
| ``where(f, g, h), where(f, g, s), where(f, s, g)`` | Element-wise ternary condition       |
+----------------------------------------------------+--------------------------------------+
| ``f < g, f < s, s < f``                            | Element-wise less than               |
+----------------------------------------------------+--------------------------------------+
| ``f > g, f > s, s > f``                            | Element-wise greater than            |
+----------------------------------------------------+--------------------------------------+
| ``f <= g, f <= s, s <= f``                         | Element-wise less or equal than      |
+----------------------------------------------------+--------------------------------------+
| ``f >= g, f >= s, s >= f``                         | Element-wise greater or equal than   |
+----------------------------------------------------+--------------------------------------+
| ``f == g, f == s, s == f``                         | Element-wise equal to                |
+----------------------------------------------------+--------------------------------------+
|                                                    |                                      |
+----------------------------------------------------+--------------------------------------+
