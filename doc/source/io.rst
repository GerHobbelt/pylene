IO Module
=========

Pylene provides some input/output utilities, which are described below. To this
aim, it uses some third-party libraries, which are **only** used to read and
write images.

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