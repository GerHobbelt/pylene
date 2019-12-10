
#define MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(FUN)                                                                     \
  namespace functional                                                                                                 \
  {                                                                                                                    \
    using mln::FUN;                                                                                                    \
    template <typename T>                                                                                              \
    struct FUN##_t                                                                                                     \
    {                                                                                                                  \
      typedef decltype(FUN(std::declval<T>())) result_type;                                                            \
                                                                                                                       \
      auto operator()(const T& x) const -> decltype(FUN(x)) { return FUN(x); }                                         \
    };                                                                                                                 \
    template <>                                                                                                        \
    struct FUN##_t<void>                                                                                               \
    {                                                                                                                  \
      template <typename T>                                                                                            \
      auto operator()(const T& x) const -> decltype(FUN(x))                                                            \
      {                                                                                                                \
        return FUN(x);                                                                                                 \
      }                                                                                                                \
    };                                                                                                                 \
  }

#define MLN_PRIVATE_FUNCTIONAL_GEN_BINARY_CODE(FUN)                                                                    \
  namespace functional                                                                                                 \
  {                                                                                                                    \
    using mln::FUN;                                                                                                    \
    template <typename T1, typename T2>                                                                                \
    struct FUN##_t                                                                                                     \
    {                                                                                                                  \
      typedef decltype(FUN(std::declval<T1>(), std::declval<T2>())) result_type;                                       \
      auto operator()(const T1& x, const T2& y) const -> decltype(FUN(x, y)) { return FUN(x, y); }                     \
    };                                                                                                                 \
    template <>                                                                                                        \
    struct FUN##_t<void, void>                                                                                         \
    {                                                                                                                  \
      template <typename T1, typename T2>                                                                              \
      auto operator()(const T1& x, const T2& y) const                                       \
      {                                                                                                                \
        return FUN(x, y);                                                                                              \
      }                                                                                                                \
    };                                                                                                                 \
  }
