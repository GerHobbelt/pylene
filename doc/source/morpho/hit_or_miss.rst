Hit or Miss
===========


Include :file:`<mln/morpho/hit_or_miss.hpp>`


.. cpp:namespace:: mln::morpho

.. cpp:function:: \
    Image{I} image_concrete_t<I> hit_or_miss(I image, StructuringElement se_hit, StructuringElement se_miss)
    void hit_or_miss(Image image, StructuringElement se_hit, StructuringElement se_miss, OutputImage out)

      The hit-or-miss transform is non-linerar filter used to detect pattern in
      images. It is defined as:

      .. math::

         UHTM_\mathcal{B}(f)(x) =
         \begin{cases}
         \varepsilon_{B_{fg}}(x) - \delta_{B_{bg}}(x) & \mathrm{if} \varepsilon_{B_{fg}}(x) > \delta_{B_{bg}}(x)\newline
         0 & otherwise
         \end{cases}


      where :math:`$ \mathcal{B} = (B_{fg}, B_{bg})` is the composite
      structuring element composed by the two disjoint foreground and background
      SEs.


      * (2\) If the optional ``output`` image is provided, it must be wide enough to store
        the results (the function does not perform any resizing).

      :param ima: Input image 𝑓
      :param se_hit:  Structuring element for the foreground :math:`B_{fg}`
      :param se_miss:  Structuring element for the foreground :math:`B_{bg}`
      :param output (optional): Output image

      :return:
         * (1\) An image whose type is deduced from the input image
         * (2\) Nothing (the output image is passed as an argument)

      :exception: N/A


Notes
-----


Complexity
----------



Example 1 : Staff lines selection
---------------------------------

Hit or miss transform to detect horizontal 1px-thick line, with pattern::

  x x x
  o o o
  x x x


.. literalinclude:: /snippets/staff_lines.cpp
   :start-after: #M1_START
   :end-before: #M1_END
   :language: cpp
   :dedent: 4

.. image:: /images/staff_lines.png
           :width: 49%

.. image:: /images/morpho_hitormiss_1.png
           :width: 49%

Hit or miss transform to detect horizontal 2px-thick line, with pattern::

  x x x
  o o o
  o o o
  x x x

.. literalinclude:: /snippets/staff_lines.cpp
   :language: cpp
   :start-after: #M2_START
   :end-before: #M2_END
   :dedent: 4

.. image:: /images/staff_lines.png
           :width: 49%

.. image:: /images/morpho_hitormiss_2.png
           :width: 49%


Logical or between the two previous images::

  using mln::view::ops;
  auto markers = markers1 || markers2;

.. image:: /images/staff_lines.png
           :width: 49%

.. image:: /images/staff_lines_markers.png
           :width: 49%


