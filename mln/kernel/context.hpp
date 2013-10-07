#ifndef MLN_KERNEL_CONTEXT_HPP
# define MLN_KERNEL_CONTEXT_HPP

# include <tuple>
# include <mln/core/image/image.hpp>
# include <mln/core/neighborhood/neighborhood.hpp>
# include <mln/accu/accumulator.hpp>
# include <mln/core/internal/intseq.hpp>
# include <boost/proto/proto.hpp>
# include <mln/kernel/types.hpp>
# include <mln/kernel/aggregate.hpp>
# include <mln/kernel/intro.hpp>
# include <mln/kernel/details/transform.hpp>

namespace mln
{

  namespace kernel
  {
    namespace proto = boost::proto;


    ///
    ///
    /// 
    template <class FullExpr, class SubexprTypeList, class AccuList, class Neighborhood, class... I>
    struct kernel_context;


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace internal
    {
      struct nil {};

      template <class... T>
      bool first_non_nil(bool v, T...)
      {
        return v;
      }

      template <class... T>
      bool first_non_nil(nil, T... x)
      {
        return first_non_nil(x...);
      }

    };

    template <class... I>
    struct meta_kernel_context
    {
      typedef std::tuple<I...> image_tuple_t;

      template<class Expr,
	       class Tag = typename proto::tag_of<Expr>::type,
	       class Args0 = typename Expr::proto_child0 >
      struct eval
	: proto::default_eval<Expr, meta_kernel_context, Tag>
      {};

      template <class Expr, int k>
      struct eval<Expr, proto::tag::terminal, ima_expr_tag<k, kernel::point>  >
      {
	typedef typename std::tuple_element<k, image_tuple_t>::type image_t;
	typedef mln_reference(image_t) result_type;
      };

      template <class Expr, int k>
      struct eval<Expr, proto::tag::terminal, ima_expr_tag<k, kernel::neighbor>  >
      {
	typedef typename std::tuple_element<k, image_tuple_t>::type image_t;
	typedef mln_reference(image_t) result_type;
      };

      template <class Expr, class Tag, class Arg0>
      struct eval<Expr, kernel::tag::aggregate<Tag>, Arg0>
      {
        typedef typename proto::result_of::eval<Arg0, meta_kernel_context>::type V;
        typedef typename accu::result_of<Tag, typename std::decay<V>::type>::type result_type;
      };

    };



    template <class FullExpr,
              class SubexprTypeList,
              class AccuList,
              class Nbh,
              class...>
    struct kernel_context_;

    template <class FullExpr,
              class SubexprTypeList,
              class AccuList,
              class Nbh,
              int... K, // 0...NumberOfImage - 1
              class... I>
    struct kernel_context_<FullExpr, SubexprTypeList, AccuList, Nbh, intseq<K...>, I...>
    {
      struct fake_iter
      {
        template <class... T>
        fake_iter(T...) {}

        void init() const {}
        void next() const {}

        constexpr
        internal::nil
        finished() const { return internal::nil(); }
      };


      kernel_context_(const AccuList& accus, const Nbh& nbh, I&&... images)
        : m_accus (accus),
          m_nbh(nbh),
          m_ima(std::forward<I>(images)...),
          m_px(rng::iter(m_ima.pixels())),
          m_spx(unzip_proxy_pixel_iterator<zip_iterator_t, K>(m_px)...),
          m_nx(rng::iter(m_nbh(std::get<K>(m_spx)))...)
      {
      }


      void p_init()
      {
        m_px.init();
      }

      void p_next()
      {
        m_px.next();
      }

      bool p_finished() const
      {
        return m_px.finished();
      }

      void n_init()
      {
        dontcare((std::get<K>(m_nx).init(), 1)... );
      }

      void n_next()
      {
        dontcare((std::get<K>(m_nx).next(), 1)... );
      }

      bool n_finished() const
      {
        return internal::first_non_nil(std::get<K>(m_nx).finished()...);
      }

      // Context operation.
      typedef std::tuple<I...> image_tuple_t;


      template<class Expr,
	       class Tag = typename proto::tag_of<Expr>::type,
	       class Args0 = typename Expr::proto_child0 >
      struct eval
	: proto::default_eval<Expr, kernel_context_, Tag>
      {};


      // Specialization for f(p)
      template <class Expr, int k>
      struct eval<Expr, proto::tag::terminal, ima_expr_tag<k, kernel::point>  >
      {
	typedef typename std::tuple_element<k, image_tuple_t>::type image_t;
	typedef mln_reference(image_t) result_type;

	result_type
	operator() (Expr&, const kernel_context_& ctx) const
	{
	  return std::get<k>(ctx.m_spx)->val();
	}
      };

      // Specialization for f(n)
      template <class Expr, int k>
      struct eval<Expr, proto::tag::terminal, ima_expr_tag<k, kernel::neighbor>  >
      {
	typedef typename std::tuple_element<k, image_tuple_t>::type image_t;
	typedef mln_reference(image_t) result_type;

	result_type
	operator() (Expr&, const kernel_context_& ctx) const
	{
	  return std::get<k>(ctx.m_nx)->val();
	}
      };

      // Specialization for terminal A(...) pre-computed
      template <class Expr, int k>
      struct eval<Expr, proto::tag::terminal, kernel::details::aggregate_pl<k>  >
      {
        typedef typename boost::mpl::at_c<SubexprTypeList, k>::type result_type;


	result_type
	operator() (Expr&, const kernel_context_& ctx) const
	{
	  return boost::fusion::at_c<k>(ctx.m_accus).to_result();
	}
      };

      // Specialization for A(...) on computation
      // This returns nothing, it only pre-computed an aggragation subexpr
      template <class Expr, class Tag, class Arg0>
      struct eval<Expr, kernel::tag::aggregate<Tag>, Arg0>
      {
        typedef typename proto::result_of::eval<Arg0, kernel_context_> result_type;

	result_type
	operator() (Expr& subexpr, const kernel_context_& ctx) const
	{
	  return proto::eval(proto::child(subexpr), ctx);
	}
      };



      // Specialization for Aggregation e.g Sum(f(n))
      // template <class Expr, class Acc, class Args0>
      // struct eval<Expr, kernel::tag::aggregate<Acc>, Args0>
      // {
      //   typedef typename std::decay<typename proto::result_of::eval<Args0, kernel_context_>::type>::type T;
      //   typedef typename accu::result_of<Acc, T>::type result_type;

      //   result_type
      //   operator() (Expr& expr, kernel_context_& ctx) const
      //   {
      //     auto acc = accu::make_accumulator<Acc, T>( Acc() );
      //     acc.init();

      //     auto m_nx = ctx.m_nx; // Cache the neighbors iterators


      //     while (! internal::first_non_nil(std::get<K>(m_nx).finished()...))
      //       {
      //         acc.take( proto::eval(proto::child(expr), ctx) );
      //         dontcare( (std::get<K>(m_nx).next(), true)... );
      //       }
      //     return acc.to_result();
      //   }

      // };

    public:
      AccuList m_accus;

    private:
      typedef expression_traits<FullExpr> ETraits;
      typedef std::tuple<typename ETraits::template image<K>::with_n ...> N_used;


      typedef zip_image<I...>                                            image_t;
      typedef mln_pixiterator(image_t)                                   zip_iterator_t;
      typedef std::tuple<unzip_proxy_pixel_iterator<zip_iterator_t, K>...>     px_tuple_t;
      typedef std::tuple<
        typename std::conditional<
          std::tuple_element<K, N_used>::type::value,
          typename range_iterator<
            typename std::result_of<Nbh(unzip_proxy_pixel_iterator<zip_iterator_t, K>&)>::type
            >::type,
          fake_iter>::type
        ...
        >                                                                nx_tuple_t;

      Nbh                   m_nbh;
      image_t               m_ima;
      zip_iterator_t        m_px;
      px_tuple_t            m_spx;
      nx_tuple_t            m_nx;

      // // aggregates aux data.
      // typedef typename std::result_of< kernel::details::retrieve_aggregate(FullExpr) >::type reduction_list_t;
      // typedef std::tuple<
      //   typename std::result_of< accu::make_accumulator
      //     (typename boost::mpl::at_c<reduction_list_t, K0>::type::feature,
      //      typename std::decay<
      //        typename proto::result_of::eval<
      //          typename proto::child<typename boost::mpl::at_c<reduction_list_t, K0>::type>::type,
      //          kernel_context_>::type
      //      >::type
      //      ) >::type ... > accu_tuple_t;

      // accu_tuple_t m_accumulators;
    };

    template <class FullExpr, class SubexprTypeList, class AccuList, class Nbh, class... I>
    struct kernel_context
      : kernel_context_<FullExpr,
                        SubexprTypeList,
                        AccuList,
                        Nbh,
                        typename int_list_seq<sizeof...(I)>::type,
                        I...>
    {
      typedef kernel_context_<FullExpr,
                              SubexprTypeList,
                              AccuList,
                              Nbh,
                              typename int_list_seq<sizeof...(I)>::type,
                              I...> base;

      kernel_context(const AccuList& accus, const Nbh& nbh, I&&... images)
        : base(accus, nbh, std::forward<I>(images)...)
      {
      }
    };



  } // end of namespace mln::kernel

} // end of namespace mln

#endif //!MLN_KERNEL_CONTEXT_HPP
