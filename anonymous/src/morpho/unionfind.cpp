
namespace ano::morpho::canvas::impl
{


  [[gnu::noinline]] int zfindroot(int* par, int x) noexcept
  {
    // find root
    int r;
    {
      int p = x, q = par[x];
      while (p != q)
      {
        p = q;
        q = par[p];
      }
      r = p;
    }
    // path compression
    for (auto p = x; p != r;)
    {
      auto q = par[p];
      par[p] = r;
      p      = q;
    }
    return r;
  }

} // namespace ano::morpho::canvas::impl
