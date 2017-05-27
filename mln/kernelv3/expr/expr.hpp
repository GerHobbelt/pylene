#ifndef MLN_KERNELV3_EXPR_EXPR_HPP
# define MLN_KERNELV3_EXPR_EXPR_HPP

# include <type_traits>
# include <boost/hana/tuple.hpp>

namespace mln
{
  namespace kernel
  {
    template <class E>
    struct Expr
    {
      template <class Visitor>
      constexpr decltype(auto) accept(Visitor& v) const
      {
        return v.visit(this->exact());
      }

      template <class Visitor>
      constexpr decltype(auto) accept(const Visitor& v) const
      {
        return v.visit(this->exact());
      }

      auto operator= (const Expr& rhs);

      template <class E2>
      auto operator= (const Expr<E2>& rhs);


      E& exact() { return static_cast<E&>(*this); }
      const E& exact() const { return static_cast<const E&>(*this); }
    };

   template <class E>
   using is_an_expr = std::is_base_of<Expr<E>,E>;

   enum class tag
   {
     /* Terminal nodes */
     image_p,
     terminal,
     /* Recursive nodes */
     function,
     assignment,
   };

   template <tag Tag, class E>
   struct expr : Expr<E> {};

   template <tag Tag, class E, class... ArgTypes>
   struct nary_expr : expr<Tag, E>
   {
     constexpr static std::size_t nargs = sizeof...(ArgTypes);

     nary_expr(const boost::hana::tuple<ArgTypes...>& args_) : args(args_) {}

     boost::hana::tuple<ArgTypes...> args;
   };

   template <tag Tag, class Lhs, class Rhs>
   struct binary_expression : nary_expr<Tag, binary_expression<Tag, Lhs, Rhs>, Lhs, Rhs>
   {
     using Base = nary_expr<Tag, binary_expression<Tag, Lhs, Rhs>, Lhs, Rhs>;

     binary_expression(const Lhs& lhs, const Rhs& rhs)
       : Base(boost::hana::make_tuple(lhs, rhs))
     {
     }

     auto arg1() const { return boost::hana::at_c<0>(Base::args); }
     auto arg2() const { return boost::hana::at_c<1>(Base::args); }
     auto arg1() { return boost::hana::at_c<0>(Base::args); }
     auto arg2() { return boost::hana::at_c<1>(Base::args); }
   };

   template <tag Tag, class Lhs, class Rhs>
   binary_expression<Tag, Lhs, Rhs>
   make_binary_expr(const Lhs& lhs, const Rhs& rhs)
   {
     return {lhs, rhs};
   }

   template <class LhsType, class RhsType>
   using assignment_expr = binary_expression<tag::assignment, LhsType, RhsType>;

   template <class E1>
   template <class E2>
   auto
   Expr<E1>::operator= (const Expr<E2>& rhs)
   {
     return assignment_expr<E1, E2>{this->exact(), rhs.exact()};
   }

   template <class E>
   auto
   Expr<E>::operator= (const Expr<E>& rhs)
   {
     return assignment_expr<E, E>{this->exact(), rhs.exact()};
   }

  } // end of namespace mln::kernel
} // end of namespace mln

#endif //!MLN_KERNELV3_EXPR_EXPR_HPP
