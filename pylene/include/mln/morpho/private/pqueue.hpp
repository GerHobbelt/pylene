#ifndef MLN_MORPHO_PRIVATE_PQUEUE_HPP
#define MLN_MORPHO_PRIVATE_PQUEUE_HPP

#include <mln/morpho/private/pqueue_hqueue_fifo.hpp>

namespace mln
{
  namespace morpho
  {
    namespace details
    {

      /// Provides a priority queue of points with a the fifo property
      template <class I>
      class pqueue_fifo
      {
        using key_type   = mln_value(I);
        using value_type = mln_point(I);

      public:
        template <class J>
        pqueue_fifo(const Image<J>& f);

        void                            push(const key_type& priority, const value_type& element);
        void                            pop();
        std::pair<key_type, value_type> top() const;
        bool                            empty() const;

      private:
        static_assert(value_traits<key_type>::quant <= 16, "Only low quantized type supported.");

        pqueue_hqueue_fifo<mln_concrete(I)> m_delegate;
      };

      /******************************************/
      /****          Implementation          ****/
      /******************************************/

      template <class I>
      template <class J>
      pqueue_fifo<I>::pqueue_fifo(const Image<J>& f)
        : m_delegate(f)
      {
      }

      template <class I>
      void pqueue_fifo<I>::push(const key_type& k, const value_type& v)
      {
        m_delegate.push(k, v);
      }

      template <class I>
      void pqueue_fifo<I>::pop()
      {
        m_delegate.pop();
      }

      template <class I>
      bool pqueue_fifo<I>::empty() const
      {
        return m_delegate.empty();
      }


      template <class I>
      std::pair<mln_value(I), mln_point(I)> pqueue_fifo<I>::top() const
      {
        return m_delegate.top();
      }
    }
  } // end of namespace mln::morpho
} // end of namespace mln

#endif //! MLN_MORPHO_PRIVATE_PQUEUE_HPP
