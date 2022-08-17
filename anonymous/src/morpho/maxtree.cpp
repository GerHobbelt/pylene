#include <cassert>
#include <climits>
#include <cstddef>
#include <cstring>
#include <memory>

namespace ano::morpho::details
{

  // Permute the array \p tab considering the permutation array \p permut
  [[gnu::noinline]] //
  void
  permute_array(int* permut, void* tab, std::size_t count, std::size_t size)
  {
    void* tmp = std::malloc(count * size);

    for (std::size_t i = 0; i < count; ++i)
    {
      int k = permut[i];
      std::memcpy((char*)tmp + k * size, (char*)tab + i * size, size);
    }

    std::memcpy(tab, tmp, count * size);
    std::free(tmp);
  }

  /// Sort the array \c par so that par[i] < i
  /// where `par` encodes a DAG relation
  /// The parent of the root node is supposed to be -1;
  /// permut[-1] is supposed to be a valid memory location
  void permute_parent(int* parent, int* permut, std::size_t n)
  {
    // Negative values in permut will be considered as UNSEEN
    std::fill(permut, permut + n, INT_MIN);
    permut[-1] = INT_MAX;

    std::size_t pos = 0; // Position in the permutation array
    for (std::size_t i = 0; i < n; ++i)
    {
      assert(i <= pos);

      if (permut[i] >= 0)
        continue;

      int branch_length = 0;
      for (int k = static_cast<int>(i); permut[k] < 0; k = parent[k])
        branch_length++;

      for (int j = branch_length - 1, k = static_cast<int>(i); j >= 0; --j, k = parent[k])
        permut[k] = static_cast<int>(pos) + j;

      pos += branch_length;
    }
    assert(pos == n);
    permut[-1] = -1;

    int* tmp_par = new int[n];
    for (std::size_t i = 0; i < n; ++i)
      tmp_par[permut[i]] = permut[parent[i]];

    std::memcpy(parent, tmp_par, n * sizeof(int));
    delete[] tmp_par;
  }
} // namespace ano::morpho::details
