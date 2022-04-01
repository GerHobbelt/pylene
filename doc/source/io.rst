IO Module
=========

Pylene provides some input/output utilities, which are described below. To this
aim, it uses some third-party libraries, which are **only** used to read and
write images.

Print image
***********

Include :file:`<mln/io/imprint.hpp>`

.. cpp:namespace:: mln::io

.. cpp:function:: template <class I> \
                  void imprint(I&& image, bool print_border = false)

    Display an image as characters in the output stream. It is based on the
    `{fmt} <https://fmt.dev/latest/index.html>`_ library so the image can be
    printed only if a formatter for the image value is defined.

    :tparam I: The image type
    :param image: The image to print
    :param print_border: Boolean to print the border if any (default: ``false``)

Freeimage plugin
****************

A plugin to read and write some common image format is provided and relies on
the `FreeImage <https://freeimage.sourceforge.io/>`_ library. This plugin is
contained in the component ``Pylene::IO-freeimage``, which depends on
``Pylene::Core``. When using it, do not forget to add it in the CMakeLists.txt
as below:

.. code-block:: cmake

    target_link_libraries(my_executable PRIVATE Pylene::IO-freeimage)

Reading image
-------------

The input type handled by the FreeImage plugin for reading are the following:

* ``unsigned int`` on 8 bits.
* ``[unsigned] int`` (on 16 and 32 bits).
* Floating point values (``float`` and ``double``).
* Boolean values for binary images (``bool``).
* Color values on 24 bits (``RGB`` triplet on 8 bits each).

Include :file:`<mln/io/imread.hpp>`

.. cpp:namespace:: mln::io

.. cpp:function:: void imread(const std::string& filename, mln::ndbuffer_image& out)

    Reads an image located at ``filename`` and stores its content in the buffer ``out``.

    :param filename: The filename to an image
    :param out: The output image

    :exception std::runtime_error: When the file format is not supported by FreeImage, when the image \
                                   located at ``filename`` does not exists, when the input type is    \
                                   not handled by the plugin or in case static image type are used    \
                                   (``mln::ndimage<T>``), where the static type of the image does not \
                                   correspond to the type of the image stored in the file. 

**Example**

::

    #include <mln/core/image/ndimage.hpp>
    #include <mln/io/imread.hpp>

    ...

    mln::image2d<std::uint8_t> ima;
    mln::io::imread("/path/to/an/image", ima);

.. cpp:namespace:: mln::io

.. cpp:function:: mln::ndbuffer_image imread(const std::string& filename)

    Reads an image located at ``filename`` and returns it.

    :param filename: The filename to an image
    :return: A dynamic buffer with the image contained in.

    :exception std::runtime_error: When the file format is not supported by FreeImage, when the image \
                                   located at ``filename`` does not exists, when the input type is    \
                                   not handled by the plugin or in case static image type are used    \
                                   (``mln::ndimage<T>``), where the static type of the image does not \
                                   correspond to the type of the image stored in the file.

**Example**

::

    #include <mln/core/image/ndimage.hpp>
    #include <mln/io/imread.hpp>

    ...

    auto ima = mln::io::imread("/path/to/an/image");

    // Dynamic handling of the type
    if (ima.sample_type() == mln::sample_type_id::UINT8)
    {
        auto casted = ima.cast_to<std::uint8_t, 2>();
        if (casted) // casted is a pointer. Returns nullptr in case of wrong conversion
            my_algorithm(*casted);
    }
    else if (ima.sample_type() == mln::sample_type_id::RGB8)
    {
        ...
    }
    else
    {
        throw std::invalid_argument("Image format not handled");
    }

Writing images
--------------

Include :file:`<mln/io/imsave.hpp>`

.. cpp:namespace:: mln::io

.. cpp:function:: template <class I> \
                  void imsave(I&& ima, const std::string& filename)

.. cpp:function:: template <class I> \
                  void imsave(I&& ima, const char* filename)

    Save the image ``ima`` at ``filename``

    :tparam I: The output image type (should be defined on a 2D cubical domain)
    :param ima: The image to save
    :param filename: The filename of the output image file
    :exception std::runtime_error: When the image is not supported for writing (domain or value space), \
                                   when the file format is not supported or when the image has not been saved.

                                
**Example**

::

    #include <mln/io/imsave.hpp>

    ...

    auto ima = ...
    mln::io::imsave(ima, "/path/to/the/output/image");

CFITSIO plugin
**************

A common file format for astronomy images is the `Flexible Image Transport
System <https://heasarc.gsfc.nasa.gov/docs/heasarc/fits.html>`_ (FITS). The
`CFITSIO <https://heasarc.gsfc.nasa.gov/docs/software/fitsio/fitsio.html>`_
library handles the reading and the writing of these image. The Pylene library
has a plugin **only to read these images** around CFITSIO. **It only handles
standard FITS file** (this can be check using `FITSVERIFY
<https://heasarc.gsfc.nasa.gov/docs/software/ftools/fitsverify/>`_.

Reading images
--------------

Include :file:`<mln/io/fits/imread.hpp>`

.. cpp:namespace:: mln::io::fits

.. cpp:function:: mln::ndbuffer_image imread(const std::string& filename, int ind=0)

    Read a FITS file located at ``filename``  and return the image in the HDU indexed at ``ind``.

    :param filename: The filename of the FITS file
    :param ind: The index of the HDU containing the image
    :return: An image
    :exception std::runtime_error: When the file is incorrect, when the index ``ind`` is incorrect, \
                                   when the HDU at ``ind`` is not an image or when the number of    \
                                   dimension is not handled (should be in [1 - 4]).

.. cpp:function:: void imread(const std::string& filename, mln::ndbuffer_image& out, int ind=0)

    Read a FITS file located at ``filename``  and store the image in the HDU indexed at ``ind`` in the image ``out``.

    :param filename: The filename of the FITS file
    :param out: The output image
    :param ind: The index of the HDU containing the image
    :exception std::runtime_error: When the file is incorrect, when the index ``ind`` is incorrect, \
                                   when the HDU at ``ind`` is not an image or when the number of    \
                                   dimension is not handled (should be in [1 - 4]).