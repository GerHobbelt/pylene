Python Module
=============

.. warning:: This module is under development.

Introduction
------------

To make an easier use of the Pylene library, we provide a python module.
To use it, you just need to import the module as follows :

>>> import pylena

Conversion between ``mln::ndbuffer_image`` and NumPy array
------------------------------------------------------------

The process of conversion from ``mln::ndbuffer_image`` to NumPy array and vice versa is processed at runtime, such that the
user does not need to perform any action, thanks to Pybind11 ``type_caster``. This type caster is implemented in the :file:`pln/core/image_cast.hpp` file.

.. important:: **For developper**: according to the Pybind11 documentation (see `here <https://pybind11.readthedocs.io/en/stable/advanced/cast/custom.html>`_),
               for each compile unit in the ``pylena`` module, the header file containing the ``type_caster`` should be included. If not, undefined behavior could happen.