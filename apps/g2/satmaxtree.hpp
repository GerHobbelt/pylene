#ifndef SATMAXTREE_HPP
# define SATMAXTREE_HPP

# include <mln/core/image/image2d.hpp>
# include <mln/morpho/component_tree/component_tree.hpp>

namespace mln
{

  std::pair<
    morpho::component_tree<unsigned, image2d<unsigned> >,
    property_map<morpho::component_tree<unsigned, image2d<unsigned> >, uint16>
    >
  satmaxtree(const image2d<uint16>& f);
}



#endif // ! SATMAXTREE_HPP

