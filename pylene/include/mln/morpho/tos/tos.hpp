#pragma once

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/morphers/casted_image.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/neighb3d.hpp>
#include <mln/morpho/maxtree/maxtree.hpp>
#include <mln/morpho/tos/private/immersion.hpp>
#include <mln/morpho/tos/private/propagation.hpp>


// FIXME:
namespace to_migrate
{
  namespace impl
  {
    template <typename I, typename V>
    void fill(I& ima, const V& v)
    {
      mln_viter(pin, ima);
      mln_forall (pin)
        *pin = v;
    }
  } // namespace impl

  template <typename OutputImage, typename Value>
  [[deprecated]] OutputImage& __fill(mln::Image<OutputImage>& output_, const Value& val) {
    OutputImage& output = mln::exact(output_);
    impl::fill(output, val);
    return output;
  }

  template <typename OutputImage, typename Value>
  [[deprecated]] OutputImage&& __fill(mln::Image<OutputImage>&& output_, const Value& val)
  {
    __fill(output_, val);
    return mln::move_exact(output_);
  }
} // namespace to_migrate

namespace mln
{
  namespace morpho
  {

    enum
    {
      tos_ensure_component_root_on_2face = 0x01,
    };

    /// \brief Compute the tree of shapes.
    /// \param[in] input Input image
    /// \param[in] start_point Root point
    /// \param[in] processing_flags Extra options to control tree construction.
    /// \return A component tree encoding the tree of shapes.
    template <typename I>
    morpho::component_tree<typename I::size_type, mln_ch_value(I, unsigned)>
        tos(const Image<I>& input, mln_point(I) start_point, int processing_flags = 0);

    template <typename I>
    morpho::component_tree<typename I::size_type, mln_ch_value(I, unsigned)> tos(const Image<I>& ima);

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace impl
    {
      template <class P, class J>
      void ensure_root_on_2f(component_tree<P, J>& tree)
      {
        mln_entering("mln::morpho::impl::__2::tos - postprocessing");
        auto& pmap  = tree._get_data()->m_pmap;
        auto& nodes = tree._get_data()->m_nodes;
        auto& S     = tree._get_data()->m_S;

        mln_ch_value(J, bool) is2F = imchvalue<bool>(pmap).init(false);
        auto dom                   = is2F.domain();
        mln_point(J) step          = 2; // {2,2} or {2,2,2}
        // FIXME
        ::to_migrate::__fill(is2F | make_strided_box(dom.pmin, dom.pmax, step), true);

        for (unsigned p = 0; p < S.size(); ++p)
        {
          if (is2F[S[p]])
          {
            unsigned q = nodes[pmap[S[p]]].m_point_index;
            if (not is2F[S[q]])
            {
              std::swap(S[p], S[q]);
            }
          }
        }
      }
    } // namespace impl

    template <typename I>
    morpho::component_tree<typename I::size_type, mln_ch_value(I, unsigned)> tos(const Image<I>& ima, mln_point(I) pmin,
                                                                                 int             processing_flags)
    {
      static_assert(std::is_same<mln_point(I), point2d>::value or std::is_same<mln_point(I), point3d>::value,
                    "Input must be a 2D or 3D image");
      using P = typename I::size_type;

      mln_entering("mln::morpho::tos");

      // 1. Compute the ord image.
      int  max_depth;
      auto g   = morpho::ToS::impl::immersion(ima);
      auto ord = morpho::ToS::impl::propagation(g, pmin * 2, max_depth, nullptr);

      // 2. Build the tree
      using nbh_t = std::conditional_t<std::is_same<mln_point(I), mln::point2d>::value, c4_t, c6_t>;
      component_tree<P, mln_ch_value(I, unsigned)> tree;
      {
        if (max_depth < value_traits<uint16>::max())
        {
          tree = morpho::maxtree_indexes(mln::imcast<uint16>(ord), nbh_t());
        }
        else
        {
          mln::trace::warn("Max depth > 16bits, slow version");
          tree = morpho::maxtree_indexes(ord, nbh_t());
        }
      }

      // 3. Ensure that the first point in node pset is a 2F
      if (processing_flags & tos_ensure_component_root_on_2face)
        impl::ensure_root_on_2f(tree);

      return tree;
    }

    template <typename I>
    morpho::component_tree<typename I::size_type, mln_ch_value(I, unsigned)> tos(const Image<I>& ima)
    {
      return tos(ima, exact(ima).domain().pmin);
    }

  } // namespace morpho
} // namespace mln
