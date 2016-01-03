#ifndef  APPS_PYGUI_TOSGUI_HPP
# define  APPS_PYGUI_TOSGUI_HPP
# include <boost/python.hpp>

namespace mln
{

  PyObject*
  loadtree(const char* path);

  PyObject*
  filtertree(PyObject* image,
             PyObject* parent,
             PyObject* nodemap,
             PyObject* nodelist);

}

#endif // !APPS_PYGUI_TOSGUI_HPP
