#ifndef MLN_MORPHO_TOS_PRIVATE_PSET_HQUEUE_HPP
# define MLN_MORPHO_TOS_PRIVATE_PSET_HQUEUE_HPP

# include <mln/core/image/image.hpp>
# include <vector>

namespace mln
{
  namespace morpho
  {
    namespace tos
    {
      namespace impl
      {

        /// \brief A point set allows to keep a ordered set of points or indexes
        /// sorted by a given key (generally the pixel value)
        /// This implementation uses hierarchical queues
        template <class I>
        class pset_hqueue
        {
        public:
          using Key = mln_value(I);
          using Point = typename I::size_type;
          static_assert(std::is_unsigned<Key>::value, "Key must an unsigned type.");

          template <class J>
          pset_hqueue(const Image<J>& ima);

          /// \brief Insert a point \p p at the given \p level
          void insert(Key level, Point p);

          /// \brief Return the closest level pair (key,point) such that:
          /// * there is a pair (l₁,p₁) in the set with l₁ ≥ level then (l₁,p₁)
          /// * there is a pair (l₁,p₂) in the set with l₂ < level then (l₂,p₂)
          std::pair<Key, Point> pop(Key level);
          std::pair<bool, Point> try_pop(Key level);

          /// \brief Return true if the set is empty
          bool empty() const;

        private:
          static constexpr Point npos = value_traits<Point>::max();
          static constexpr int nlevel = (1 << value_traits<Key>::quant);
          static_assert(nlevel < value_traits<int>::max(), "Incompatible type.");

          Point pop_at_level(int level);

          std::vector<Point>            m_head; ///< Vector of queue heads
          mln_ch_value(I, Point)        m_next;
          unsigned                      m_size = 0;
        };

        /******************************************/
        /****          Implementation          ****/
        /******************************************/

        template <class I>
        template <class J>
        pset_hqueue<I>::pset_hqueue(const Image<J>& ima)
          : m_head(nlevel, (Point)npos),
            m_next(exact(ima), mln::init())
        {
        }

        template <class I>
        bool pset_hqueue<I>::empty() const
        {
          return m_size == 0;
        }

        template <class I>
        void pset_hqueue<I>::insert(Key level, Point p)
        {
          m_next[p] = m_head[level];
          m_head[level] = p;
          m_size++;
        }

        template <class I>
        typename I::size_type
        pset_hqueue<I>::pop_at_level(int level)
        {
          Point p = m_head[level];
          m_head[level] = m_next[p];
          m_size--;
          return p;
        }

        template <class I>
        std::pair<bool, typename I::size_type>
        pset_hqueue<I>::try_pop(Key level)
        {
          if (m_head[level] != npos)
            return std::make_pair(true, pop_at_level(level));
          else
            return std::make_pair(false, Point());
        }

        template <class I>
        std::pair<mln_value(I), typename I::size_type>
        pset_hqueue<I>::pop(Key level)
        {
          mln_precondition(m_size > 0);

          // Try to go up
          {
            int l = level;
            while (l < nlevel)
            {
              if (m_head[l] != npos)
                return std::make_pair(static_cast<Key>(l), pop_at_level(l));
              l++;
            }
          }

          // Try to go down
          {
            int l = level - 1;
            while (l >= 0)
            {
              if (m_head[l] != npos)
                return std::make_pair(static_cast<Key>(l), pop_at_level(l));
              l--;
            }
          }
          mln_assertion(false && "This should not have reached this location.");
          return std::make_pair(Key(), (Point)npos);
        }

      } // end of namespace mln::morpho::tos::impl
    }   // end of namespace mln::morpho::tos
  }     // end of namespace mln::morpho
} // end of namespace mln

#endif //! MLN_MORPHO_TOS_PRIVATE_PSET_HQUEUE_HPP
