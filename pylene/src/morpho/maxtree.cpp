#include <cstddef>
#include <cstring>
#include <cassert>
#include <memory>

namespace mln::morpho::experimental::details
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
  void permute_parent(int* parent, int* permut, std::size_t n)
  {
    constexpr int npos = -1;
    std::fill(permut, permut + n, npos);


    std::size_t pos = 0; // Position in the permutation array
    for (std::size_t i = 0; i < n; ++i)
    {
      assert(i <= pos);

      if (permut[i] != npos)
        continue;

      int branch_length = 0;
      for (int k = static_cast<int>(i); permut[k] == npos; k = parent[k])
        branch_length++;

      for (int j = 1, k = static_cast<int>(i); j <= branch_length; ++j, k = parent[k])
        permut[k] = pos + branch_length - j;

      pos += branch_length;
    }
    assert(pos == n);

    int* tmp_par = new int[n];
    for (std::size_t i = 0; i < n; ++i)
      tmp_par[permut[i]] = permut[parent[i]];

    std::memcpy(parent, tmp_par, n * sizeof(int));
    delete[] tmp_par;
  }
}
