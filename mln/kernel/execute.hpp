#ifndef MLN_KERNEL_EXECUTE_HPP
# define MLN_KERNEL_EXECUTE_HPP

# include <mln/core/neighborhood/neighborhood.hpp>
# include <mln/core/image/image.hpp>
# include <mln/core/internal/intseq.hpp>
# include <mln/kernel/context.hpp>

# include <boost/fusion/include/as_list.hpp>

namespace mln
{

  namespace kernel
  {

    ///
    /// \brief Execute a kernel expression
    ///
    ///
    template <class Expr, class N, class... I>
    void
    execute(const Expr& expr, const Neighborhood<N>& nbh, I&&... images);



    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace impl
    {

      template <class ExprList, class Context, int... K>
      void
      exec_nbh(const ExprList& elist, Context& ctx, const intseq<K...>&)
      {
        // eval with the const version of the variable enables
        // gcc to perform a better SRA since ctx does not need to
        // live in memory anymore.
        namespace bf = boost::fusion;

        const Context ctx_ = ctx;
        dontcare( (bf::at_c<K>(ctx.m_accus)
                   .take(proto::eval(proto::child(bf::at_c<K>(elist)), ctx_)), true) ... );
      }

      template <class AccuList, int... K>
      void
      init_nbh(AccuList& accus, const intseq<K...>&)
      {
        dontcare( (boost::fusion::at_c<K>(accus).init(), true) ... );
      }

      template <class Context>
      struct retrieve_accu_result_type
      {
        template <class> struct result;

        template <class F, class Expr>
        struct result<F(Expr)>
        {
          typedef typename proto::result_of::eval<Expr, Context>::type type;
        };
      };

      template <class Context>
      struct meta_make_accu
      {
        template <class> struct result;

        template <class F, class Expr>
        struct result<F(Expr)>
        {
          typedef typename proto::tag_of<Expr>::type::feature feature;
          typedef typename proto::result_of::child<Expr>::type child;
          typedef typename proto::result_of::eval<typename std::remove_reference<child>::type, Context>::type V;
          typedef typename feature::template apply<typename std::decay<V>::type>::type  type;
        };

        template <class Expr>
        typename result< void(Expr)>::type
        operator() (const Expr&) const
        {
          return typename result<void(Expr)>::type ();
        }
      };


      template <class Expr, class Nbh, class... I>
      void
      execute(Expr& expr, const Nbh& nbh, I&&... images)
      {


        kernel::details::transform_aggregate<> t1;
        kernel::details::retrieve_aggregate  t2;

        auto newexpr = t1(expr);
        auto subexprs_ = t2(expr, boost::fusion::nil());
        auto subexprs = boost::fusion::as_list(subexprs_);

        // Defines some helperf for aggregates
        // SubexprList: The type of the fusion list holding subexprs
        // SubExprTypeList: The type of the mpl list holding the subexprs evaluation type
        typedef decltype(subexprs) SubexprList;
        typedef meta_kernel_context<I...> meta_ctx;
        typedef retrieve_accu_result_type<meta_ctx> Acc2Type;
        typedef typename boost::fusion::result_of::transform<const SubexprList, Acc2Type>::type SubExprTypeList_;
        typedef typename boost::fusion::result_of::as_list<SubExprTypeList_>::type SubExprTypeList;

        // accumulator list for each aggregate
        typedef typename boost::fusion::result_of::transform<const SubexprList, meta_make_accu<meta_ctx> >::type AccuList_;
        typedef typename boost::fusion::result_of::as_list<AccuList_>::type AccuList;
        AccuList_ accus_ = boost::fusion::transform(subexprs, meta_make_accu<meta_ctx> ());
        AccuList accus = boost::fusion::as_list(accus_);

        typename int_list_seq<boost::mpl::size<SubexprList>::type::value>::type iseq;

        typedef zip_image<I...> image_t;
        typedef mln_reference(image_t) V;

        auto z = imzip(images...);
        mln_pixter(px, z);
        mln_iter(nx, nbh(px));

        // Define context
        typedef kernel::kernel_context<V, SubExprTypeList, AccuList> Context;

        mln_forall(px)
        {
            init_nbh(accus, iseq);

            mln_forall(nx)
            {
              Context ctx (accus, px->val(), nx->val());
              exec_nbh(subexprs, ctx, iseq);
            }

            const Context ctx (accus, px->val(), px->val());
            proto::eval(newexpr, ctx);
          }
      }

    }


    template <class Expr, class N, class... I>
    void
    execute(const Expr& expr, const Neighborhood<N>& nbh, I&&... images)
    {
      impl::execute(expr, exact(nbh), std::forward<I>(images)...);
    }




  }

} // end of namespace mln::kernel

#endif //!MLN_KERNEL_EXECUTE_HPP
