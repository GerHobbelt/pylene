#pragma once

#include <mln/morpho/component_tree/component_tree.hpp>

#include <iosfwd>


namespace mln
{

  namespace morpho
  {

    template <class P, class Amap, class PropertyMap>
    void write_graphviz(std::ostream& os, const component_tree<P, Amap>& tree, const PropertyMap& property);

    template <class P, class Amap, class Prop>
    void write_graphviz(std::ostream& os, const component_tree<P, Amap>& tree);

    /***********************************/
    /**   Implementation	      **/
    /***********************************/

    template <class P, class Amap, class PropertyMap>
    void write_graphviz(std::ostream& os, const component_tree<P, Amap>& tree, const PropertyMap& property)
    {
      os << "digraph {" << std::endl;

      mln_foreach (auto x, tree.nodes())
        os << "\t" << x.id() << " [label=\"" << property[x] << "\"];" << std::endl;

      mln_foreach (auto x, tree.nodes_without_root())
        os << "\t" << x.id() << " -> " << x.get_parent_id() << ";" << std::endl;

      os << "}" << std::endl;
    }
  } // namespace morpho
} // namespace mln
