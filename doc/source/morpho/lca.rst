Lowest Common Ancestor
======================

* Include :file:`<mln/morpho/lca.hpp>`

The Lowest Common Ancestor (or LCA) of two nodes is the lowest node in the tree
whose subtrees contains these nodes.

Pylene implements two algorithms to compute the LCA:

    * A naive LCA.
    * An Euler tour based LCA.

All the algorithms are implemented as classes and designed to follow the same pattern :

    * A constructor, taking into argument the tree, which makes the preprocessing (if there is).
    * A function call operator, taking into arguments the two nodes, which query the LCA.

The naive LCA
-------------

.. cpp:class:: simple_lca

This class implements a simple algorithms to compute the LCA, consisting on
climbing the tree from two given nodes until they reach the same node, which is
the lowest common ancestor of these two nodes.

The Euler tour based LCA
------------------------

.. cpp:class:: template <typename RMQ> lca

This class implements a LCA based on the reduction to the Range Minimum Query (RMQ) [Ben00]_.

It takes in template argument a class implementing the RMQ, based on the same
design as the LCA classes : a constructor for the preprocessing and a function
call operator for the query. Two RMQ are implemented in the library :

    * The ``rmq_sparse_table`` class, based on the building of a sparse table to preprocess the tree. This class has a preprocessing time in :math:`O(nlog(n))` and a query time in :math:`O(1)`.
    * The ``restricted_rmq`` class, taking into advantage the fact that two consecutives node in the Euler tour have a difference of depth of :math:`\pm1`. This class has a preprocessing time in :math:`O(n)` and a query time in :math:`O(1)`.

The ``restricted_rmq`` is used by default, but it uses the ``rmq_sparse_table`` (see [Ben00]_ for more details).

Dynamic LCA
-----------

.. cpp:class:: dyn_lca

This class implements a dynamic (runtime) choice of the LCA algorithm used,
thanks to *type-erasure*. It provides two LCA algorithms :

    * ``dyn_lca::kind::SIMPLE``, which uses the ``simple_lca`` class
    * ``dyn_lca::kind::LINEAR``, which uses the ``lca<restricted_rmq>`` class.

It does not provide the Sparse Table LCA since it is always slower than one of
the two previous one, depending of the usage.

The ``dyn_lca`` class keeps the same design as the other LCA classes : the
preprocessing is done in the constructor and the query is done thanks to the
function call operator.

    .. rubric:: Example

    Below is an example of the usage of the ``dyn_lca`` class::

        auto [t, nm] = ...; // Compute a tree
        auto lca = mln::morpho::dyn_lca(mln::morpho::dyn_lca::kind::SIMPLE, t); // Use the simple_lca class
        int a = lca(1, 2);


.. [Ben00] Michael A. Bender and Martin Farach-Colton (2000) The LCA Problem Revisited. *Latin American Symposium on Theorical Informatics*. Springer, Berlin, Heidelberg. 88-94