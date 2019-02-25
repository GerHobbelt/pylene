Pont statique dynamique
#######################


Pour simplifier le problème, on suppose qu'on a ce type générique d'image:

::

    template <class T>
    class image2d
    {
        public:
            // Default constructed empty image
            image2d();

            // Default-initialized image of size: width x height 
            image2d(int width, int height);

            // Image of size width x height initialized with \p val 
            image2d(int width, int height, T val);

            // Return the range of values
            std::span<T> values();
            std::span<const T> values() const;

            // External non-owning constructor
            static image2d<T> from_buffer(T* buffer, int width, int height);

            T* buffer();
            const T* buffer();
            int width() const;
            int height() const;

        private:
            // Exposition only
            std::shared_ptr<image_data_t<T>> m_data;
            T*                               m_buffer;
    };

et un algorithme générique::

    template <class T>
    image2d<float> stretch(const image2d<T>&);



L'objectif est d'être capable d'appeler cette routine depuis python avec des types natifs::

    f = np.randint((15,10))
    g = pylene.stretch(f)


Vcsn's approach: JIT
====================

.. image:: /figures/rumination/jit.svg
    :width: 75%
    :align: center


where `any_image` is an opaque type roughly defined as::

    class any_image
    {
        public:
            std::type_info type() { return m_image.type(); }
            T& cast_to<T>() { return std::any_cast<T&>(m_image); }

        std::any m_image;
    };



L'implementation de l'algorithm `stretch` s'apparente alors à::

    any_image stretch(any_image f)
    {
        std::type_info t = f.type();
        std::function<any_image(const any_image&)> f = COMPILE(stretch<T>) WITH T=t
        return fun(f); 
    }    

With COMPILE that generates (as text [1]_), compiles the C++ function::

    any_image stretch_%t(const any_image& f)
    {
        image2d<%T> g = f.cast_to<%T>();
        auto h = stretch(g);
        return {h};
    }

into a shared library and returns the function as a pointer to function (with dlopen).

.. [1] '%t' and '%T' are to be replaced by the typeid and the typename.


`any_image` is opaque. It cannot be manipulated in Python, but can only be generated and manipulated to the c++ in the
c++ workspace. In the Python's workspace, we can only call c++ binded function with this type.

The problems here:

* we haven't got numpy bindings
* we need to embed a compiler

The pros:

* performance


Matlab's approach : Coercision polymorphism
===========================================

.. image:: /figures/rumination/coercision.svg
    :width: 75%
    :align: center

The idea is to perform a conversion to a larger type able to represent all the others.


Any 2d ndarray can be converted dynamically to `image2d<double>`. And any `image2d<T>` can be represented by a ndarray.
We have the following utilities (whose implementation is straightforward)::

    image2d<double> convert(ndarray X);
    template <class T> ndarray as(image2d<T> X);

`Convert` would implies a copy, while `as` is just buffer transfer between two image representations.

Thus, to bind the function `stretch`, one would imlement::

    ndarray stretch(ndarray X)
    {
        image2d<double> Y = convert(X);
        auto Z = strech(Y);
        return as(Z);
    }



The problems here:

FIXME

The pros:

FIXME

