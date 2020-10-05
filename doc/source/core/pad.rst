Padding
#######


Include :file:`<mln/core/extension/padding.hpp>`

.. cpp:namespace:: mln


This file provides functions to pad a rectangular buffer or an image either:

* inplace: the padding region is defined by some amount of border pixels
* in an output buffer: the region is copied and the missing values are filled
  with the padding policies described below.

.. figure:: /figures/core/padding-1.svg
            :align: center

            Copy the gray area to an output image and fill the missing values according to some padding policies


.. list-table::

   * - .. image:: /figures/core/padding-5.svg
     - .. image:: /figures/core/padding-3.svg
     - .. image:: /figures/core/padding-4.svg
     - .. image:: /figures/core/padding-2.svg

   * - Padding with a constant value
     - Padding by edge mirroring
     - Padding by edge replication
     - Padding by wraping

.. cpp:enum:: e_padding_mode

   Enum that defines the padding strategies for regular grids. Example is given for
   a 1D array `[0 0 1 2 3 0 0 0 0]`, borders = `{2, 4}`

   .. cpp:enumerator:: PAD_ZERO

      pad each dimension with value zero `[0 0 1 2 3 0 0 0 0]`

   .. cpp:enumerator:: PAD_CONSTANT

      pad each dimension with a value (here *9*) `[9 9 1 2 3 9 9 9 9]`

   .. cpp:enumerator:: PAD_WRAP

      pad each dimension using periodization `[2 3 1 2 3 1 2 3 1]`

   .. cpp:enumerator:: PAD_MIRROR

      pad each dimension using edge mirroring `[2 1 1 2 3 3 2 1 1]`

   .. cpp:enumerator:: PAD_REPLICATE

      pad each dimension using edge replication `[1 1 1 2 3 3 3 3 3]`




.. cpp:function:: void pad(Image image, e_padding_mode mode, const int borders[][2], T value = {})
                  template <class T, std::size_t dim> \
                  void pad(const ranges::mdspan<T, dim>& buffer, e_padding_mode mode, const int borders[][2], T value = {})

                  Pad an image or a buffer inplace with the given borders. The `borders` parameter is a `dim × 2` that
                  contains the left/right border width in each dimension that will be filled accorded to the padding policy.


                  .. list-table::

                     * - .. image:: /figures/core/padding-inplace-1.svg
                       - .. image:: /figures/core/padding-inplace-2.svg
                     * - Before padding
                       - After padding



                  :param image: The input ndimage (or ndimensional buffer).
                  :param mode: The padding policy
                  :param borders: A `dim × 2` array containing the border widths in each dimension
                  :param value: The value used to fill the array (used if mode is `PAD_CONSTANT`



                  .. rubric:: Example

                  .. code-block:: cpp

                     #include <mln/core/extension/padding.hpp>


                     int borders[2][2] = {{2,1}, // 1st (x-) dimension
                                          {3,2}} // 2nd (y-) dimension
                     auto input = ...;
                     mln::pad(input, mln::PAD_CONSTANT, borders, 69);



.. cpp:function:: void copy_pad(Image input, Image output, e_padding_mode mode,  T value = {})
                  template <class T, int dim> \
                  void copy_pad(ranges::mdspan<T, dim> input, ranges::mdspan<T, dim> out, mln::ndbox<dim> roi, e_padding_mode mode,  T value = {})

                  Copy an image or a buffer to an external image and fill the missing regions according to the padding
                  policy.


                  .. list-table::

                     * - .. image:: /figures/core/padding-1.svg
                       - .. image:: /figures/core/padding-5.svg
                     * - Before padding
                       - After padding



                  **First version**

                  :param input: The input ndimage (or ndimensional buffer).
                  :param output: The input ndimage (or ndimensional buffer).
                  :param mode: The padding policy
                  :param value: The value used to fill the array (used if mode is `PAD_CONSTANT`

                  **Second version**

                  :param input: The input ndimensional buffer.
                  :param out: The output ndimensional buffer.
                  :param roi: The roi of the output buffer
                  :param mode: The padding policy
                  :param value: The value used to fill the array (used if mode is `PAD_CONSTANT`



                  .. rubric:: Example

                  .. code-block:: cpp

                     #include <mln/core/extension/padding.hpp>

                     mln::box2d roi = {0,0,12,12};
                     mln::box2d roi = {-2,6,12,12};

                     mln::image2d<uint8_t> input(iroi);
                     mln::image2d<uint8_t> out(oroi);

                     mln::copy_and_pad(input, out, mln::PAD_CONSTANT, 69);



