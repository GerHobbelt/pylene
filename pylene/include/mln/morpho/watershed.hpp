#pragma once

#include <mln/core/extension/extension.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/neighborhood/neighborhood.hpp>
#include <mln/core/trace.hpp>
#include <mln/labeling/local_extrema.hpp>
#include <mln/morpho/private/pqueue.hpp>


namespace mln
{
  namespace morpho
  {

    template <class Label_t, class I, class N>
    mln_ch_value(I, Label_t) watershed(const Image<I>& ima, const Neighborhood<N>& nbh, int& nlabel);

    namespace experimental
    {
      template <class Label_t, class InputImage, class N>
      image_ch_value_t<InputImage, Label_t> watershed(InputImage ima, const Neighborhood<N>& nbh, int& nlabel);
    }


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace details
    {


      template <class I, class N, class O>
      int watershed(const I& input, const N& nbh, O& output)
      {
        using Label_t = mln_value(O);
        using V       = mln_value(I);


        // 1. Labelize minima (note that output in initialized to -1)
        const int nlabel = mln::labeling::details::local_minima(input, nbh, output, std::less<Label_t>());


        constexpr int kUnlabeled = -2;
        constexpr int kInqueue   = -1;
        constexpr int kWaterline = 0;

        // 2. inset neighbors inqueue
        // Pixels in the border gets the status 0 (deja vu)
        // Pixels in the queue get -1
        // Pixels not in the queue get -2
        pqueue_fifo<I> pqueue(input);
        {
          mln::extension::fill(output, kWaterline);

          mln_pixter(px, output);
          mln_iter(nx, nbh(px));
          mln_forall (px)
          {
            if (px->val() == 0)
            {
              bool is_local_min_neighbor = false;
              mln_forall (nx)
                if (nx->val() > 0) // p is neighbhor to a local minimum
                {
                  is_local_min_neighbor = true;
                  break;
                }
              if (is_local_min_neighbor)
              {
                px->val() = kInqueue;
                pqueue.push(input(px->point()), px->point());
              }
              else
              {
                px->val() = kUnlabeled;
              }
            }
          }
        }


        // 3. flood from minima
        {
          mln_cpixel(I) pxIn(&input);
          mln_pixel(O) pxOut(&output);
          mln_iter(nxIn, nbh(pxIn));
          mln_iter(nxOut, nbh(pxOut));

          while (!pqueue.empty())
          {
            mln_point(I) p;
            V level;
            std::tie(level, p) = pqueue.top();

            pxOut = output.pixel(p);
            mln_assertion(pxOut.val() == kInqueue);
            pqueue.pop();

            // Check if there is a single marked neighbor
            Label_t common_label               = kWaterline;
            bool    has_single_adjacent_marker = false;
            mln_forall (nxOut)
            {
              int nlbl = nxOut->val();
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
              pxIn        = input.pixel(p);
              mln_forall (nxIn, nxOut)
              {
                auto nlbl = nxOut->val();
                if (nlbl == kUnlabeled)
                {
                  pqueue.push(nxIn->val(), nxIn->point());
                  nxOut->val() = kInqueue;
                }
              }
            }
          }
        }

        // 3. Label all unlabeled pixels
        {
          mln_foreach (auto px, output.pixels())
            if (px.val() < 0)
              px.val() = kWaterline;
        }

        return nlabel;
      }
    } // namespace details


    template <class Label_t, class I, class N>
    mln_ch_value(I, Label_t) watershed(const Image<I>& ima_, const Neighborhood<N>& nbh_, int& nlabel)
    {
      static_assert(std::is_integral<Label_t>::value, "The label type must integral.");
      static_assert(std::is_signed<Label_t>::value, "The label type must be signed.");

      mln_entering("mln::morpho::watershed");

      const I& ima = exact(ima_);
      const N& nbh = exact(nbh_);

      constexpr Label_t kUninitialized = -1;
      mln_ch_value(I, Label_t) output  = imchvalue<Label_t>(ima).adjust(nbh).init(kUninitialized);
      if (extension::need_adjust(output, nbh))
      {
        auto out = extension::add_value_extension(output, kUninitialized);
        nlabel   = details::watershed(ima, nbh, out);
      }
      else
      {
        mln::extension::fill(output, kUninitialized);
        nlabel = details::watershed(ima, nbh, output);
      }
      return output;
    }

    namespace experimental
    {

      template <class Label_t, class InputImage, class N>
      image_ch_value_t<InputImage, Label_t> watershed(InputImage ima, const Neighborhood<N>& nbh_, int& nlabel)
      {
        static_assert(is_a<InputImage, Image>());
        static_assert(std::is_integral<Label_t>::value, "The label type must integral.");
        static_assert(std::is_signed<Label_t>::value, "The label type must be signed.");

        mln_entering("mln::morpho::watershed");

        const N& nbh = exact(nbh_);

        constexpr Label_t kUninitialized         = -1;
        mln_ch_value(InputImage, Label_t) output = imchvalue<Label_t>(ima).adjust(nbh).init(kUninitialized);
        if (extension::need_adjust(output, nbh))
        {
          auto out = extension::add_value_extension(output, kUninitialized);
          nlabel   = details::watershed(ima, nbh, out);
        }
        else
        {
          mln::extension::fill(output, kUninitialized);
          nlabel = details::watershed(ima, nbh, output);
        }
        return output;
      }

    } // namespace experimental


  } // namespace morpho
} // namespace mln
