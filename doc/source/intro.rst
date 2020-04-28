Introduction
############

Overview
========

|Pylene| is an open-source image processing library that aims at proving a generic framework to develop IP algorithms
with safety, simplicity and performance in Mind. It is originated from Milena_ with the following objectives:


* Simplicity: simple modern C++ syntax with Python binding (in development) 
* Efficiency: write algorithms in a simple way and run them as if they were written in C .We follow one guideline: zero-cost abstraction.
* Genericity: write algorithms that are able to run on many kind of images with, yet, zero-cost abstraction.
* Interoperability: run algorithms on image coming from and to external libraries (even on your own image type).

|Pylene| features ready-to-use image data structures (regular 2D, 3D images) and focus (but is not limited to
Mathematical Morphology Operators).

Usage
=====

The library provides ready-to-use building blocks for Mathematical Morphology. If you are just interested in using and
chaining pre-built operators, just include and use the corresponding file.

.. literalinclude:: /snippets/intro-1.cpp
    :start-after: BEGIN-CODE
    :end-before: END-CODE

.. list-table::

    * - .. image:: /images/Olena-c6gradi.png
      - .. image:: /images/intro-1-1.png


Genericity
==========

A generic algorithm is defined in an abstract way so that it can be used with a wide varieties of inputs. Such
algorithms are written once, and can then be reused with data of many different types.

The previous snippet of code could be moved in its own routine to improve reusabilityCol

.. mermaid::

    graph LR
      A(Input) --> B[Negate]
      subgraph Chain
      B --> C[Closing]
      C --> D[Watershed]
      D --> E[Coloring]
      end
      E --> F(Output)
		

could have been written with more genericity as:

.. literalinclude:: /snippets/intro-1.cpp
    :start-after: BEGIN-CODE-2
    :end-before: END-CODE-2


Each input must satisfy constraints corresponding to the nature of the data expected by the algorithm. E.g. in the
previous algorithm, the first argument must be an image, the second argument must be neighborhood (compatible with the
type of point of ima), and the value type of image must be ``unsigned``, to be negated.


.. list-table::
    :width: 15cm
    :widths: 30 30 30
    :header-rows: 1
    :align: center

    * - 2D gradient image
      - Graph with distance on egdes
      - Triangle mesh (weighted with curvature information),
        seen as a simplicial 2-complex

    * - .. image:: /images/Olena-c6gradi.png
            :width: 90%
            :align: center

      - .. image:: /images/Olena-classgradi.png
            :width: 90%
            :align: center

      - .. image:: /images/Olena-bunny-holefilled.png
            :width: 90%
            :align: center

    * - .. image:: /images/intro-1-1.png
            :width: 90%
            :align: center

      - .. image:: /images/Olena-classwst2b.png
            :width: 90%
            :align: center

      - .. image:: /images/Olena-bunny-holefilled-pinv-curv-segm.png
            :width: 90%
            :align: center



Views and Lazy-computation
==========================

.. figure:: /figures/core/transform_negate.svg

   The view system allows to build lazy-evaluation images

Inspired by many most C++ mathematical libraries (Eigen, Blitz++), and following the Modern C++ trends of *Views*
(Range-v3), |Pylene| allows to build expression templates that delays the execution later in the pipeline.

Instead of calling the *transform algorithm*, we use the *transform view* to change the input and delay the evaluation
while processing ``area_closing``::

    auto negate = mln::view::transform(input, [](uint8_t x) -> uint8_t { return 255 - x; });

Arithmetic operators on images allow to build views with a simple syntax::

  auto negate = uint8_t(255) - input



Generic framework made simple
=============================

Writing a *generic* algorithm **and** efficient is made simple with |Pylene|. For example, the algorithm transform is as
simple as::

  template <mln::concepts::Image I, class F>
  auto transform(I input, F fun)
  {
     using R = std::invoke_result_t<F, mln::image_value_t<I>>;    // Deduce the value type of the output

     mln::image_ch_value_t<I, R> out = mln::imchavalue<R>(input); // Create a writtable image similar to the input
     auto zz = mln::view::zip(input, out);

     mln_foreach((auto&& [vin, vout]), zz.values())
       vout = fun(vin);

     return out;
  }

It works for every image type (graph, 2D, 3D) and is as-efficient as if we had written the specific code for each of
these types.



Comparison with other libraries
===============================

FIXME
