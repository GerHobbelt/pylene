First steps
===========

Installation
------------

.. important:: For now, only the ``pylena_numpy`` library is available. It makes possible to write Python packages using the ``Pylene`` library. In the future,
               the ``pylena`` module will be developped, making the ``Pylene`` algorithms available in Python and easily usable. 

Using Conan
^^^^^^^^^^^

.. code-block:: console

    $ conan install pylene/head@lrde/stable -o pylene:fPIC=True -o pylene:python=True

For developpers (Build from source)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: console

    $ git clone https://gitlab.lrde.epita.fr/olena/pylene.git
    $ cd pylene
    $ mkdir build && cd build
    $ conan install .. --build missing
    $ cmake .. -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DPYLENE_BUILD_PYTHON=ON
    $ make pylena_numpy