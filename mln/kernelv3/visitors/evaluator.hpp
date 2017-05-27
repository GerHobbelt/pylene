#ifndef MLN_KERNEL_VISITORS_EVALUATOR_HPP
# define MLN_KERNEL_VISITORS_EVALUATOR_HPP

namespace mln
{
  namespace kernel
  {
    namespace visitors
    {

      class EvaluatorBase
      {
      public:
        template <class E>
        constexpr auto visit(const expr<tag::terminal, E>& e) const
        {
          return e.exact().value;
        }
      };


      template <class ValueTuple>
      class Evaluator : public EvaluatorBase
      {
      public:
        Evaluator(ValueTuple v) : m_value(std::move(v)) {}

        using EvaluatorBase::visit;

        template <class I, int k>
        constexpr decltype(auto) visit(const image_expr_p<I, k>&) const
        {
          return std::get<k>(m_value);
        }


        template <class F, class... ExprArgs, std::size_t... Ints>
        constexpr decltype(auto) _apply(F&& f, boost::hana::tuple<ExprArgs...> args,
                                        std::index_sequence<Ints...>) const
        {
          return std::forward<F>(f)(boost::hana::at_c<Ints>(args)...);
        }


        template <class F, class... Args>
        constexpr decltype(auto) visit(const function_expr<F, Args...>& e) const
        {
          auto args = boost::hana::transform(e.args, [&](auto exp) {
              return exp.accept(*this);
            });
          return this->_apply(e.f, args, std::make_index_sequence<sizeof...(Args)>() );
        }

        template <class Lhs, class Rhs>
        constexpr decltype(auto) visit(const binary_expression<tag::assignment, Lhs, Rhs>& e) const
        {
          return e.arg1().accept(*this) = e.arg2().accept(*this);
        }

      private:
        ValueTuple m_value;
      };

    } // end of namespace mln::kernel::visitors
  } // end of namespace mln::kernel
} // end of namespace mln

#endif //!MLN_KERNEL_VISITORS_EVALUATOR_HPP
