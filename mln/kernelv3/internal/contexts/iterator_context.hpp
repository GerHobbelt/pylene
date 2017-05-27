#ifndef MLN_KERNEL_INTERNAL_CONTEXTS_ITERATOR_CONTEXT_HPP
# define MLN_KERNEL_INTERNAL_CONTEXTS_ITERATOR_CONTEXT_HPP

# include <mln/core/internal/tuple_utility.hpp>
# include <mln/kernelv3/visitors/extract_images.hpp>

namespace mln
{
  namespace kernel
  {
    namespace internal
    {
      namespace contexts
      {
        template <class ZipIterator>
        struct iterator_context
        {
          ZipIterator& iter() { return m_iter; }
          ZipIterator m_iter;
        };


        template <class E>
        auto make_iterator_context(const Expr<E>& e)
        {
          auto images = visitors::extract_images(e);
          auto iterators = mln::internal::tuple_transform(images, [](auto* im_ptr) {
              return rng::iter(im_ptr->values());
            });
          zip_iterator<decltype(iterators)> zip_it(iterators);
          return iterator_context<decltype(zip_it)> {zip_it};
        }

      } // end of namespace mln::kernel::internal::contexts
    } // end of namespace mln::kernel::internal
  } // end of namespace mln::kernel
} // end of namespace mln

#endif //!MLN_KERNEL_INTERNAL_CONTEXTS_ITERATOR_CONTEXT_HPP
