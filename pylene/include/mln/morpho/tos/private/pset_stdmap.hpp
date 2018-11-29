#ifndef MLN_MORPHO_TOS_PRIVATE_PSET_SET_HPP
#define MLN_MORPHO_TOS_PRIVATE_PSET_SET_HPP

#include <map>
#include <mln/core/image/image.hpp>

namespace mln
{
  namespace morpho
  {
    namespace ToS
    {
      namespace impl
      {

        /// \brief A point set allows to keep a ordered set of points or indexes
        /// sorted by a given key (generally the pixel value)
        /// This implementation uses red-black tree impl (std::map)
        template <class I>
        class pset_set
        {
        public:
          using Key   = mln_value(I);
          using Point = typename I::size_type;

          template <class J>
          pset_set(const Image<J>& ima);

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
          std::multimap<Key, Point> m_set;
        };

        /******************************************/
        /****          Implementation          ****/
        /******************************************/

        template <class I>
        template <class J>
        pset_set<I>::pset_set(const Image<J>&)
        {
        }

        template <class I>
        bool pset_set<I>::empty() const
        {
          return m_set.empty();
        }

        template <class I>
        void pset_set<I>::insert(Key level, Point p)
        {
          m_set.insert(std::make_pair(level, p));
        }


        template <class I>
        std::pair<bool, typename I::size_type> pset_set<I>::try_pop(Key level)
        {
          auto it = m_set.find(level);
          if (it == m_set.end())
            return std::make_pair(false, Point());

          Point p = it->second;
          m_set.erase(it);
          return std::make_pair(true, p);
        }

        template <class I>
        std::pair<mln_value(I), typename I::size_type> pset_set<I>::pop(Key level)
        {
          assert(!m_set.empty());

          auto it = m_set.lower_bound(level); // try to go up
          if (it == m_set.end())
            --it; // if cannot try to go down

          auto res = *it;
          m_set.erase(it);
          return res;
        }

      } // end of namespace mln::morpho::tos::impl
    }   // end of namespace mln::morpho::tos
  }     // end of namespace mln::morpho
} // end of namespace mln

#endif //! MLN_MORPHO_TOS_PRIVATE_PSET_SET_HPP
