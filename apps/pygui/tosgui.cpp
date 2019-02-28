#include <boost/python.hpp>
#define PY_ARRAY_UNIQUE_SYMBOL tosgui_ARRAY_API
#include <numpy/ndarrayobject.h>
#include "tosgui.hpp"

BOOST_PYTHON_MODULE(tosGUI_)
{
  using namespace boost::python;
  def("loadtree", mln::loadtree);
  def("filtertree", mln::filtertree);
  import_array();
}
