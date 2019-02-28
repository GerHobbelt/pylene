#define PY_ARRAY_UNIQUE_SYMBOL tosgui_ARRAY_API
#define NO_IMPORT_ARRAY
#include "loadtree.hpp"
#include <numpy/ndarrayobject.h>
#include <mln/core/image/image2d.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <iostream>
#include <fstream>

namespace mln
{

  PyObject*
  loadtree(const char* path)
  {
    std::cout << "Loading from: " << path << std::endl;

    typedef morpho::component_tree< unsigned, image2d<unsigned> > tree_t;
    tree_t tree;

    // 1. Load the tree
    std::ifstream f(path, std::ios::binary);
    if (f.good())
      morpho::load(f, tree);
    else {
      Py_INCREF(Py_None);
      return Py_None;
    }

    // A map old index -> new index
    property_map<tree_t, unsigned> new_index(tree);
    PyArrayObject* parent;

    // 2. Make the parent array
    {
      unsigned i = 0;
      npy_intp dims[2] = {0,0};
      npy_uint* buffer;

      mln_foreach(auto x, tree.nodes())
        new_index[x] = i++;

      dims[0] = i;
      parent = (PyArrayObject*) PyArray_SimpleNew(1, dims, NPY_UINT);
      buffer = (npy_uint*) PyArray_DATA(parent);

      i = 0;
      buffer[i++] = 0; // parent of root -> root
      mln_foreach(auto x, tree.nodes_without_root())
        buffer[i++] = new_index[x.parent()];
    }

    // 3. Make the node2idx image
    PyArrayObject* idxmap;
    {
      point2d shp = tree._get_data()->m_pmap.domain().shape();
      npy_intp dims[2] = {shp[0], shp[1]};
      npy_uint* buffer;

      idxmap = (PyArrayObject*) PyArray_SimpleNew(2, dims, NPY_UINT);
      buffer = (npy_uint*) PyArray_DATA(idxmap);
      mln_pixter(px, tree._get_data()->m_pmap);
      mln_forall(px)
        buffer[px->point()[0] * dims[1] + px->point()[1]] = new_index[px->val()];
    }

    // 4. Leaves array
    PyArrayObject* leaves;
    {
      unsigned sz = tree._get_data()->m_pmap.domain().size();
      npy_intp dims[2] = {sz, 2};
      npy_ushort* buffer;

      leaves = (PyArrayObject*) PyArray_SimpleNew(2, dims, NPY_USHORT);
      buffer = (npy_ushort*) PyArray_DATA(leaves);

      unsigned i = 0;
      mln_pixter(px, tree._get_data()->m_pmap);
      mln_forall(px)
      {
        auto x = tree.get_node_at(px->index());
        if (not x.has_child()) {
          buffer[i * 2] = px->point()[0];
          buffer[i * 2 + 1] = px->point()[1];
          i++;
        }
      }

      PyArray_Dims pdim = { dims, 2};
      dims[0] = i;
      std::cout << "Number of leaves: " << i << std::endl;
      PyArray_Resize(leaves, &pdim, 0, NPY_ANYORDER);
    }

    return PyTuple_Pack(3, parent, idxmap, leaves);
  }


}
