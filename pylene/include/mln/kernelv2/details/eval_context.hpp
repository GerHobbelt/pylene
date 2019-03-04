#pragma once

#include <boost/proto/proto.hpp>

#include <tuple>

namespace mln
{
  namespace kernel
  {
    namespace details
    {
      namespace proto = boost::proto;

      template <class PVTuple, class NVTuple>
      struct eval_context
      {
        template <class Expr, class Tag = typename proto::tag_of<Expr>::type>
        struct eval : proto::default_eval<Expr, const eval_context, Tag>
        {
        };

        template <class Expr, int k>
        struct eval<Expr, tag::image_call_p<k>>
        {
          typedef typename proto::result_of::value<Expr>::type      type_;
          typedef typename std::remove_reference<type_>::type::type I;
          typedef mln_reference(I) result_type;

          result_type operator()(Expr, const eval_context& ctx) const { return std::get<k>(ctx.pvals); }
        };

        template <class Expr, int k>
        struct eval<Expr, tag::image_call_n<k>>
        {
          typedef typename proto::result_of::value<Expr>::type      type_;
          typedef typename std::remove_reference<type_>::type::type I;
          typedef mln_reference(I) result_type;

          result_type operator()(Expr, const eval_context& ctx) const { return std::get<k>(ctx.nvals); }
        };

        template <class Expr>
        struct eval<Expr, tag::aggregate>
        {
          typedef typename proto::result_of::left<Expr>::type       accu_expr;
          typedef typename proto::result_of::value<accu_expr>::type accu;
          typedef typename accu::result_type                        result_type;

          result_type operator()(Expr x, eval_context) const { return proto::value(proto::left(x)).to_result(); }
        };

        PVTuple pvals;
        NVTuple nvals;
      };

    } // namespace details

  } // namespace kernel

} // namespace mln
