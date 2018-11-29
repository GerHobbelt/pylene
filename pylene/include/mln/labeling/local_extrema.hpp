#ifndef MLN_LABELING_LOCAL_MINIMA_HPP
#define MLN_LABELING_LOCAL_MINIMA_HPP

#include <mln/core/extension/extension.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/neighborhood/neighborhood.hpp>
#include <mln/core/trace.hpp>

namespace mln
{
  namespace labeling
  {

    /// Compute and labelize the local mimima of an image
    ///
    /// \param[in] input Input image
    /// \param[in] nbh neighborhood
    /// \param[out] nlabel Number of minima
    /// \param[in] cmp (optional) Strict weak ordering comparison function for values
    template <class Label_t, class I, class N, class Compare = std::less<mln_value(I)>>
    mln_ch_value(I, Label_t)
        local_minima(const Image<I>& input, const Neighborhood<N>& nbh, int& nlabel, Compare cmp = Compare());

    /// Compute and labelize the local maxima of an image
    ///
    /// \param[in] input Input image
    /// \param[in] nbh neighborhood
    /// \param[out] nlabel Number of maxima
    template <class Label_t, class I, class N>
    mln_ch_value(I, Label_t) local_maxima(const Image<I>& input, const Neighborhood<N>& nbh, int& nlabel);


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace details
    {
      template <class Parent, class Point>
      Point zfindroot(Parent& parent, Point& p)
      {
        auto& par = parent(p);
        if (par != p)
          p = zfindroot(parent, par);
        return p;
      }


      // Output must initialized to Label_t(-1)
      template <class I, class O, class N, class Compare>
      int local_minima(const I& input, const N& nbh, O& output, Compare cmp)
      {
        using Label_t                        = mln_value(O);
        constexpr Label_t kUnseen            = static_cast<Label_t>(-1);
        mln_ch_value(I, mln_point(I)) parent = imchvalue<mln_point(I)>(input);

        mln_pixter(pxIn, input);
        mln_pixter(pxOut, output);
        mln_pixter(pxPar, parent);

        mln_iter(nxIn, nbh(pxIn));
        mln_iter(nxOut, nbh(pxOut));
        mln_iter(nxPar, nbh(pxPar));

        {
          // Forward scan
          mln_forall (pxIn, pxOut, pxPar)
          {
            auto p             = pxPar->point();
            auto p_input_value = pxIn->val();

            // Make set
            auto root = p;

            bool is_possible_minimum = true;
            mln_forall (nxIn, nxOut, nxPar)
            {
              if (nxOut->val() == kUnseen)
                continue;

              auto n_input_value = nxIn->val();
              if (cmp(n_input_value, p_input_value)) // input(n) < input(p)
              {
                is_possible_minimum = false;
              }
              else if (cmp(p_input_value, n_input_value)) // input(p) < input(n) -> n is not a local minimum
              {
                const auto r = zfindroot(parent, nxPar->val());
                output(r)    = false;
              }
              else // Flat zone linking
              {
                const auto n_root = zfindroot(parent, nxPar->val());
                if (n_root != root)
                {
                  is_possible_minimum  = is_possible_minimum && output(n_root);
                  auto roots           = std::minmax(root, n_root);
                  parent(roots.second) = roots.first;
                  root                 = roots.first;
                }
              }
            }
            pxPar->val() = root;
            pxOut->val() = is_possible_minimum;
            output(root) = is_possible_minimum;
          }
        }

        int n_label = 0;
        // Labelisation
        {
          mln_forall (pxOut, pxPar)
          {
            if (pxOut->val())
            {
              if (pxPar->point() == pxPar->val()) // New minimum
                pxOut->val() = ++n_label;
              else
              {
                auto r       = zfindroot(parent, pxPar->val());
                pxOut->val() = output(r);
              }
            }
          }
        }
        mln_assertion((n_label <= value_traits<Label_t>::max()) &&
                      "Overflow detected (the number of label has exceed the label type capacity");
        return n_label;
      }
    }

    template <class Label_t, class I, class N, class Compare>
    mln_ch_value(I, Label_t) local_minima(const Image<I>& input_, const Neighborhood<N>& nbh_, int& nlabel, Compare cmp)
    {
      mln_entering("mln::labeling::local_minima");

      static_assert(std::is_integral<Label_t>::value && !std::is_same<Label_t, bool>::value,
                    "Label type must a non-bool integral type");

      constexpr Label_t kUnseen       = static_cast<Label_t>(-1);
      const I&          input         = exact(input_);
      const N&          nbh           = exact(nbh_);
      mln_ch_value(I, Label_t) output = imchvalue<Label_t>(input).adjust(nbh).init(kUnseen);

      if (extension::need_adjust(output, nbh))
      {
        auto out = extension::add_value_extension(output, kUnseen);
        nlabel   = details::local_minima(input, nbh, out, cmp);
      }
      else
      {
        mln::extension::fill(output, kUnseen);
        nlabel = details::local_minima(input, nbh, output, cmp);
      }
      return output;
    }

    template <class Label_t, class I, class N>
    mln_ch_value(I, Label_t) local_maxima(const Image<I>& input, const Neighborhood<N>& nbh, int& nlabel)
    {
      return local_minima<Label_t>(input, nbh, nlabel, std::greater<mln_value(I)>());
    }


  } // end of namespace mln::labeling
} // end of namespace mln

#endif //! MLN_LABELING_LOCAL_MINIMA_HPP
