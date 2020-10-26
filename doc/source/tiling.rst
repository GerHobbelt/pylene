Tiling
######

A technique in image processing when rather than processing the entire image
at once, you instead split the image up in smaller pieces often called "tiles" which
you then process one by one before putting back into the image.

.. figure:: /figures/core/tiling.png
            :align: center

            Tiling concept

This process can allow better memory management, for instance by limiting what can
be loaded in order to never exceed memory limits.
In the case of parallel algorithms in Pylene, tiling is used to split the image into
different tiles before feeding those tiles to different threads.