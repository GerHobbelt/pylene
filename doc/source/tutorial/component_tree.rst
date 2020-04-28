Component Trees
===============

.. |tos| replace:: Tree of Shapes

Component trees are morphological hierarchical representations of
images based on their level set decomposition:

* The min-tree organizes lower connected-components into a tree:

  .. math::

     \Gamma^- = \{ CC(X) : X \in [u \le \lambda] \}

* The max-tree organizes upper connected-components into a tree:

  .. math::

     \Gamma^+ = \{ CC(X) : X \in [u \ge \lambda] \}

* The |tos| merges the hole-filled components of the  min and
  max-trees into a single tree.

.. contents::
   :local:



Computing the tree
------------------

The are basically three functions:

* :cpp:func:`mln::morpho::maxtree`
* :cpp:func:`mln::morpho::mintree`
* :cpp:func:`mln::morpho::tos`


.. code::

   #include <mln/core/image/image2d.hpp>
   #include <mln/io/imread.hpp>
   #include <mln/morpho/maxtree/maxtree.hpp>


   mln::image2d<uint8> f;
   mln::io::imread("lena.pgm", f);
   auto tree = mln::morpho::maxtree(f, c4);


Morphological tree structures
-----------------------------

A component tree stores only stores the points (or indexes) of the
image (not the values). Components are represented by *nodes* which
only contain the points that are not present in any sub-component.

* A *tree* `t` provides few features in fact. It allows to retrieve its
  set of nodes with ``t.nodes()`` or ``t.nodes_without_root()`` so that
  one can iterate on it. Nodes are *ordered*: a forward traversal is top
  down (i.e., it goes from the root to the leaves), and a reverse scan
  is bottom up.
  It also provides a mapping *point or index -> tree node* through the
  function ``t.get_node_at``.

* A *node* `n` is described by a unique *id*, but the function
  ``t.nodes()`` does not return a set of *id* but a set of nodes where
  a *node* is actually a small structure providing additional
  facilities.  From a node, one can access to its parent and its
  children through ``n.parent()`` or ``n.chidren()``. 

Here are the most important low-level manipulation functions for *trees*:

* `t` is a tree of type `tree_t`
* `p` is a point or an index from the image
* `id` is a node identifier
* `n` is a node of type `tree_t::node_type`

+-------------------------------+-------------------------------------------------------------+
| Function                      |                                                             |
+===============================+=============================================================+
| ``t.nodes()``                 | Return the range of the tree nodes.                         |
+-------------------------------+-------------------------------------------------------------+
| ``n = t.get_node_at(p)``      | Return the largest component that contains `p` (its node)   |
+-------------------------------+-------------------------------------------------------------+
| ``n = t.get_node(id)``        | Return a node structure                                     |
+-------------------------------+-------------------------------------------------------------+
| ``n = t.get_root()``          | Return the root node                                        |
+-------------------------------+-------------------------------------------------------------+
| ``id = t.get_root_id()``      | Return the root node *identifier*                           |
+-------------------------------+-------------------------------------------------------------+
| ``t = t.get_subtree(id)``     | Get the subtree rooted in the node of *identifier* `id`.    |
+-------------------------------+-------------------------------------------------------------+
| ``t.size()``                  | Get the size of tree (usually number of nodes)              |
+-------------------------------+-------------------------------------------------------------+

Here are the most important low-level manipulation functions for
*nodes*:

+---------------------+-----------------------------------------------------------------+
| ``n.id()``          | Get the *identifier* of the node.                               |
+---------------------+-----------------------------------------------------------------+
| ``n = n.parent()``  | Get the parent node of `n`.                                     |
+---------------------+-----------------------------------------------------------------+
| ``n.children()``    | Return the range of child nodes of `n`.                         |
+---------------------+-----------------------------------------------------------------+
| ``n.is_root()``     | True if the `n` is the root node.                               |
+---------------------+-----------------------------------------------------------------+
| ``n.has_child()``   | False if the `n` is a leaf node.                                |
+---------------------+-----------------------------------------------------------------+
| ``t = n.tree()``    | Return the underlying tree.                                     |
+---------------------+-----------------------------------------------------------------+

Tree traversal examples
-----------------------

.. topic:: Traversing a branch to get all the components containing `p`

   .. code::

      tree_t::node_type n = t.get_node_at(p)
      while (n.id() != tree_t.npos()) {
        // do something with n
        n = n.parent();
      }


.. topic:: Computing the size of each component.

   .. code::

      property_map<tree_t, unsigned> sz(t, 0); // ignore this for now
      // Iterate on the image
      mln_foreach(auto px, f.pixels()) {
        tree_t::node_type n = t.get_node_at(px.index());
        sz[n]++;
      }
      // Accumulate bottom up
      mln_reverse_foreach(auto n, t.nodes_without_root()) {
        sz[n.parent()] += sz[n];
      }

.. topic:: Computing the number of leaves of a node `n`

   .. code::

      unsigned nleaves = 0;
      auto subtree = t.get_subtree(n.id());
      mln_foreach(auto x, subtree.nodes())
        if (not x.has_child())
          nleaves++;

.. _property map:

Property maps
-------------

It is common to associate values to nodes. The implementation keeps a
distinction between the data structure (the *tree*) and the values
associated to the nodes through *property maps*. A property map is a
simple mapping `node -> value` or `node identifier -> value`.
``property_map<tree_t, V>`` is the type of *property map* for
component trees of type `tree_t` and having property values of type
`V`.

Here is a example to compute the number of children per node:

.. code::

   // Declare and initialize to 0 a property map for the tree
   property_map<tree_t, unsigned> nchildren(t, 0);

   mln_reverse_foreach(auto x, t.nodes_without_root())
     nchilden[x.parent()]++;


Attribute maps are versatile and can be computed on the fly (from a
function for example). Below is an example of boolean property map
that says if a node has a single child. It can be used to simplify the
tree by collapsing the nodes in a chain.

.. code::

   auto f = [&nchildren](tree_t::node_type x) -> bool {
     return nchildren[x] != 1;
   };
   auto predicate = make_functional_property_map<tree_t::node_type>(f);

   predicate[f.get_root()]; // true or false...


Computing attributes
--------------------

It is really common to compute descriptors on the tree's
components. Such descriptors are generally called *attributes*. They
are computed through :concept:`accumulators <Accumulator>`. The result
of the accumulation is stored in a :ref:`property map <property map>`.

Since this is such a common operation, it is already implemented
through a set of algorithms.

Here:

* `acc` is an accumulator or a feature.
* `pmap` is a property map

+--------------------------------------+--------------------------------------------------------------------+
| ``pmap = accumulate(t,acc)``         | Accumulate the point or indexes of the components (depending if the|
|                                      | tree stores points or indexes).                                    |
+--------------------------------------+--------------------------------------------------------------------+
| ``pmap = vaccumulate(t, f, acc)``    | Accumulate the values of the `f` pixels foreach component.         |
+--------------------------------------+--------------------------------------------------------------------+
| ``pmap = paccumulate(t, f, acc)``    | Accumulate the points of the `f` pixels foreach component.         |
+--------------------------------------+--------------------------------------------------------------------+
| ``pmap = pixaccumulate(t, f, acc)``  | Accumulate `f` pixels foreach components                           |
+--------------------------------------+--------------------------------------------------------------------+

Each algorithm has a variation denoted with the suffix `*_proper`
(`accumulate_proper`, `vaccumulate_proper` ...)  where the
accumulation is done on the proper pixels of each node (i.e., the
pixels that are not in any sub-component).

Below is an example for computing the average gray level of each component.

.. code::

   #include <mln/accu/accumulators/mean.hpp>
   #include <mln/morpho/component_tree/accumulate.hpp>

   image2d<uint8> f = ...;
   tree_t f = ...;
   auto gl_map = morpho::accumulate(t, f, accu::features::mean<> ());


Property maps are also used to encode the original values of the image
from which the tree has been built. This is done thanks to the
function `make_property_map_from_image`::

  property_map<tree_t, uint8> vmap = make_attribute_map_from_image(t, f);



Filtering the tree
------------------

Filtering is also a common operation on morphological trees. There
exist several filtering strategies: *min, max, and direct*,
implemented through the algorithms below:

`pred` is a boolean *property map* where `pred[x] = true` says that we
want to preserve the node `x`.

+--------------------------------------+-----------------------------------------------+
| ``filter_min_inplace(t, pred)``      | Min filtering strategy                        |
+--------------------------------------+-----------------------------------------------+
| ``filter_max_inplace(t, pred)``      | Max filtering strategy                        |
+--------------------------------------+-----------------------------------------------+
| ``filter_direct_inplace(t, pred)``   | Direct filtering strategy                     |
+--------------------------------------+-----------------------------------------------+

Here is an example of filtering to collapse the nodes part of a chain
to simplify the tree::

   #include ... // as above
   #include <mln/morpho/component_tree/filtering.hpp>

   auto f = [&nchildren](tree_t::node_type x) -> bool {
     return nchildren[x] != 1;
   };
   auto predicate = make_functional_property_map<tree_t::node_type>(f);
   filter_direct_inplace(t, predicate);

Image reconstruction
--------------------

The next step after image filtering is generally the reconstruction of
the image from the filtered tree. This is done through the algorithm
`reconstruction`::

  #include <mln/morpho/component_tree/reconstruction.hpp>

  tree_t filtered_tree = ...; // the filtered tree
  property_map<tree_t, uint8> vmap = ...; // the property map: node -> value

  image2d<uint8> out;
  resize(out, f);
  reconstruction(filtered_tree, vmap, out);


Loading and saving a tree
-------------------------

There are basically two IO routines for component trees
`mln::morpho::load` and `mln::morpho::save` illustrated below::

  #include <mln/morpho/component_tree/io.hpp>

  tree_t t = ...; // The tree

  std::ofstream outfile("mytree");
  morpho::save(t, outfile);

  tree_t t2;
  std::ifstream infile("mytree");
  morpho::load(infile, t2);


A complete example
------------------

Filtering the square roadsigns.

.. image:: /snippets/images/roadsigns.png

Method description
^^^^^^^^^^^^^^^^^^

The method will proceed as follows:

1. Compute the |tos| of the input image::

    auto t = mln::morpho::tos(f);

2. The |tos| uses a special representation that needs to double the
   size of the image by adding intermediate pixels. We thus need to
   resize the input image (leaving the values on intermediate pixels
   undefined) for further processing::

    mln::image2d<uint8> f2;
    mln::resize(f2, t._get_data()->m_pmap);
    mln::box2d d = f2.domain();
    mln::copy(f, f2 | mln::sbox2d{d.pmin, d.pmax, {2,2}});

3. Compute the bounding box of each shape and its size. Note
   that the bbox is actually the `inf` and `sup` of the points in the
   shape::

     auto prop_map = mln::morpho::paccumulate(t, f2,
                                           mln::accu::features::inf<> () &
                                           mln::accu::features::sup<> () &
                                           mln::accu::features::count<> ());
4. Filter the objects that:

   * are not too big (< 1000 x 1000 pixels)
   * are not too small (> 100 x 100 pixels)
   * cover 95% of the bounding box at least (so rectangular shapes)

   ::

      auto pred = [&prop_map](tree_t::vertex_id_t x) {
        mln::point2d  pmin = mln::accu::extractor::inf(prop_map[x]);
        mln::point2d  pmax = mln::accu::extractor::sup(prop_map[x]);
        unsigned sz   = mln::accu::extractor::count(prop_map[x]);
        unsigned area_bbox = (pmax[0] - pmin[0] + 1) * (pmax[1] - pmin[1] + 1);
        return sz > (0.95 * area_bbox) and sz > 10000 and sz < 1000000;
      };
      auto pred_ = mln::make_functional_property_map<tree_t::vertex_id_t>(pred);
      mln::morpho::filter_direct_inplace(t, pred_);

5. Reconstruct the image::

     mln::image2d<uint8> out;
     mln::resize(out, t._get_data()->m_pmap);

     auto vmap = mln::morpho::make_attribute_map_from_image(t, f2);
     mln::morpho::reconstruction(t, vmap, out);

(The full code is given below)

It gives the following result:

.. image:: /snippets/images/roadsigns-square.png


Which is not bad but not perfect. Roadsigns have a high contrast
between their background and their content, thus a large variance. We
can use this to improve the classification. The minimal variance was
arbitrary set to 250, which means an average of at least 50 gray
levels with average gray level of the shape::

   auto var_map = mln::morpho::vaccumulate(t, f2, mln::accu::features::variance<> ());

   auto pred = [&prop_map,&var_map](tree_t::vertex_id_t x) {
    mln::point2d  pmin = mln::accu::extractor::inf(prop_map[x]);
    mln::point2d  pmax = mln::accu::extractor::sup(prop_map[x]);
    unsigned sz   = mln::accu::extractor::count(prop_map[x]);
    unsigned area_bbox = (pmax[0] - pmin[0] + 1) * (pmax[1] - pmin[1] + 1);
    double var = var_map[x];
    return sz > (0.95 * area_bbox) and sz > 10000 and sz < 1000000 and var > 250;
   };

Result:

.. image:: /snippets/images/roadsigns-square-2.png


Full code
^^^^^^^^^

.. literalinclude:: /snippets/component_tree_1.cpp










