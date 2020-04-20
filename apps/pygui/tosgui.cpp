#define PY_ARRAY_UNIQUE_SYMBOL tosgui_ARRAY_API

#include "tosgui.hpp"

#include <boost/python.hpp>

#include <numpy/ndarrayobject.h>


BOOST_PYTHON_MODULE(tosGUI_)
{
  using namespace boost::python;
  def("loadtree", mln::loadtree);
  def("filtertree", mln::filtertree);
  import_array();
}
