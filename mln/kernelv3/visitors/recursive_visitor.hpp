#ifndef MLN_KERNELV3_VISITORS_RECURSIVE_VISITOR_HPP
# define MLN_KERNELV3_VISITORS_RECURSIVE_VISITOR_HPP

# include <mln/core/concept/object.hpp>
# include <mln/kernelv3/expr/expr.hpp>
# include <mln/core/internal/tuple_utility.hpp>
# include <mln/kernelv3/expr/ref.hpp>

namespace mln
{
  namespace kernel
  {
    namespace visitors
    {
      template <class Viz>
      class RecursiveTransformVisitor : public Object<Viz>
      {
      public:
        template <class E>
        auto visit(const expr<tag::image_p, E>& e)
        {
          return e.exact();
        }

        template <class E>
        auto visit(const expr<tag::reference, E>& e)
        {
          return /*ref(*/ e.exact().value->accept(exact(*this));
        }

        template <class F, class...ArgTypes>
        auto visit(const function_expr<F, ArgTypes...>& e)
        {
          auto recursecall = [&](const auto& x) {
            return x.accept(exact(*this));
          };

          auto args = mln::internal::tuple_transform(e.args, recursecall);
          return make_function_expr(e.f, args);
        }

        template <class E>
        auto visit(const expr<tag::assignment, E>& e)
        {
          const auto& exp = e.exact();
          return make_binary_expr<tag::assignment>(exp.arg1().accept(exact(*this)),
                                                   exp.arg2().accept(exact(*this)));
        }


        template <class E>
        auto visit(const expr<tag::terminal, E>& e)
        {
          return e.exact();
        }

      };

    } // end of namespace mln::kernel::visitors

  } // end of namespace mln::kernel

} // end of namespace mln

#endif //!MLN_KERNELV3_VISITORS_RECURSIVE_VISITOR_HPP
