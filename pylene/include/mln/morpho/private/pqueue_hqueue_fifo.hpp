#pragma once

#include <type_traits>


namespace mln
{
  namespace morpho
  {
    namespace details
    {

      template <class I>
      class pqueue_hqueue_fifo
      {
        using key_type   = mln_value(I);
        using value_type = mln_point(I);

      public:
        template <class J>
        pqueue_hqueue_fifo(const Image<J>& f);

        void                            push(const key_type& k, const value_type& v);
        void                            pop();
        std::pair<key_type, value_type> top() const;
        bool                            empty() const;

      private:
        static_assert(!std::is_signed<key_type>::value, "Key type must be unsigned.");

        static constexpr int nvalues = (1 << value_traits<key_type>::quant);
        struct node_t
        {
          int        size = 0;
          value_type head;
          value_type tail;
        };

        // Pointer to head and tail element in the queue
        std::array<node_t, nvalues> m_queues;

        // Next pointer image
        mln_ch_value(I, value_type) m_next;

        key_type m_current_level;
      };

      template <class I>
      template <class J>
      pqueue_hqueue_fifo<I>::pqueue_hqueue_fifo(const Image<J>& f)
        : m_next(mln::exact(f), mln::init())
        , m_current_level(nvalues - 1)
      {
      }

      template <class I>
      void pqueue_hqueue_fifo<I>::pop()
      {
        mln_precondition(m_queues[m_current_level].size > 0);

        auto& head = m_queues[m_current_level].head;
        head       = m_next(head);
        m_queues[m_current_level].size--;

        // Try to go down
        {
          int level = m_current_level;
          while (level > 0 && m_queues[level].size == 0)
            level--;
          if (m_queues[level].size > 0)
          {
            m_current_level = level;
            return;
          }
        }

        // Try to go up
        {
          int level = m_current_level;
          while (level < nvalues - 1 && m_queues[level].size == 0)
            level++;

          if (m_queues[level].size > 0)
          {
            m_current_level = level;
            return;
          }
        }
        m_current_level = nvalues - 1;
      }

      template <class I>
      void pqueue_hqueue_fifo<I>::push(const key_type& k, const value_type& v)
      {
        if (m_queues[k].size == 0)
        {
          m_queues[k].head = v;
          m_queues[k].tail = v;
          m_queues[k].size = 1;
        }
        else
        {
          m_next(m_queues[k].tail) = v;
          m_queues[k].tail         = v;
          m_queues[k].size++;
        }

        if (k < m_current_level)
          m_current_level = k;
      }

      template <class I>
      std::pair<mln_value(I), mln_point(I)> pqueue_hqueue_fifo<I>::top() const
      {
        mln_precondition(m_queues[m_current_level].size > 0);
        return std::make_pair(m_current_level, m_queues[m_current_level].head);
      }

      template <class I>
      bool pqueue_hqueue_fifo<I>::empty() const
      {
        return m_queues[m_current_level].size == 0;
      }


    } // namespace details
  }   // namespace morpho
} // namespace mln
