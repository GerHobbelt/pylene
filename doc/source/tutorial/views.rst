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
