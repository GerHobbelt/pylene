Image views
===========

Lazy computation of images
--------------------------

.. figure:: /figures/core/transform_thresholding.svg
  :align: center
  
  Illustration of view performing a thresholding.

|pylene| enables the computation of images *on the fly*. It means that
the values are not stored in memory but computed when needed. It
enables to perform optimizations by evaluating images only at the
every last moment, e.g. in the expression::

  mln::image2d<uint8_t> u;
  auto h = [](int x) -> bool { return x < 150; }
  auto v = mln::transform(u, h);

`v` is an image that does not store the values in memory but evaluates ``v(x) = u(x) <
150`` for each element on the fly. In this example, `h` is not writable as ``h(x) =
false`` is not a valid expression.


:ref:`views` can be seen as the *table views* used in
some SQL engines. `Tables` are in-memory storage of data and `views`
are some **virtual** `tables`. Depending on the way columns are
generated in `views`, they can be writable or not. 


Fundamental image views
---------------------------

``auto g = mln::view::transform(f, fun)``
  Return an image `g` defined by:

  * ``g(p) = fun(f(p))`` if `fun` is a function on the `value type` of `f`

``auto g = mln::view::zip(f₁, f₂, ..., fₙ)``
  Return an image `g` where values are formed by the tuple of each
  value in `fᵢ`, i.e., ``g(p) = <f₁(p), f₂(p), ..., fₙ(p)>``

``auto g = mln::view::filter(f, pred)``
  Return an image `g` from `f` restricted on the domain defined by:
   * :math:`\{ p \mid pred(f(p)) \}` if if `pred` is a function on the
     `value type` of `f`.

``auto g = mln::view::clip(f, subdomain)``
  Return an image `g` from `f` restricted to `subdomain`.


Lazy images operators
---------------------

Arithmetical, logical and comparison operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. rubric:: Notations

* `f`, `g`, `h` are images of value type: `V1`, `V2`, `R`
* `s` is a scalar of type `S`

All arithmetical and logical operators on images return a view where
values will be computed on the fly. The value type of the view will be
the *common type* between operand types (note that *integer promotion*
will performed during the computation but then casted to the *common
type*). For example::

  mln::image2d<uint8_t> f, g;
  auto h = f + g; // V1 = uint8_t, V2 = uint8_t, R = uint8_t
  auto h = f + 2; // V1 = uint8_t, V2 = int, R = int
  auto h = -f; // V1 = uint8_t, R = uint8_t
  auto h = - mln::view::cast<int>(f); // V1 = int, R = int
  auto h = (f == g); // V1 = uint8_t, V2 = int, R = bool

.. topic:: Operators

+-------------------------------------------------------+------------------------------------+
| ``-f``                                                | Element-wise negate                |
+-------------------------------------------------------+------------------------------------+
| ``lnot(f)``                                           | Element-wise logical not           |
+-------------------------------------------------------+------------------------------------+
| ``f + g, f + s, s + f``                               | Element-wise binary addition       |
+-------------------------------------------------------+------------------------------------+
| ``f - g, f - s, s - f``                               | Element-wise binary soustraction   |
+-------------------------------------------------------+------------------------------------+
| ``f * g, s * f, f * s``                               | Element-wise binary multiplication |
+-------------------------------------------------------+------------------------------------+
| ``f / g, s / f, f / s``                               | Element-wise binary division       |
+-------------------------------------------------------+------------------------------------+
| ``land(f,g), land(f,s), land(s,f)``                   | Element-wise logical and           |
+-------------------------------------------------------+------------------------------------+
| ``lor(f,g), lor(f,s), lor(s,f)``                      | Element-wise logical or            |
+-------------------------------------------------------+------------------------------------+
| ``ifelse(f, g, h), ifelse(f, g, s), ifelse(f, s, g)`` | Element-wise ternary condition     |
+-------------------------------------------------------+------------------------------------+
| ``f < g, f < s, s < f``                               | Element-wise less than             |
+-------------------------------------------------------+------------------------------------+
| ``f > g, f > s, s > f``                               | Element-wise greater than          |
+-------------------------------------------------------+------------------------------------+
| ``f <= g, f <= s, s <= f``                            | Element-wise less or equal than    |
+-------------------------------------------------------+------------------------------------+
| ``f >= g, f >= s, s >= f``                            | Element-wise greater or equal than |
+-------------------------------------------------------+------------------------------------+
| ``f == g, f == s, s == f``                            | Element-wise equal to              |
+-------------------------------------------------------+------------------------------------+
|                                                       |                                    |
+-------------------------------------------------------+------------------------------------+
