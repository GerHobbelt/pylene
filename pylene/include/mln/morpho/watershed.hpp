#pragma once

#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/view/value_extended.hpp>
#include <mln/core/neighborhood/neighborhood.hpp>
#include <mln/core/trace.hpp>

#include <mln/core/utils/dontcare.hpp>
#include <mln/labeling/blobs.hpp>
#include <mln/labeling/local_extrema.hpp>
#include <mln/morpho/private/pqueue.hpp>

namespace mln::morpho
{
  template <class Label_t, class InputImage, class Neighborhood>
  image_ch_value_t<std::remove_reference_t<InputImage>, Label_t> //
  watershed(InputImage&& ima, Neighborhood&& nbh, int& nlabel);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {
    struct watershed_default_visitor
    {
      void init(mln::dontcare_t /*nlabel*/, mln::dontcare_t /*markers*/) {}
      void on_waterline(mln::dontcare_t /*lbl1*/, mln::dontcare_t /*lbl2*/, mln::dontcare_t /*cur_p*/,
                        mln::dontcare_t /*cur_v*/)
      {
      }
      bool on_label(mln::dontcare_t /*p*/, mln::dontcare_t /*val*/) { return true; /* Stop on second label */ }
      void finalize() {}
    };

    template <class I, class N, class O, class WV>
    int watershed(I input, N nbh, O output, WV& watershed_visitor, bool markers = false)
    {
      using Label_t = image_value_t<O>;
      using V       = image_value_t<I>;

      // 1. Labelize minima (note that output is initialized to -1)
      int nlabel = 0;
      if (!markers)
        nlabel = mln::labeling::impl::local_minima(input, nbh, output, std::less<V>());

      watershed_visitor.init(nlabel, output);

      constexpr int kUnlabeled = -2;
      constexpr int kInqueue   = -1;
      constexpr int kWaterline = 0;

      // 2. inset neighbors inqueue
      // Pixels in the border gets the status 0 (deja vu)
      // Pixels in the queue get -1
      // Pixels not in the queue get -2
      constexpr auto impl_type = mln::morpho::details::pqueue_impl::linked_list;
      mln::morpho::details::pqueue_fifo<I, impl_type, /* reversed = */ true> pqueue(input);
      {
        output.extension().fill(kWaterline);

        mln_foreach (auto px, output.pixels())
        {
          // Not a local minimum => early exit
          if (px.val() != 0)
            continue;

          bool is_local_min_neighbor = false;
          for (auto nx : nbh(px))
            if (nx.val() > 0) // p is neighbhor to a local minimum
            {
              is_local_min_neighbor = true;
              break;
            }
          if (is_local_min_neighbor)
          {
            px.val() = kInqueue;
            pqueue.push(input(px.point()), px.point());
          }
          else
          {
            px.val() = kUnlabeled;
          }
        }
      }

      // 3. flood from minima
      {
        while (!pqueue.empty())
        {
          auto [level, p] = pqueue.top();

          auto pxOut = output.pixel(p);
          mln_assertion(pxOut.val() == kInqueue);
          pqueue.pop();

          // Check if there is a single marked neighbor
          Label_t common_label               = kWaterline;
          bool    has_single_adjacent_marker = false;
          for (auto nxOut : nbh(pxOut))
          {
            const int nlbl = nxOut.val();
            if (nlbl <= 0)
              continue;
            else if (common_label == kWaterline)
            {
              common_label               = nlbl;
              has_single_adjacent_marker = true;
            }
            else if (nlbl != common_label)
            {
              has_single_adjacent_marker = false;
              if (watershed_visitor.on_waterline(nlbl, common_label, p, level))
                break;
            }
          }

          if (!has_single_adjacent_marker)
          {
            // If there are multiple labels => waterline
            pxOut.val() = kWaterline;
          }
          else
          {
            // If a single label, it gets labeled
            // Add neighbors in the queue
            pxOut.val() = common_label;
            watershed_visitor.on_label(pxOut.point(), common_label);
            for (auto q : nbh(p))
            {
              auto nlbl = output.at(q);
              if (nlbl == kUnlabeled)
              {
                pqueue.push(input(q), q);
                output(q) = kInqueue;
              }
            }
          }
        }
      }

      // 3. Label all unlabeled pixels
      {
        mln::for_each(output, [](auto& v) {
          if (v < 0)
            v = kWaterline;
        });
      }

      watershed_visitor.finalize();

      return nlabel;
    }
  } // namespace impl

  template <class Label_t, class InputImage, class Neighborhood>
  image_ch_value_t<std::remove_reference_t<InputImage>, Label_t> //
  watershed(InputImage&& ima, Neighborhood&& nbh, int& nlabel)
  {
    using I = std::remove_reference_t<InputImage>;
    using N = std::remove_reference_t<Neighborhood>;

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<N, mln::details::Neighborhood>());

    static_assert(std::is_integral<Label_t>::value, "The label type must integral.");
    static_assert(std::is_signed<Label_t>::value, "The label type must be signed.");

    mln_entering("mln::morpho::watershed");

    constexpr Label_t kUninitialized = -1;

    image_build_error_code err = IMAGE_BUILD_OK;

    auto output = imchvalue<Label_t>(ima) //
                      .adjust(nbh)
                      .set_init_value(kUninitialized)
                      .get_status(&err)
                      .build();

    impl::watershed_default_visitor viz{};

    if (err == IMAGE_BUILD_OK)
    {
      nlabel = impl::watershed(ima, nbh, output, viz);
    }
    else
    {
      mln::trace::warn("[Performance] The extension is not wide enough");
      auto out = view::value_extended(output, kUninitialized);
      nlabel   = impl::watershed(ima, nbh, out, viz);
    }
    return output;
  }

  template <class Label_t, class InputImage, class SeedImage, class Neighborhood>
  image_ch_value_t<std::remove_reference_t<InputImage>, Label_t> //
  watershed_from_markers(InputImage&& ima, Neighborhood&& nbh, SeedImage&& seeds, int& nlabel)
  {
    using I = std::remove_reference_t<InputImage>;
    using N = std::remove_reference_t<Neighborhood>;
    using S = std::remove_reference_t<SeedImage>;

    static_assert(mln::is_a<I, mln::details::Image>());
    static_assert(mln::is_a<N, mln::details::Neighborhood>());

    static_assert(std::is_integral<Label_t>::value, "The label type must integral.");
    static_assert(std::is_signed<Label_t>::value, "The label type must be signed.");
    static_assert(std::numeric_limits<Label_t>::max() >= std::numeric_limits<image_value_t<S>>::max());

    mln_entering("mln::morpho::watershed_from_markers");
    assert(ima.domain() == seeds.domain());

    impl::watershed_default_visitor viz{};

    auto output = mln::labeling::blobs<Label_t>(seeds, nbh, nlabel);
    if (output.border() < nbh.radial_extent())
      impl::watershed(ima, nbh, view::value_extended(output, 0), viz, true);
    else
      impl::watershed(ima, nbh, output, viz, true);
    return output;
  }
} // namespace mln::morpho
