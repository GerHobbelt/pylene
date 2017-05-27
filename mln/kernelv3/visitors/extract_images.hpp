#ifndef MLN_KERNELV3_VISITORS_EXTRACT_IMAGES_HPP
# define MLN_KERNELV3_VISITORS_EXTRACT_IMAGES_HPP

# include <mln/kernelv3/expr/expr.hpp>
# include <boost/hana/not_equal.hpp>
# include <boost/hana/find_if.hpp>
# include <boost/hana/transform.hpp>
# include <boost/hana/maximum.hpp>
# include <boost/hana/integral_constant.hpp>

namespace mln
{
  namespace kernel
  {
    namespace visitors
    {

      /// \brief A vistor to extract a images for an expr
      /// and return this a tuple
      template <class E>
      auto extract_images(const Expr<E>& expression);

      /******************************************/
      /****          Implementation          ****/
      /******************************************/


      namespace internal
      {

        /// \brief Extract the k(nth image
        template <int k>
        class ImageExtractor
        {
        public:
          template <class I>
          constexpr auto visit(const image_expr_p<I, k>& e) const
          {
            return boost::hana::just(&e.value);
          }

          template <class I, int k2>
          constexpr auto visit(const image_expr_p<I, k2>&) const
          {
            return boost::hana::nothing;
          }

          template <class E>
          constexpr auto visit(const expr<tag::terminal, E>&) const
          {
            return boost::hana::nothing;
          }

          template <tag Tag, class E, class... Args>
          constexpr auto visit(const nary_expr<Tag, E, Args...>& e) const
          {
            auto search_seq = boost::hana::transform(e.exact().args, [&](auto exp) {
                return exp.accept(*this);
              });
            auto result = boost::hana::find_if(search_seq, boost::hana::is_just);
            return result.value_or(boost::hana::nothing);
          }
        };


        class ImageCounter
        {
        public:
          template <class E>
          constexpr auto visit(const Expr<E>&) const
          {
            return boost::hana::int_c<0>;
          }

          template <class I, int k>
          constexpr auto visit(const image_expr_p<I, k>&) const
          {
            return boost::hana::int_c<k+1>;
          }

          template <tag Tag, class E, class... Args>
          constexpr auto visit(const nary_expr<Tag, E, Args...>& e) const
          {
            auto search_seq = boost::hana::transform(e.exact().args, [&](auto exp) {
                return exp.accept(*this);
              });
            return boost::hana::maximum(search_seq);
          }
        };

        template <class E, std::size_t... k>
        auto extract_images(const Expr<E>& e, std::index_sequence<k...>)
        {
          return std::make_tuple(e.accept(ImageExtractor<k>()).value()...);
        }
      }

      template <class E>
      auto extract_images(const Expr<E>& expression)
      {
        auto n_images = expression.accept(internal::ImageCounter());
        return internal::extract_images(expression, std::make_index_sequence<n_images>());
      }


    } // end of namespace mln::kernel::visitors
  } // end of namespace mln::kernel
} // end of namespace mln

#endif //!MLN_KERNELV3_VISITORS_EXTRACT_IMAGES_HPP
