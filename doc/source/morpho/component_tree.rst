Component Trees (Overview)
==========================

* Include :file:`<mln/morpho/component_tree.hpp>`
* Include :file:`<mln/morpho/maxtree.hpp>`
* Include :file:`<mln/morpho/tos.hpp>`

.. cpp:namespace:: mln::morpho



Component trees are morphological hierarchical representations of
images based on their level set decomposition:

* The *min-tree* organizes lower connected-components into a tree:

  .. math::

     \Gamma^- = \{ CC(X) : X \in [u \le \lambda] \}

* The *max-tree* organizes upper connected-components into a tree:

  .. math::

     \Gamma^+ = \{ CC(X) : X \in [u \ge \lambda] \}

* The *tree of shapes* merges the hole-filled components of the  min and
  max-trees into a single tree.


.. list-table:: Example
    :align: center
    :widths: 2 1 1 1

    * - Input
      - Min-tree
      - Max-tree
      - Tree of Shapes
    * - .. image:: /figures/morpho/simpleimage_grayv.svg  
        .. image:: /figures/morpho/simpleimage-shapes.svg
      - .. image:: /figures/morpho/simpleimage-mintree.svg
      - .. image:: /figures/morpho/simpleimage-maxtree.svg
      - .. image:: /figures/morpho/simpleimage-tos.svg



.. contents:: Table of contents
    :local:


Component tree Internal Representation
--------------------------------------

A tree is represented as an array of nodes where a node contains:

1. The index of the parent node
2. The level of the current node

The :cpp:class:`component_tree` is thus composed of two arrays:

1. A ``parent`` array with element of type ``int``
2. A ``level`` array 

These arrays are **sorted** in the topological order, that is, we have ``parent[i] < i``. The parent of the root
node is -1 (``parent[0] = -1``).

The correspondance between the pixels and the node they belong is stored in an image, called the `node_map`.

.. list-table:: Example
    :align: center

    * - Node map
      - Component tree internal arrays
    * - .. image:: /figures/morpho/simpleimage-nodemap.svg
      - .. image:: /figures/morpho/simpleimage-treenc.svg

.. cpp:class::  template <class V> component_tree

    .. cpp:var:: std::vector<int> parent

        Get the underlying *parent* array

    .. cpp:var:: std::vector<V> levels

        Get the underlying *levels*

Computing trees
---------------

See :doc:`maxtree` and :doc:`tos`.


Tree traversal examples
-----------------------

.. topic:: Traversing a branch to get all the components containing `p`

    .. code::

        int id = node_map(p);
        while (id != -1) {
            // do something with id
            id = t.parent[id];
        }

.. topic:: Computing the size of each component of tree `t`

   .. code::

        std::size_t n = t.size();
        std::vector<int> sz(n, 0);
        // Iterate on the image
        mln_foreach(auto id, node_map.values())
          sz[id]++;

        // Accumulate bottom up
        for (int i = n - 1; i > 0; --i)
          sz[t.parent[i]] += sz[n];


Attribute Computation
---------------------

It is common to associate values to nodes. The implementation keeps a distinction between the data structure (the
*tree*) and the values associated to the nodes through *property maps*. A property map is a mapping `node id -> value`
that is encoded with a standard vector.

Here is a example to compute the number of children per node for a tree `t`::

    // Declare and initialize to 0 a property map for the tree `t`:
    std::size_t n = t.size();
    std::vector<int> nchildren(n, 0);

    for (int i = n - 1; i > 0; --i)
        nchildren[t.parent[i]]++;


One may compute features for each component of the tree (e.g. for filtering) related with the image content. Most common
attributes are related to the shape of the component or its colors, e.g.:

* The size of the component
* The average gray level of the component
* The circularity of the component
* ...

A :cpp:class:`component_tree` `t` has the following methods.


.. cpp:namespace-push::  template <class V> component_tree

.. cpp:function:: auto compute_attribute_on_points(Image node_map, Accumulator accu) const
                  auto compute_attribute_on_values(Image node_map, Image values, Accumulator accu) const
                  auto compute_attribute_on_pixels(Image node_map, Image values, Accumulator accu) const

    Accumulate the points of each component.

    :param node_map: An image thats maps: point -> node id
    :param values: An image where values to accumlate are taken from
    :param accu: The feature to compute

    :return: A vector mapping for each node the result of accumulation.

    Pseudo-code::

        std::vector<Acc> accus(n);
        std::vector<...> result(n);

        // (1) On points
        mln_foreach(auto px, node_map.pixels())
            accus[px.val()].take(px.point());
        
        // or (2) On values
        auto zz = mln::ranges::view::zip(node_map.values(), values.values());
        mln_foreach((auto [id, v]), zz)
            accus[id].take(v);

        // or (3) on pixels
        mln_foreach(auto px, values.pixels())
            accus[node_map(px.point())].take(px);           

        for (int i = n-1; i > 0; --i)
            accus[parent[i]].take(accus[i]);
        
        std::ranges::transform(accus, result, [](auto&& a) { return a.to_result(); });


Example: computing the size (number of points) of the components.

::

    #include <mln/accu/accumulators/count.hpp>

    auto sizes = tree.compute_attribute_on_points(node_map, mln::accu::features::count<>());

.. cpp:function:: auto compute_depth() const

    Compute the depth attribute for each node where *depth* stands for the length of the path from the root to the node.
    (the root node has a null depth).

    :return: An `int` vector mapping `node_id -> int`





Tree Filtering
--------------

Filtering is also a common operation on morphological trees to simplify the tree or filter some unwanted shapes.  Let
`pred` be a predicate where `pred(x) = true` says that we want to preserve the node of id `x`. We have the following
filtering strategies.

MIN (`CT_FILTER_MIN`)

    A node is remove if it does not pass the predicate and all its descendant are removed as well. Formally, a component
    Γ is kept if  𝓒(Γ) with 𝓒(Γ) = ⋁ { 𝓟(Γ'), Γ ⊆ Γ' }.

MAX (`CT_FILTER_MAX`)

    A node is remove if every node in its childhood does not pass
    the predicate. All its descendant are removed as well.  Formally, a
    component Γ is kept if 𝓒(Γ) with 𝓒(Γ) = ⋀ { 𝓟(Γ'), Γ' ⊆ Γ }.

DIRECT (`CT_FILTER_DIRECT`)

    A node is remove if it does not pass the predicate, the others remain.


A :cpp:class:`component_tree` `t` has the following methods.

.. cpp:function::  void filter(ct_filtering strategy, Predicate pred)
                   void filter(ct_filtering strategy, Image node_map, Predicate pred)


    Filter the tree according to a given strategy. If *node_map* is provided, it is updated so that
    pixels map to valid nodes.

    :param strategy: The filtering strategy
    :param node_map: (Optional) An image thats maps ``point -> node id``
    :param pred: The predicate fuction ``node id -> bool``

    .. rubric:: Example


    Here is an example of filtering to collapse the nodes part of a chain
    to simplify the tree::

        #include ... // as above
        #include <mln/morpho/component_tree.hpp>

        component_tree<uint8_t> t = ...;
        std::vector<int> nchildren;
        // Compute the attribute node_id -> #number of children (see above)

        auto pred = [&](int id) { nchildren > 1; };      
        t.filter(CT_FILTER_DIRECT, pred);


Image reconstruction
--------------------

The next step after image filtering is generally the reconstruction of
the image from the filtered tree.

.. cpp:function::  auto reconstruct(Image node_map)
                   auto reconstruct_from(Image node_map, std::span<V> values)

    Reconstruct the tree from the initial levels or from levels taken from the parameter *values*.


    :param node_map: An image thats maps ``point -> node id``
    :param values: (Optional) Recontruction values (default is ``t.levels``) 

    .. rubric:: Example

    Connected opening of size 20::

        auto [t, node_map] = ...; // Tree Computation
        auto area = t.compute_attribute_on_points(node_map, mln::accu::features::count<>());
        t.filter(CT_FILTER_DIRECT, [&area](int id) { return area[id] > 20; });
        auto out = t.reconstruct(node_map);

Horizontal cut
--------------
When the tree is a hierarchy of partition, such as the :doc:`alphatree`, it is possible
to make an horizontal cut of this tree.

.. cpp:function::   I horizontal_cut(const T threshold, I nodemap) const
                    I horizontal_cut_from_levels(const T threshold, I nodemap, ::ranges::span<V> levels) const

    Make an horizontal cut at threshold ``threshold`` of the tree and return the nodemap associated to the cut.

    :param threshold: The threshold of the horizontal cut
    :param nodemap: An image thats maps ``point -> node id``
    :param levels: (Optional) The altitude of each node in the tree (for example the :math:`\alpha` associated to each node for the alphatree).

Saliency Computation
--------------
It is also possible to compute the saliency map to obtain another visualization.

.. cpp:function:: auto saliency(Image node_map)

    Compute and return the saliency map of the tree.

    :param node_map: An image thats maps ``point -> node id``

    :return: The saliency map as an image

A complete example
------------------

Filtering the square roadsigns.

.. image:: /snippets/images/roadsigns.png

Method description
^^^^^^^^^^^^^^^^^^

The method will proceed as follows:

1. Compute the ToS of the input image::

    auto [t, node_map] = mln::morpho::tos(f);

2. The ToS uses a special representation that needs to double the
   size of the image by adding intermediate pixels. We thus need to
   reconstruct an image of the right size::

        mln::image2d<uint8_t> f2 = t.reconstruct(node_map);



3. Compute the bounding box, the size and the variance of each shape. We create a custom
   accumulator object for this, whose implementation is described below in the full code section::

        struct my_accu_t : mln::Accumulator<my_accu_t>
         {
           using result_type   = my_accu_t;
           using argument_type = mln::image_pixel_t<mln::image2d<uint8_t>>;

           my_accu_t() = default;

           template <class Pix> void take(Pix px) { ... };
           void take(const my_accu_t& other) { ... };

           int count() const { ... }
           int area_bbox() const { ... }
           double variance() const { ... }
           my_accu_t to_result() const { return *this; }
         };

        auto prop_map = t.compute_attribute_on_pixels(node_map, f2, my_accu_t{});


4. Filter the objects that:

   * are not too big (< 1000 x 1000 pixels)
   * are not too small (> 100 x 100 pixels)
   * cover 95% of the bounding box at least (so rectangular shapes)

   ::

        auto pred = [&prop_map](int x) {
            int sz = prop_map[x].count();
            return sz > (0.95 * prop_map[x].area_bbox()) && sz > 10000 and sz < 1000000 and prop_map[x].variance() > 250;
        };

        // Filter
        t.filter(mln::morpho::CT_FILTER_DIRECT, node_map, pred);

5. Reconstruct the image::

    t.values[0] = 0; // Set the root gray level to 0
    auto out = t.reconstruct(node_map);


.. image:: /snippets/images/roadsigns-square-2.png

Full code
^^^^^^^^^

.. literalinclude:: /snippets/component_tree_1.cpp