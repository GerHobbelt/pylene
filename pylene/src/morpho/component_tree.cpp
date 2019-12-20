#include <mln/morpho/experimental/component_tree.hpp>


namespace mln::morpho::experimental
{
  void component_tree<void>::filter_direct(const std::vector<bool>& pred)
  {
    this->filter_direct_T([&pred](int x) { return pred[x]; });
  }
}
