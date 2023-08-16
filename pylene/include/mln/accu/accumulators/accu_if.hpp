#pragma once

#include <mln/accu/accumulator_base.hpp>

namespace mln
{

  namespace accu
  {

    namespace accumulators
    {

      template <class Accu, class Predicate, class ArgType = typename Accu::argument_type>
      struct accu_if : accumulator_base<accu_if<Accu, Predicate, ArgType>, ArgType, typename Accu::result_type,
                                        typename Accu::feature>
      {
        using argument_type = ArgType;
        using result_type   = typename Accu::result_type;
        using feature       = typename Accu::feature;

        accu_if(const Accu& accu = Accu(), const Predicate& pred = Predicate())
          : m_accu(accu)
          , m_pred(pred)
        {
        }

        void init() { m_accu.init(); }

        void take(const argument_type& arg)
        {
          if (m_pred(arg))
            m_accu.take(arg);
        }

        void take(const accu_if& other) { m_accu.take(other.m_accu); }

        template <class Feat>
        friend result_type extract(const accu_if& accu, Feat feat)
        {
          return extract(accu.m_accu, feat);
        }

      private:
        Accu      m_accu;
        Predicate m_pred;
      };
    } // namespace accumulators
  }   // namespace accu
} // namespace mln
