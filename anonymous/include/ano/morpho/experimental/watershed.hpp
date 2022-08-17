#pragma once

#include <ano/core/algorithm/for_each.hpp>
#include <ano/core/extension/fill.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/image/view/value_extended.hpp>
#include <ano/core/neighborhood/neighborhood.hpp>
#include <ano/core/trace.hpp>

#include <ano/labeling/experimental/local_extrema.hpp>
#include <ano/morpho/experimental/private/pqueue.hpp>

namespace ano::morpho::experimental
{

  template <class Label_t, class InputImage, class Neighborhood>
  image_ch_value_t<std::remove_reference_t<InputImage>, Label_t> //
  watershed(InputImage&& ima, Neighborhood&& nbh, int& nlabel);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {


    template <class I, class N, class O>
    int watershed(I input, N nbh, O output)
    {
      using Label_t = image_value_t<O>;
      using V       = image_value_t<I>;

      // 1. Labelize minima (note that output is initialized to -1)
      const int nlabel = ano::labeling::experimental::impl::local_minima(input, nbh, output, std::less<V>());

      constexpr int kUnlabeled = -2;
      constexpr int kInqueue   = -1;
      constexpr int kWaterline = 0;

      // 2. inset neighbors inqueue
      // Pixels in the border gets the status 0 (deja vu)
      // Pixels in the queue get -1
      // Pixels not in the queue get -2
      constexpr auto impl_type = ano::morpho::experimental::details::pqueue_impl::linked_list;
      ano::morpho::experimental::details::pqueue_fifo<I, impl_type, /* reversed = */ true> pqueue(input);
      {
        output.extension().fill(kWaterline);

        ano_foreach_new (auto px, output.new_pixels())
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

          auto pxOut = output.new_pixel(p);
          ano_assertion(pxOut.val() == kInqueue);
          pqueue.pop();

          // Check if there is a single marked neighbor
          Label_t common_label               = kWaterline;
          bool    has_single_adjacent_marker = false;
          for (auto nxOut : nbh(pxOut))
          {
            int nlbl = nxOut.val();
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
  } // namespace impl

  template <class Label_t, class InputImage, class Neighborhood>
  image_ch_value_t<std::remove_reference_t<InputImage>, Label_t> //
  watershed(InputImage&& ima, Neighborhood&& nbh, int& nlabel)
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
      nlabel = impl::watershed(ima, nbh, output);
    }
    else
    {
      ano::trace::warn("[Performance] The extension is not wide enough");
      auto out = view::value_extended(output, kUninitialized);
      nlabel   = impl::watershed(ima, nbh, out);
    }
    return output;
  }

} // namespace ano::morpho::experimental
