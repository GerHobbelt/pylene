#pragma once

#include <ano/core/algorithm/for_each.hpp>
#include <ano/core/extension/fill.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/image/view/value_extended.hpp>
#include <ano/core/neighborhood/neighborhood.hpp>
#include <ano/core/trace.hpp>

#include <ano/core/utils/dontcare.hpp>
#include <ano/labeling/blobs.hpp>
#include <ano/labeling/local_extrema.hpp>
#include <ano/morpho/private/pqueue.hpp>

namespace ano::morpho
{
  template <class Label_t, class InputImage, class Neighborhood>
  image_ch_value_t<std::remove_reference_t<InputImage>, Label_t> //
  watershed(InputImage&& ima, Neighborhood&& nbh, int& nlabel, bool waterline = true);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {
    template <class I, class N, class O>
    int watershed(I input, N nbh, O output, bool markers = false)
    {
      using Label_t = image_value_t<O>;
      using V       = image_value_t<I>;

      // 1. Labelize minima (note that output is initialized to -1)
      int nlabel = 0;
      if (!markers)
        nlabel = ano::labeling::impl::local_minima(input, nbh, output, std::less<V>());

      constexpr int kUnlabeled = -2;
      constexpr int kInqueue   = -1;
      constexpr int kWaterline = 0;

      // 2. inset neighbors inqueue
      // Pixels in the border gets the status 0 (deja vu)
      // Pixels in the queue get -1
      // Pixels not in the queue get -2
      constexpr auto impl_type = ano::morpho::details::pqueue_impl::linked_list;
      ano::morpho::details::pqueue_fifo<I, impl_type, /* reversed = */ true> pqueue(input);
      {
        output.extension().fill(kWaterline);

        ano_foreach (auto px, output.pixels())
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
          ano_assertion(pxOut.val() == kInqueue);
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
        ano::for_each(output, [](auto& v) {
          if (v < 0)
            v = kWaterline;
        });
      }

      return nlabel;
    }

    template <class I, class N, class O>
    int watershed_partition(I input, N nbh, O output, bool markers = false)
    {
      using V = image_value_t<I>;

      int nlabel = 0;
      if (!markers)
        nlabel = ano::labeling::impl::local_minima(input, nbh, output, std::less<V>());

      constexpr auto impl_type = ano::morpho::details::pqueue_impl::linked_list;
      ano::morpho::details::pqueue_fifo<I, impl_type, /* reversed = */ true> pqueue(input);
      {
        output.extension().fill(-1);
        ano_foreach (auto pix, output.pixels())
        {
          if (pix.val() <= 0)
            continue;
          for (auto p : nbh(pix))
          {
            if (p.val() == 0)
            {
              pqueue.push(input(pix.point()), pix.point());
              break;
            }
          }
        }

        while (!pqueue.empty())
        {
          auto [lvl, p] = pqueue.top();
          pqueue.pop();
          for (auto q : nbh(p))
          {
            if (output.at(q) == 0) // If extension then -1
            {
              output(q) = output(p);
              pqueue.push(input(q), q);
            }
          }
        }
      }
      return nlabel;
    }
  } // namespace impl

  template <class Label_t, class InputImage, class Neighborhood>
  image_ch_value_t<std::remove_reference_t<InputImage>, Label_t> //
  watershed(InputImage&& ima, Neighborhood&& nbh, int& nlabel, bool waterline)
  {
    using I = std::remove_reference_t<InputImage>;
    using N = std::remove_reference_t<Neighborhood>;

    static_assert(ano::is_a<I, ano::details::Image>());
    static_assert(ano::is_a<N, ano::details::Neighborhood>());

    static_assert(std::is_integral<Label_t>::value, "The label type must integral.");
    static_assert(std::is_signed<Label_t>::value, "The label type must be signed.");

    ano_entering("ano::morpho::watershed");

    constexpr Label_t kUninitialized = -1;

    image_build_error_code err = IMAGE_BUILD_OK;

    auto output = imchvalue<Label_t>(ima) //
                      .adjust(nbh)
                      .set_init_value(kUninitialized)
                      .get_status(&err)
                      .build();

    if (err == IMAGE_BUILD_OK)
    {
      if (waterline)
        nlabel = impl::watershed(ima, nbh, output);
      else
        nlabel = impl::watershed_partition(ima, nbh, output);
    }
    else
    {
      ano::trace::warn("[Performance] The extension is not wide enough");
      auto out = view::value_extended(output, kUninitialized);
      if (waterline)
        nlabel = impl::watershed(ima, nbh, out);
      else
        nlabel = impl::watershed_partition(ima, nbh, out);
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

    static_assert(ano::is_a<I, ano::details::Image>());
    static_assert(ano::is_a<N, ano::details::Neighborhood>());

    static_assert(std::is_integral<Label_t>::value, "The label type must integral.");
    static_assert(std::is_signed<Label_t>::value, "The label type must be signed.");
    static_assert(std::numeric_limits<Label_t>::max() >= std::numeric_limits<image_value_t<S>>::max());

    ano_entering("ano::morpho::watershed_from_markers");
    assert(ima.domain() == seeds.domain());

    auto output = ano::labeling::blobs<Label_t>(seeds, nbh, nlabel);
    if (output.border() < nbh.radial_extent())
      impl::watershed(ima, nbh, view::value_extended(output, 0), true);
    else
      impl::watershed(ima, nbh, output, true);
    return output;
  }
} // namespace ano::morpho
