======
Images
======

operator== return a point-wise view where == comp is done on each pixel.

Images defined by a vector of point -> value are not accessible (ima(p))

Forsee to add pdim (point) and vdim (value) for dimension constant in iamge concept

.. toctree::
   :hidden:

   values
   points
   pixels
   domains
   indexes
   view/transform


.. contents::
   :local:





.. raw:: html

         <h2>Values</h2>


:doc:`core/values` are the fundamentals tools to model values held by images.


.. topic:: Concepts

   * :ref:`Value <concept-values-Value>`
   * :ref:`ComparableValue <concept-values-ComparableValue>`
   * :ref:`OrderedValue <concept-values-OrderedValue>`

.. topic:: Predefined values

   * :doc:`core/values/rgba`

.. topic:: Utilities

   TODO channel utilities
..   * :cpp:func:`is_multichannel`
..   * :cpp:func:`dynamic_channel_get`
..   * :cpp:func:`dynamic_channel_size`
..   * :cpp:func:`static_channel_get`
..   * :cpp:func:`static_channel_size`



.. raw:: html

         <h2>Points</h2>


:doc:`core/points` are the fundamentals tools to locate a value inside an images.


.. topic:: Concepts

   * :ref:`Point <concept-points-Point>`




.. raw:: html

         <h2>Pixels</h2>


:doc:`core/pixels` are the fundamentals tools when browsing an images to access both the point and the value.


.. topic:: Concepts

   * :ref:`Pixel <concept-pixels-Pixel>`




.. raw:: html

         <h2>Domains</h2>


:doc:`core/domains` are the fundamental tools when expressing the valid set of points for an image.


.. topic:: Concepts

   * :ref:`Domain <concept-domains-Domain>`




.. raw:: html

         <h2>Indexes</h2>


:doc:`core/indexes` are the lowest level tools to locate a value inside an images.


.. topic:: Concepts

   * :ref:`Index <concept-indexes-Index>`

Concepts
--------


.. cpp:namespace:: mln::concepts

.. _concept-images-Image:

Image
#####


.. _concept-images-IndexableImage:

IndexableImage
##############


.. _concept-images-AccessibleImage:

AccessibleImage
###############


.. _concept-images-ReversibleImage:

ReversibleImage
###############


.. _concept-images-RandomAccessImage:

RandomAccessImage
#################


.. _concept-images-ExtendedImage:

ExtendedImage
#############


.. _concept-images-ChValueImage:

ChValueImage
############


.. _concept-images-RawImage:

RawImage
########


.. _concept-images-WritableImage:

WritableImage
#############


.. _concept-images-ViewImage:

ViewImage
#########


.. _concept-images-ConcreteImage:

ConcreteImage
#############




Predefined images
-----------------

.. toctree::

   images/ndimage




Concepts detail
---------------

.. toctree::
   
   concepts/images
