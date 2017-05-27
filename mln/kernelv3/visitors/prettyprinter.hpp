#ifndef MLN_KERNELV3_VISITORS_PRETTYPRINTER_HPP
# define MLN_KERNELV3_VISITORS_PRETTYPRINTER_HPP

# include <mln/kernelv3/expr/expr.hpp>
# include <mln/core/internal/tuple_utility.hpp>
# include <map>

namespace mln
{
  namespace kernel
  {
    namespace visitors
    {

      class PrettyPrinter
      {
      public:
        template <class E>
        void visit(const expr<tag::image_p, E>& e)
        {
          const void* ptr = static_cast<const void*>(&e);
          auto idPtr = m_img2id.find(ptr);

          char id;
          if (idPtr == m_img2id.end())
            id = m_img2id[ptr] = m_next_image_id++;
          else
            id = idPtr->second;
          std::cout << id << "(p)";
        }

        template <class E>
        void visit(const expr<tag::function, E>& e)
        {
          auto printargs = [&](const auto& x) {
            x.accept(*this);
            std::cout << ",";
          };

          const auto& fcall = e.exact();
          std::cout << "Invoke(" << typeid(fcall.f).name() << ",";
          mln::internal::tuple_for_each(fcall.args, printargs);
          std::cout << ")";
        }

        template <class Arg1, class Arg2>
        void visit(const function_expr<std::plus<>, Arg1, Arg2>& e)
        {
          const auto& exp = e.exact();
          std::cout << "(";
          boost::hana::at_c<0>(exp.args).accept(*this);
          std::cout << "+";
          boost::hana::at_c<1>(exp.args).accept(*this);
          std::cout << ")";
        }


        template <class E>
        void visit(const expr<tag::assignment, E>& e)
        {
          const auto& exp = e.exact();
          exp.arg1().accept(*this);
          std::cout << " = ";
          exp.arg2().accept(*this);
        }


        template <class E>
        void visit(const expr<tag::terminal, E>& e)
        {
          std::cout << e.exact().value;
        }

      private:
        char m_next_image_id = 'A';
        std::map<const void*, char> m_img2id;
      };

    } // end of namespace mln::kernel::visitors
  } // end of namespace mln::kernel
} // end of namespace mln

#endif //!MLN_KERNELV3_VISITORS_PRETTYPRINTER_HPP
