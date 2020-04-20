#pragma once

#include <boost/python.hpp>


namespace mln
{
  PyObject* loadtree(const char* path);

  PyObject* filtertree(PyObject* image, PyObject* parent, PyObject* nodemap, PyObject* nodelist);
} // namespace mln
