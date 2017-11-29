#ifndef MLN_MORPHO_TOS_PRIVATE_PSET_REDBLACKTREE_HPP
# define MLN_MORPHO_TOS_PRIVATE_PSET_REDBLACKTREE_HPP

# include <mln/core/image/image.hpp>
# include <boost/intrusive/set.hpp>

namespace mln
{
  namespace morpho
  {
    namespace tos
    {
      namespace impl
      {
        template <class Key, class Point>
        struct pset_node;


        /// \brief A point set allows to keep a ordered set of points or indexes
        /// sorted by a given key (generally the pixel value)
        /// This implementation uses red-black tree impl (std::map)
        template <class I>
        class pset_redblacktree
        {
        public:
          using Key = mln_value(I);
          using Point = typename I::size_type;

          template <class J>
          pset_redblacktree(const Image<J>& ima);

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
          using node_t = pset_node<Key, Point>;
          using key_getter_t = typename node_t::keyer;

          mln_ch_value(I, node_t)       m_allocated_nodes;
          boost::intrusive::multiset<node_t,
                                     boost::intrusive::constant_time_size<false>,
                                     boost::intrusive::key_of_value<key_getter_t>
                                     > m_set;
        };

        /******************************************/
        /****          Implementation          ****/
        /******************************************/

        template <class Key, class Point>
        struct pset_node : boost::intrusive::set_base_hook<boost::intrusive::optimize_size<true> >
        {
          Key   level;
          Point p;

          friend bool operator< (const pset_node& a, const pset_node& b)
          {
            return a.value < b.value;
          }

          struct keyer
          {
            using type = Key;
            Key operator() (const pset_node& node) { return node.level; }
          };
        };

        template <class I>
        template <class J>
        pset_redblacktree<I>::pset_redblacktree(const Image<J>& ima)
          : m_allocated_nodes(imchvalue<node_t>(ima))
        {
        }

        template <class I>
        bool pset_redblacktree<I>::empty() const
        {
          return m_set.empty();
        }

        template <class I>
        void pset_redblacktree<I>::insert(Key level, Point p)
        {
          node_t& node = m_allocated_nodes[p];
          node.level = level;
          node.p = p;
          m_set.insert(node);
        }


        template <class I>
        std::pair<bool, typename I::size_type>
        pset_redblacktree<I>::try_pop(Key level)
        {
          auto it = m_set.find(level);
          if (it == m_set.end())
            return std::make_pair(false, Point());

          auto& node = *it;
          m_set.remove_node(node);
          return std::make_pair(true, node.p);
        }

        template <class I>
        std::pair<mln_value(I), typename I::size_type>
        pset_redblacktree<I>::pop(Key level)
        {
          assert(!m_set.empty());

          auto it = m_set.lower_bound(level); // try to go up
          if (it == m_set.end())
            --it; // if cannot try to go down

          node_t& node = *it;
          m_set.remove_node(node);
          return std::make_pair(node.level, node.p);
        }

      } // end of namespace mln::morpho::tos::impl
    }   // end of namespace mln::morpho::tos
  }     // end of namespace mln::morpho
} // end of namespace mln

#endif //! MLN_MORPHO_TOS_PRIVATE_PSET_REDBLACKTREE_HPP
