
#define MLN_VEC_PRIVATE_VEC_PROMOTE_FUN(T, dim, tag, fun) internal::vec_base<decltype(fun(std::declval<T>())), dim, tag>

#define MLN_VEC_PRIVATE_PROMOTE(T1, T2) decltype(std::declval<T1>() + std::declval<T2>())

#define MLN_VEC_PRIVATE_PROMOTE_FUN(T, fun) decltype(fun(std::declval<T>()))

#define MLN_VEC_PRIVATE_PROMOTE_FUNCOMP(T, f, g) decltype(f(g(std::declval<T>())))

#define MLN_VEC_PRIVATE_GEN_UNARY_CODE(FUN)                                                                            \
  template <class T, unsigned dim, class tag>                                                                          \
  inline MLN_VEC_PRIVATE_VEC_PROMOTE_FUN(T, dim, tag, FUN) FUN(const internal::vec_base<T, dim, tag>& x)               \
  {                                                                                                                    \
    MLN_VEC_PRIVATE_VEC_PROMOTE_FUN(T, dim, tag, FUN) res;                                                             \
    for (unsigned i = 0; i < dim; ++i)                                                                                 \
      res[i] = FUN(x[i]);                                                                                              \
    return res;                                                                                                        \
  }

#define MLN_VEC_PRIVATE_GEN_BINARY_CODE(FUN, FUNBASE, EXPR)                                                            \
  template <class T, unsigned dim, class tag>                                                                          \
  inline auto FUN(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y)                  \
      ->decltype(FUNBASE(EXPR))                                                                                        \
  {                                                                                                                    \
    return FUNBASE(EXPR);                                                                                              \
  }
