#ifndef MLN_MORPHO_TOS_PRIVATE_PSET_HPP
#define MLN_MORPHO_TOS_PRIVATE_PSET_HPP

#include "pset_hqueue.hpp"
#include "pset_redblacktree.hpp"
#include <mln/core/image/image.hpp>

namespace mln
{
  namespace morpho
  {
    namespace ToS
    {
      namespace impl
      {
        enum class pset_impl
        {
          kAuto,
          kHqueues,
          kRedBlackTree,
        };

        /// \brief A point set allows to keep a ordered set of points or indexes
        /// sorted by a given key (generally the pixel value)
        /// For now, it only supports indexes
        ///
        /// \warning For two entries with identical keys (levels), this data structure acts
        /// like a LIFO (last in first out)
        template <class I, pset_impl kImplementation = pset_impl::kAuto>
        class pset
        {
        public:
          using Key   = mln_value(I);
          using Point = typename I::size_type;

          template <class J>
          pset(const Image<J>& ima);

          /// \brief Insert a point \p p at the given \p level
          void insert(Key level, Point p);

          /// \brief Return and remove the closest level pair (key,point) such that:
          /// * there is a pair (l₁,p₁) in the set with l₁ ≥ level then (l₁,p₁)
          /// * there is a pair (l₁,p₂) in the set with l₂ < level then (l₂,p₂)
          std::pair<Key, Point> pop(Key level);

          /// \brief Return and remove the level pair (key,point) if it exists such a pair
          std::pair<bool, Point> try_pop(Key level);

          /// \brief Return true if the set is empty
          bool empty() const;

        private:
          static constexpr pset_impl kSelectedImplementation =
              (kImplementation != pset_impl::kAuto)
                  ? kImplementation
                  : ((mln::value_traits<Key>::quant <= 16) ? pset_impl::kHqueues : pset_impl::kRedBlackTree);

          using ConcreteImageType   = mln_concrete(I);
          using hqueue_impl_t       = impl::pset_hqueue<ConcreteImageType>;
          using redblacktree_impl_t = impl::pset_redblacktree<ConcreteImageType>;
          using delegate_t =
              std::conditional_t<kSelectedImplementation == pset_impl::kHqueues, hqueue_impl_t, redblacktree_impl_t>;
          delegate_t m_delegate;
        };

        /******************************************/
        /****          Implementation          ****/
        /******************************************/

        template <class I, pset_impl kImplementation>
        template <class J>
        pset<I, kImplementation>::pset(const Image<J>& ima)
          : m_delegate(ima)
        {
        }

        template <class I, pset_impl kImplementation>
        void pset<I, kImplementation>::insert(Key level, Point p)
        {
          m_delegate.insert(level, p);
        }

        template <class I, pset_impl kImplementation>
        std::pair<mln_value(I), typename I::size_type> pset<I, kImplementation>::pop(Key level)
        {
          return m_delegate.pop(level);
        }

        template <class I, pset_impl kImplementation>
        std::pair<bool, typename I::size_type> pset<I, kImplementation>::try_pop(Key level)
        {
          return m_delegate.try_pop(level);
        }


        template <class I, pset_impl kImplementation>
        bool pset<I, kImplementation>::empty() const
        {
          return m_delegate.empty();
        }

      } // end of namespace mln::morpho::tos::impl
    }   // end of namespace mln::morpho::tos
  }     // end of namespace mln::morpho
} // end of namespace mln

#endif //! MLN_MORPHO_TOS_PRIVATE_PSET_HPP
