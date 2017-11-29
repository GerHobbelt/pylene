#ifndef MLN_MORPHO_TOS_V2_TOS_HPP
# define MLN_MORPHO_TOS_V2_TOS_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/image/morphers/casted_image.hpp>
# include <mln/core/neighb2d.hpp>
# include <mln/morpho/datastruct/component_tree.hpp>
# include <mln/morpho/maxtree/maxtree.hpp>
# include <mln/morpho/tos/private/immersion.hpp>
# include <mln/morpho/tos/private/propagation.hpp>
# include <mln/core/algorithm/fill.hpp>

namespace mln
{
  namespace morpho
  {
    namespace __2
    {

      enum {
        tos_ensure_component_root_on_2face = 0x01,
      };

      template <typename I>
      morpho::component_tree<typename I::size_type, mln_ch_value(I, unsigned)>
      tos(const Image<I>& ima, mln_point(I) start_point,
          int processing_flags = 0);

      template <typename I>
      morpho::component_tree<typename I::size_type, mln_ch_value(I, unsigned)>
      tos(const Image<I>& ima);



      /******************************************/
      /****          Implementation          ****/
      /******************************************/

      namespace impl
      {
        template <class P, class J>
        void ensure_root_on_2f(component_tree<P, J>& tree)
        {
          mln_entering("mln::morpho::impl::__2::tos - postprocessing");
          auto& pmap = tree._get_data()->m_pmap;
          auto& nodes = tree._get_data()->m_nodes;
          auto& S = tree._get_data()->m_S;

          mln_ch_value(J, bool) is2F = imchvalue<bool>(pmap).init(false);
          auto dom = is2F.domain();
          fill(is2F | sbox2d{dom.pmin, dom.pmax, {2,2}}, true);

          for (unsigned p = 0; p < S.size(); ++p)
            {
              if (is2F[S[p]]) {
                unsigned q = nodes[pmap[S[p]]].m_point_index;
                if (not is2F[S[q]]) {
                  std::swap(S[p], S[q]);
                }
              }
            }
        }
      }

      template <typename I>
      morpho::component_tree<typename I::size_type, mln_ch_value(I, unsigned)>
      tos(const Image<I>& ima, mln_point(I) pmin, int processing_flags)
      {
        static_assert(std::is_same<mln_point(I), point2d>::value, "Must be a 2D image");
        using P = typename I::size_type;

        mln_entering("mln::morpho::tos");

        // 1. Compute the ord image.
        int max_depth;
        auto g = morpho::tos::impl::immersion(ima);
        auto ord = morpho::tos::impl::propagation(g, pmin * 2, max_depth, nullptr);

        // 2. Build the tree
        component_tree<P, mln_ch_value(I, unsigned)> tree;
        {
          if (max_depth < value_traits<uint16>::max())
          {
            tree = morpho::maxtree_indexes(imcast<uint16>(ord), c4);
          }
          else
          {
            mln::trace::warn("Max depth > 16bits, slow version");
            tree = morpho::maxtree_indexes(ord, c4);
          }
        }

        // 3. Ensure that the first point in node pset is a 2F
        if (processing_flags & tos_ensure_component_root_on_2face)
          impl::ensure_root_on_2f(tree);

        return tree;
      }

      template <typename I>
      morpho::component_tree<typename I::size_type, mln_ch_value(I, unsigned)>
      tos(const Image<I>& ima)
      {
        return tos(ima, exact(ima).domain().pmin);
      }

    } // end of namespace mln::morpho::__2
  } // end of namespace mln::morpho
} // end of namespace mln

#endif //!MLN_MORPHO_TOS_V2_TOS_HPP
