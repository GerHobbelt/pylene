#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/value_extended.hpp>
#include <mln/core/neighborhood/neighborhood.hpp>
#include <mln/core/trace.hpp>

#include <mln/morpho/experimental/canvas/unionfind.hpp>

#include <type_traits>
#include <functional>


namespace mln::labeling::experimental
{

  /// Compute and labelize the local mimima of an image
  ///
  /// \param[in] input Input image
  /// \param[in] nbh neighborhood
  /// \param[out] nlabel Number of minima
  /// \param[in] cmp (optional) Strict weak ordering comparison function for values
  template <class Label_t, class I, class N, class Compare = std::less<>>
  [[gnu::noinline]] //
  image_ch_value_t<std::remove_reference_t<I>, Label_t>
  local_minima(I&& input, N&& nbh, int& nlabel, Compare cmp = Compare{});


  /// Compute and labelize the local maxima of an image
  ///
  /// \param[in] input Input image
  /// \param[in] nbh neighborhood
  /// \param[out] nlabel Number of maxima
  template <class Label_t, class I, class N>
  [[gnu::noinline]] //
  image_ch_value_t<std::remove_reference_t<I>, Label_t>
  local_maxima(I&& input, N&& nbh, int& nlabel);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {

    // Output must initialized to Label_t(-1)
    template <class I, class O, class N, class Compare>
    [[gnu::noinline]] int local_minima(I input, N nbh, O output, Compare cmp)
    {
      using Label_t = image_value_t<O>;
      using point_t = image_point_t<I>;
      using mln::morpho::experimental::canvas::impl::zfindroot;

      constexpr Label_t kUnseen = static_cast<Label_t>(-1);

      image_ch_value_t<I, point_t> parent = imchvalue<point_t>(input);

      {
        // Forward scan
        mln_foreach_new(auto p, input.domain())
        {
          auto fp = input(p);

          // Make set
          auto root = p;

          bool is_possible_minimum = true;
          for (auto q : nbh.before(p))
          {
            if (output.at(q) == kUnseen)
              continue;

            auto fq  = input(q);
            // TODO: use <=> in cpp20
            if (cmp(fq, fp)) // input(n) < input(p)
            {
              is_possible_minimum = false;
            }
            else if (cmp(fp, fq)) // input(p) < input(n) -> n is not a local minimum
            {
              const auto r = zfindroot(parent, q);
              output(r)    = false;
            }
            else // Flat zone linking
            {
              const auto q_root = zfindroot(parent, q);
              if (q_root != root)
              {
                is_possible_minimum  = is_possible_minimum && output(q_root);
                auto roots           = std::minmax(root, q_root);
                parent(roots.second) = roots.first;
                root                 = roots.first;
              }
            }
          }
          parent(p)    = root;
          output(p)    = is_possible_minimum;
          output(root) = is_possible_minimum;
        }
      }

      int n_label = 0;
      // Labelisation
      {
        mln_foreach_new (auto pxOut, output.new_pixels())
        {
          if (pxOut.val())
          {
            auto p = pxOut.point();
            if (p == parent(p)) // New minimum
              pxOut.val() = ++n_label;
            else
            {
              auto r      = zfindroot(parent, p);
              pxOut.val() = output(r);
            }
          }
        }
      }
      mln_assertion((n_label <= std::numeric_limits<Label_t>::max()) &&
                    "Overflow detected (the number of label has exceed the label type capacity");
      return n_label;
    }
  } // namespace impl

  template <class Label_t, class I_, class N, class Compare>
  image_ch_value_t<std::remove_reference_t<I_>, Label_t> local_minima(I_&& input, N&& nbh, int& nlabel, Compare cmp)
  {
    mln_entering("mln::labeling::local_minima");
    static_assert(std::is_integral<Label_t>::value && !std::is_same<Label_t, bool>::value,
                  "Label type must a non-bool integral type");

    constexpr Label_t      kUnseen = static_cast<Label_t>(-1);
    image_build_error_code err     = IMAGE_BUILD_OK;

    auto out = imchvalue<Label_t>(input) //
                   .adjust(nbh)
                   .set_init_value(kUnseen)
                   .get_status(&err)
                   .build();

    if (err == IMAGE_BUILD_OK) // Extension is wide enough
    {
      nlabel = impl::local_minima(input, nbh, out, cmp);
    }
    else
    {
      mln::trace::warn("[Performance] The extension is not wide enough");
      nlabel = impl::local_minima(input, nbh, view::value_extended(out, kUnseen), cmp);
    }
    return out;
  }

  template <class Label_t, class I, class N>
  image_ch_value_t<std::remove_reference_t<I>, Label_t> local_maxima(I&& input, N&& nbh, int& nlabel)
  {
    return local_minima<Label_t>(input, nbh, nlabel, std::greater<>());
  }


} // namespace mln::labeling::experimental
