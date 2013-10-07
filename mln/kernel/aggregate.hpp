#ifndef MLN_KERNEL_AGGREGATE_HPP
# define MLN_KERNEL_AGGREGATE_HPP

# include <boost/proto/proto.hpp>

namespace mln
{

  namespace kernel
  {

    namespace tag
    {

      /// \brief The node tag for aggregation nodes.
      template <typename AccTag>
      struct aggregate {
        typedef AccTag feature;
      };

    }

    template <class AccTag, class Expr>
    using aggregate_expr = boost::proto::expr<kernel::tag::aggregate<AccTag>,
                                              boost::proto::list1<Expr> >;


    template <class AccTag>
    struct Aggregate
    {

      template <typename Expr>
      aggregate_expr<AccTag, Expr>
      operator() (Expr&& expr) const
      {
        return aggregate_expr<AccTag, Expr>::make(std::forward<Expr>(expr));
      }

    };


  } // end of namespace mln::kernel

} // end of namespace mln

#endif //!MLN_KERNEL_AGGREGATE_HPP
