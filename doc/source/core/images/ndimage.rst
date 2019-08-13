N-dimensional buffer images
===========================

.. contents::
   :local:


Include:
    * :file:`<mln/core/image/ndbuffer_image.hpp>` for ``ndbuffer_image``
    * :file:`<mln/core/image/ndimage.hpp>` for ``ndimage<T>``
    * :file:`<mln/core/image/ndimage_fwd.hpp>` for forward declarations.

.. cpp:namespace:: mln

N-dimensional buffer images :cpp:class:`__ndbuffer_image` 
are the most common image types that stores elements within a buffer.

Memory layout and memory management
***********************************

:cpp:class:`__ndbuffer_image` supports strided access (this is necessary for border management and clipping operations)
but ensures that elemnents of a row are contiguous.

:cpp:class:`__ndbuffer_image` can own the buffer or can borrow data from an external buffer (see
:cpp:func:`__ndbuffer_image::from_buffer`). In both case :cpp:class:`__ndbuffer_image` has reference semantics on
the owned buffer. It means that copying the image results in a shallow copy. The memory is automatically freed when
the last reference goes out.


.. rubric:: Dynamic vs Static image and Type-erasure

While the type of the element `T` and the number of dimension `pdim` are
known at compile-time, :cpp:class:`__ndbuffer_image` supports partial type and dimension erasure.

+------+--------------+---------------------------+------------------------+
|  T   |     pdim     |        Type alias         |      Description       |
+======+==============+===========================+========================+
| void | dynamic = -1 | :cpp:any:`ndbuffer_image` | Full type-erased image |
+------+--------------+---------------------------+------------------------+
| T    | 1            | :cpp:any:`image1d\<T>`    | 1d image type          |
+------+--------------+---------------------------+------------------------+
| T    | 2            | :cpp:any:`image2d\<T>`    | 2d image type          |
+------+--------------+---------------------------+------------------------+
| T    | 3            | :cpp:any:`image3d\<T>`    | 3d image type          |
+------+--------------+---------------------------+------------------------+
| T    | N            | :cpp:any:`ndimage\<T, N>` | nd image type          |
+------+--------------+---------------------------+------------------------+

.. cpp:type:: ndbuffer_image = __ndbuffer_image<void, dynamic>
.. cpp:type:: template <class T> image1d<T> = __ndbuffer_image<T, 1>
.. cpp:type:: template <class T> image2d<T> = __ndbuffer_image<T, 2>
.. cpp:type:: template <class T> image3d<T> = __ndbuffer_image<T, 3>

Border management
*****************

FIXME. to be documented


Overview of the n-dimensional image API
***************************************


.. list-table:: Overview of the n-dimensional image API
    :width: 150%
    :widths: 1 10 10
    :header-rows: 1
    :stub-columns: 1
    :class: table-synopsis

    *   - 
        - ``__ndbuffer_image<void, dynamic>``
        - ``__ndbuffer_image<T, N>``
    *   - Constructors
        - .. cpp:alias:: __ndbuffer_image<void, dynamic>::__ndbuffer_image
        - .. cpp:alias:: template <class T, int N> __ndbuffer_image<T, N>::__ndbuffer_image
    *   - Constructors from external sources
        - .. cpp:alias:: __ndbuffer_image<void, dynamic>::from_buffer
        - .. cpp:alias:: template <class T, int N> __ndbuffer_image<T, N>::from_buffer
    *   - Resizing
        - .. cpp:alias:: __ndbuffer_image<void, dynamic>::resize
        - .. cpp:alias:: template <class T, int N> __ndbuffer_image<T, N>::resize
    *   - Geometry information
        - .. cpp:alias:: __ndbuffer_image<void, dynamic>::pdim
                         __ndbuffer_image<void, dynamic>::width
                         __ndbuffer_image<void, dynamic>::height
                         __ndbuffer_image<void, dynamic>::depth
                         __ndbuffer_image<void, dynamic>::size
                         __ndbuffer_image<void, dynamic>::border
                         __ndbuffer_image<void, dynamic>::domain
        - .. cpp:alias:: template <class T, int N> __ndbuffer_image<T, N>::pdim
                         template <class T, int N> __ndbuffer_image<T, N>::width
                         template <class T, int N> __ndbuffer_image<T, N>::height
                         template <class T, int N> __ndbuffer_image<T, N>::depth
                         template <class T, int N> __ndbuffer_image<T, N>::size
                         template <class T, int N> __ndbuffer_image<T, N>::border
                         template <class T, int N> __ndbuffer_image<T, N>::domain

    *   - Data & Layout information
        - .. cpp:alias:: __ndbuffer_image<void, dynamic>::sample_type
                         __ndbuffer_image<void, dynamic>::buffer
                         __ndbuffer_image<void, dynamic>::byte_stride
                         __ndbuffer_image<void, dynamic>::stride
                         __ndbuffer_image<void, dynamic>::index_of_point
                         __ndbuffer_image<void, dynamic>::point_at_index
                         __ndbuffer_image<void, dynamic>::delta_index      
        - .. cpp:alias:: template <class T, int N> __ndbuffer_image<T, N>::sample_type
                         template <class T, int N> __ndbuffer_image<T, N>::buffer
                         template <class T, int N> __ndbuffer_image<T, N>::byte_stride
                         template <class T, int N> __ndbuffer_image<T, N>::stride
                         template <class T, int N> __ndbuffer_image<T, N>::index_of_point
                         template <class T, int N> __ndbuffer_image<T, N>::point_at_index
                         template <class T, int N> __ndbuffer_image<T, N>::delta_index 
    *   - Data accessors
        - .. cpp:alias:: __ndbuffer_image<void, dynamic>::operator()
                         __ndbuffer_image<void, dynamic>::at
                         __ndbuffer_image<void, dynamic>::operator[]
        - .. cpp:alias:: template <class T, int N> __ndbuffer_image<T, N>::operator()
                         template <class T, int N> __ndbuffer_image<T, N>::at
                         template <class T, int N> __ndbuffer_image<T, N>::operator[]
    *   - Slicing & clipping operations
        - .. cpp:alias::  __ndbuffer_image<void, dynamic>::clip
                          __ndbuffer_image<void, dynamic>::row
                          __ndbuffer_image<void, dynamic>::slice 
        - .. cpp:alias::  template <class T, int N> __ndbuffer_image<T, N>::clip
                          template <class T, int N> __ndbuffer_image<T, N>::row
                          template <class T, int N> __ndbuffer_image<T, N>::slice
    *   - Casting operators
        - .. cpp:alias:: __ndbuffer_image<void, dynamic>::cast_to
        - Implicitely convertible to :cpp:any:`ndbuffer_image`
    *   - Iteration facilities
        -
        - .. cpp:alias:: template <class T, int N> __ndbuffer_image<T, N>::values()
                     template <class T, int N> __ndbuffer_image<T, N>::pixels()


API Reference of the dynamic N-dimensional buffer images (`ndbuffer_image`)
***************************************************************************

.. cpp:class:: template <> __ndbuffer_image<void, dynamic>

    .. rubric:: Type definitions

    .. cpp:type:: point_type = Point
                  value_type = void*
                  index_type = int 
                  domain_type = Box


    .. rubric:: Constructors

    .. cpp:alias:: __ndbuffer_image

    .. rubric:: Construction from external buffers

    .. cpp:alias:: from_buffer

    .. rubric:: Resizing facilities

    .. cpp:alias:: resize


    .. rubric:: Geometry information

    .. cpp:alias:: pdim
                   width
                   height
                   depth
                   size
                   border
                   domain

    .. rubric:: Data and layout information

    .. cpp:alias:: sample_type
                    buffer
                    byte_stride
                    stride
                    index_of_point
                    point_at_index
                    delta_index

    .. rubric:: Data accessors

    .. cpp:alias:: operator()
                   at
                   operator[]

    .. rubric::  Slicing & clipping operations

    .. cpp:alias::  clip
                    row
                    slice

    .. rubric:: Casting operators

    .. cpp:alias:: cast_to


    .. rubric:: Constructors

    .. cpp:function:: __ndbuffer_image()

        Creates a empty image

    .. cpp:function:: __ndbuffer_image(sample_type_id sample_type, ConstBoxRef domain, const image_build_params& = {})
                    __ndbuffer_image(sample_type_id sample_type, int width, const image_build_params& params = {})
                    __ndbuffer_image(sample_type_id sample_type, int width, int height, const image_build_params& params = {})
                    __ndbuffer_image(sample_type_id sample_type, int width, int height, int depth, const image_build_params& params = {})
                    

        Creates an image of dimensions given by `domain` with the given `sample_type`. The overloads are provided for convenience:
        
        2. Creates a 1d image of the given `width`
        3. Creates a 2d image of size `width × height`
        4. Creates a 3d image of size `width × height × depth`

        By default, the memory is left default-initialized.
        The optional `params` parameter can be used to provide advanced initialization information:
            
        * `params.init_value` can be used to value-initialize the buffer
        * `params.border` can be used to allocate the image with a given border size. 

        .. code-block::

            // Create a 3d image of size (width=2, height=3, depth=4) with type uint8_t and default border width (3)
            // and random values
            ndbuffer_image a(sample_type_id::UINT8, 2, 3, 4);
            

            // Create a 2d image of size (width=5, height=5) with type uint16_t and border width = 5px
            // with values set to zero.
            image_build_params params;
            params.init_value = uint16_t{0};
            params.border = 5
            ndbuffer_image b(sample_type_id::UINT16, 5, 5, params);




    .. cpp:function:: __ndbuffer_image(const __ndbuffer_image& other, const image_build_params&)

        Initialization constructor 


    .. cpp:function:: template <class T> __ndbuffer_image(std::initializer_list<T>)
                      template <class T> __ndbuffer_image(std::initializer_list<std::initializer_list<T>>)
                      template <class T> __ndbuffer_image(std::initializer_list<std::initializer_list<std::initializer_list<T>>>)

        Constructor from initializer lists (1d, 2d or 3d images).::

            mln::ndbuffer_image f = {{1,2,3}, {4,5,6}};
            // Extents: (width=3 x height=2) of type: INT32

            mln::ndbuffer_image g = { {{1.f,2.f,3.f}, {4.f,5.f,6.f}}, {{7.f,8.f,9.f}, {10.f,11.f,12.f}} };
            // Extents (width=3 x height=2 x depth=2) of type FLOAT

    .. rubric:: Construction from external buffers

    .. cpp:function:: static ndbuffer_image from_buffer(std::byte* buffer, sample_type_id sample_type, int dim, const\
            int sizes[], const std::ptrdiff_t byte_strides[] = nullptr, bool copy = false)                                       
            static ndbuffer_image from_buffer(std::byte* buffer, sample_type_id sample_type, int dim, const\
            int topleft[], const int sizes[], const std::ptrdiff_t byte_strides[] = nullptr, bool copy = false)

            Constructs an image using an external buffer.

            :param buffer: Pointer to the buffer
            :param sample_type: Type of elements
            :param dim: Number of dimensions
            :param topleft: Pointer to an array of `dim` elements holding the coordinates of the top-left corner (x, y, z,
                ...) In (1), it is assumed to be (0, 0, 0,...)
            :param sizes: Pointer to an array of `dim` elements holding the sizes of the image (width, height, depth, ...)
            :param byte_strides (optional): Pointer to an array of `dim` elements holding the strides between consecutive
                elements in each dimension (in bytes). If `NULL`, strides are computed assuming the data are stored contiguously.
            :param copy (optional): If true, a copy of the buffer is done and managed internally. Otherwise, the buffer is
                not copied and the user is responsible for its destruction.

    .. rubric:: Resizing facilities

    .. cpp:function:: void resize(const __ndbuffer_image& other, image_build_params params)
                      void resize(sample_type_id sample_type, int width, const image_build_params& = {})
                      void resize(sample_type_id sample_type, int width, int height, const image_build_params& = {})
                      void resize(sample_type_id sample_type, int width, int height, int depth, const image_build_params& = {})
                      void resize(sample_type_id sample_type, ConstBoxRef domain, const image_build_params& = {})

        See the corresponding contructors. These functions allow an image to be default-constructed and resized afterward.
        
        .. note::
            A new buffer is allocated. If a buffer was already attached to the image and this is the last
            reference, the memory is reclaimed.

    .. rubric:: Geometry information

    .. cpp:function:: int pdim() const noexcept

        Get the number of dimensions of the image

    .. cpp:function:: int width() const noexcept

        Get the width of the image (0 if empty)

    .. cpp:function:: int height() const noexcept

        Get the height of the image (0 if empty). Returns 1 if the image is 1D. 


    .. cpp:function:: int depth() const noexcept

        Get the depth of the image (0 if empty). Returns 1 if the image is 1D or 2D.

    .. cpp:function:: int size(int dim = 0) const noexcept

        Get the number of elements in the given dimension (0 if empty). Returns 1 if `dim >= pdim()`.

    .. cpp:function:: int border() const noexcept

        Get the size of the border.

    .. cpp:function:: Box domain() const noexcept

        Get the domain of the image.

    .. rubric:: Data and layout information

    .. cpp:function:: sample_type_id                sample_type() const noexcept

        Get the sample type of the data.
    
    .. cpp:function:: std::byte*                    buffer() const noexcept

        Get a pointer to first element (in the domain).

    .. cpp:function:: std::ptrdiff_t                byte_stride(int dim = 1) const noexcept

        Get the stride (in bytes) between two consecutive elements in the given dim.

    .. cpp:function:: std::ptrdiff_t                stride(int dim = 1) const noexcept

        Get the stride (in number of elements) between two consecutive elements in the given dim.

    .. cpp:function:: index_type  index_of_point(ConstPointRef p) const noexcept

        Get the linear index (offset in the buffer) of multi-dimensional point.
 
    .. cpp:function:: point_type  point_at_index(index_type i) const noexcept

        Get the point corresponding to the given index.

    .. cpp:function:: index_type  delta_index(ConstPointRef p) const noexcept

        Get the linear index offset for the given point.


    .. rubric::  Slicing & clipping operations

    .. cpp:function:: ndbuffer_image slice(int z) const

        Return the slice at coordinate `z` in the 3th dimension.

        :except: std::runtime_error if `y` in invalid or `dim() != 3`.

    .. cpp:function:: ndbuffer_image row(int y) const
    
        Return the row at coordinate `y` in the 2nd dimension.

        :except: std::runtime_error if `y` in invalid or `dim() != 2`.
    
    .. cpp:function:: ndbuffer_image clip(ConstBoxRef roi) const

        Return the image restricted to the ROI `roi`. `roi` must be included in the domain. 

        :except: std::runtime_error if ``domain().includes(roi)`` is false or dimensions mismatch.


    .. rubric:: Data accessors

    .. cpp:function:: const void* operator() (ConstPointRef p) const noexcept
                      void* operator() (ConstPointRef p) noexcept
 
        Returns a pointer to the element at `p`.
        
        **Precondition**:  ``this->domain().has(p)``

    .. cpp:function:: const void* at (ConstPointRef p) const noexcept
                      void* at (ConstPointRef p) noexcept
 
        Returns a pointer to the element at `p`. `p` can be in the extension.
        
        **Precondition**:  `p` belongs to the extended domain.

    .. cpp:function:: const void* operator[] (index_type i) const noexcept
                      void* operator[] (index_type i) noexcept
 
        Returns a pointer to the element at index `i`.

        **Precondition**: `i` must be a valid index.


    .. rubric:: Casting operators

    .. cpp:function:: template <class T, int N>\
                        const __ndbuffer_image<T, N>* cast_to() const
                      template <class T, int N>\
                        __ndbuffer_image<T, N>* cast_to()

        Down-cast (or trans-cast) to the requested n-dimensional image type. Returns `nullptr` if the requested types do
        not match the dynamic type information.

        .. code-block::

            // Create a 3d image of size (width=2, height=3, depth=4) with type uint8_t and default border width (3)
            // and random values
            ndbuffer_image a(sample_type_id::UINT8, 2, 3, 4);

            image3d<uint8_t>* b1 = a.template cast_to<uint8_t, 3>(); // Ok
            image2d<uint8_t>* b2 = a.template cast_to<uint8_t, 2>(); // Fails (null pointer returned)

API Reference of the static N-dimensional buffer images (`ndimage<T, N>`)
*************************************************************************

.. cpp:class:: template <class T, int N> __ndbuffer_image<T, N>

    .. rubric:: Type definitions

    .. cpp:type:: point_type = ndpoint<N>
                  value_type = T
                  index_type = int
                  domain_type = ndbox<N>
                  pixel_type
                  const_pixel_type  

    .. rubric:: Constructors

    .. cpp:alias:: __ndbuffer_image

    .. rubric:: Construction from external buffers

    .. cpp:alias:: from_buffer

    .. rubric:: Resizing facilities

    .. cpp:alias:: resize


    .. rubric:: Geometry information

    .. cpp:alias:: pdim
                   width
                   height
                   depth
                   size
                   border
                   domain

    .. rubric:: Data and layout information

    .. cpp:alias:: sample_type
                    buffer
                    byte_stride
                    stride
                    index_of_point
                    point_at_index
                    delta_index

    .. rubric:: Data accessors

    .. cpp:alias:: operator()
                   at
                   operator[]

    .. rubric::  Slicing & clipping operations

    .. cpp:alias::  clip
                    row
                    slice

    .. rubric:: Casting operators

    Implcitely convertible to :cpp:any:`ndbuffer_image`.

    .. rubric:: Iteration

    .. cpp:alias:: values
                   pixels


    .. rubric:: Constructors

    .. cpp:function:: __ndbuffer_image()

        Creates a empty ``N``-d image with sample type ``T``

    .. cpp:function:: __ndbuffer_image(ndbox<N> domain, const image_build_params& = {})
                    [[when N = 1]] __ndbuffer_image(int width, const image_build_params& params = {})
                    [[when N = 2]] __ndbuffer_image(int width, int height, const image_build_params& params = {}) 
                    [[when N = 3]] __ndbuffer_image(int width, int height, int depth, const image_build_params& params = {})
                    

        Creates an image of dimensions given by `domain` with value type given by ``T``. The overloads are provided for
        convenience and availability depends on ``N``:
        
        2. **When N = 1** Creates a 1d image of the given `width`
        3. **When N = 2** Creates a 2d image of size `width × height`
        4. **When N = 3** Creates a 3d image of size `width × height × depth`

        By default, the memory is left default-initialized.
        The optional `params` parameter can be used to provide advanced initialization information:
        
        * `params.init_value` can be used to value-initialize the buffer
        * `params.border` can be used to allocate the image with a given border size. 

        .. code-block::

            // Create a 3d image of size (width=2, height=3, depth=4) with type uint8_t and default border width (3)
            // and random values
            image3d<uint8_t> a(2, 3, 4);
            

            // Create a 2d image of size (width=5, height=5) with type uint16_t and border width = 5px
            // with values set to zero.
            image_build_params params;
            params.init_value = uint16_t{0};
            params.border = 5
            image2d<uint16_t> b(5, 5, params);

    .. cpp:function:: __ndbuffer_image(const __ndbuffer_image& other, const image_build_params&)

        Initialization constructor 


    .. cpp:function:: [[when N = 1]] __ndbuffer_image(std::initializer_list<T>)
                      [[when N = 2]] __ndbuffer_image(std::initializer_list<std::initializer_list<T>>)
                      [[when N = 3]] __ndbuffer_image(std::initializer_list<std::initializer_list<std::initializer_list<T>>>)

        Constructor from initializer lists. Availability depends on ``N``.

        .. code-block::

            image2d<int> = {{1,2,3}, {4,5,6}};
            // Extents: (width=3 x height=2) of type: INT32

            image3d<float> g = { {{1.f,2.f,3.f}, {4.f,5.f,6.f}}, {{7.f,8.f,9.f}, {10.f,11.f,12.f}} };
            // Extents (width=3 x height=2 x depth=2) of type FLOAT

    .. rubric:: Construction from external buffers

    .. cpp:function:: static ndbuffer_image from_buffer(T* buffer, const int sizes[], const std::ptrdiff_t byte_strides[] = nullptr, bool copy = false)                                       
            static ndbuffer_image from_buffer(T* buffer, const int topleft[], const int sizes[], const std::ptrdiff_t byte_strides[] = nullptr, bool copy = false)

            Constructs an image using an external buffer.

            :param buffer: Pointer to the buffer
            :param topleft: Pointer to an array of `dim` elements holding the coordinates of the top-left corner (x, y, z,
                ...) In (1), it is assumed to be (0, 0, 0,...)
            :param sizes: Pointer to an array of `dim` elements holding the sizes of the image (width, height, depth, ...)
            :param byte_strides (optional): Pointer to an array of `dim` elements holding the strides between consecutive
                elements in each dimension (in bytes). If `NULL`, strides are computed assuming the data are stored contiguously.
            :param copy (optional): If true, a copy of the buffer is done and managed internally. Otherwise, the buffer is
                not copied and the user is responsible for its destruction.

    .. rubric:: Resizing facilities

    .. cpp:function:: void resize(const __ndbuffer_image& other, image_build_params params)
                      void resize(int width, const image_build_params& = {})
                      void resize(int width, int height, const image_build_params& = {})
                      void resize(int width, int height, int depth, const image_build_params& = {})
                      void resize(ConstBoxRef domain, const image_build_params& = {})

        See the corresponding contructors. These functions allow an image to be default-constructed and resized afterward.
        
        .. note::
            A new buffer is allocated. If a buffer was already attached to the image and this is the last
            reference, the memory is reclaimed.


    .. rubric:: Geometry information

    .. cpp:function:: int pdim() const noexcept

        Get the number of dimensions of the image

    .. cpp:function:: int width() const noexcept

        Get the width of the image.

    .. cpp:function:: int height() const noexcept

        Get the height of the image. Returns 1 if the image is 1D. 


    .. cpp:function:: int depth() const noexcept

        Get the depth of the image. Returns 1 if the image is 1D or 2D.

    .. cpp:function:: int size(int dim = 0) const noexcept

        Get the number of elements in the given dimension. Returns 1 if `dim >= pdim()`.

    .. cpp:function:: int border() const noexcept

        Get the size of the border.

    .. cpp:function:: ndbox<N> domain() const noexcept

        Get the domain of the image.

    .. rubric:: Data and layout information

    .. cpp:function:: sample_type_id sample_type() const noexcept

        Get the sample type of the data.
    
    .. cpp:function:: T* buffer() const noexcept

        Get a pointer to first element (in the domain).

    .. cpp:function:: std::ptrdiff_t byte_stride(int dim = 1) const noexcept

        Get the stride (in bytes) between two consecutive elements in the given dim.

    .. cpp:function:: std::ptrdiff_t stride(int dim = 1) const noexcept

        Get the stride (in number of elements) between two consecutive elements in the given dim.

    .. cpp:function:: index_type  index_of_point(ndpoint<N> p) const noexcept

        Get the linear index (offset in the buffer) of multi-dimensional point.
 
    .. cpp:function:: ndpoint<N>  point_at_index(index_type i) const noexcept

        Get the point corresponding to the given index.

    .. cpp:function:: index_type  delta_index(ndpoint<N> p) const noexcept

        Get the linear index offset for the given point.



    .. rubric:: Slicing & clipping operations

    .. cpp:function:: [[when N = 3]] image2d<T> slice(int z) const noexcept

        Return the slice at coordinate `z` in the 3th dimension.

        **exceptions**: std::runtime_error if `z` is invalid

    .. cpp:function:: [[when N = 2]] image1d<T> row(int y) const noexcept
    
        Return the row at coordinate `y` in the 2nd dimension.

        **exceptions**: std::runtime_error if `y` is invalid
    
    .. cpp:function:: __ndbuffer_image<T, N> clip(ndbox<N> roi) const

        Return the image restricted to the ROI `roi`. `roi` must be included in the domain. 

        :except: std::runtime_error if ``domain().includes(roi)`` is false or dimensions mismatch.


    .. rubric:: Data accessors

    .. cpp:function:: const T& operator() (ndpoint<N> p) const noexcept
                      T& operator() (ndpoint<N> p) noexcept
 
        Returns a reference to the element at `p`.
        
        **Precondition**:  ``this->domain().has(p)``

    .. cpp:function:: const T& at (ndpoint<N> p) const noexcept
                      T& at (ndpoint<N> p) noexcept
 
        Returns a reference to the element at `p`. `p` can be in the extension.
        
        **Precondition**:  `p` belongs to the extended domain.

    .. cpp:function:: const T& operator[] (index_type i) const noexcept
                      T& operator[] (index_type i) noexcept
 
        Returns a reference to the element at index `i`.

        **Precondition**: `i` must be a valid index.

    .. rubric:: Iteration

    .. cpp:function:: auto values() const
                      auto values()
        
        Returns a range to iterate on image values.

    .. cpp:function:: auto pixels() const
                      auto pixels()

        Return a range to iterate on image pixels.
