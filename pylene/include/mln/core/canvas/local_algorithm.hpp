#pragma once

#include <mln/core/concept/structuring_elements.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <mln/core/trace.hpp>
#include <mln/core/box.hpp>

namespace mln::canvas
{

  template <class SE, class I, class J, class Self = void>
  // This concept check makes an ICE with MSVC
  #ifndef _MSC_VER
   requires concepts::StructuringElement<SE, image_point_t<I>>&& concepts::Image<I>&& concepts::Image<J>
  #endif
  class LocalAlgorithm
  {
    static_assert(mln::is_a<SE, experimental::StructuringElement>());
    static_assert(mln::is_a<I, experimental::Image>());
    static_assert(mln::is_a<J, experimental::Image>());

    static_assert(std::is_same_v<image_domain_t<I>, image_domain_t<J>>,
                  "Running a local algorithm on two images with differents domain types is not supported.");


  protected:
    SE m_se;
    I& m_i; // Image that will be accessed through ima(n) (and ima(p) maybe)
    J& m_j; // Image that will be accessed through ima(p)

  public:
    LocalAlgorithm(const SE& se, I& f, J& g)
      : m_se{se}
      , m_i{f}
      , m_j{g}
    {
    }

    virtual void Execute();

  protected:
    using self_t = std::conditional_t<std::is_void_v<Self>, LocalAlgorithm*, Self*>;

    self_t self() { return static_cast<self_t>(this); }


    virtual void ExecuteAtLineStart() = 0;

    virtual void EvalBeforeLocalLoop(image_reference_t<I> pval_i, image_reference_t<J> pval_j) = 0;
    virtual void EvalInLocalLoop(image_reference_t<I> nval_i, image_reference_t<I> pval_i,
                                 image_reference_t<J> pval_j)                                  = 0;
    virtual void EvalAfterLocalLoop(image_reference_t<I> pval_i, image_reference_t<J> pval_j)  = 0;
  };

  // To be incremental:
  // * the domain contiguous on rows
  // * the SE must incremental

  // FIXME:
  // we need to know if a domain is "row-contiguous" that is if the iteration leads to a unit move.
  namespace details
  {
    template <class Domain, class = void>
    struct is_domain_row_contiguous : std::false_type
    {
    };

    template <class Impl>
    struct is_domain_row_contiguous<mln::experimental::_box<Impl>> : std::true_type
    {
    };
  }


  template <class SE, class I, class J, class Self = void>
  class IncrementalLocalAlgorithm : public LocalAlgorithm<SE, I, J, Self>
  {
    static_assert(SE::incremental::value, "The Structuring Element must be incremental.");
    static_assert(details::is_domain_row_contiguous<image_domain_t<I>>::value, "The domain must be quite regular.");

  public:
    using IncrementalLocalAlgorithm::LocalAlgorithm::LocalAlgorithm;


    virtual void Execute() override;


  private:
    using self_t = std::conditional_t<std::is_void_v<Self>, IncrementalLocalAlgorithm*, Self*>;
    self_t self() { return static_cast<self_t>(this); }


    virtual void EvalInDecLoop(image_reference_t<I> nval_i, image_reference_t<I> pval_i,
                               image_reference_t<J> pval_j) = 0;

    virtual void EvalInIncLoop(image_reference_t<I> nval_i, image_reference_t<I> pval_i,
                               image_reference_t<J> pval_j) = 0;
  };


  template <class SE, class I, class J, class Self>
  void LocalAlgorithm<SE, I, J, Self>::Execute()
  {
    mln_entering("LocalAlgorithm::Execute (Non-incremental)");
    auto zz = ranges::view::zip(m_i.new_pixels(), m_j.new_pixels());
    for (auto rows : ranges::rows(zz))
    {
      self()->ExecuteAtLineStart();
      for (auto [px_i, px_j] : rows)
      {
        self()->EvalBeforeLocalLoop(px_i.val(), px_j.val());
        for (auto nx_i : m_se(px_i))
          self()->EvalInLocalLoop(nx_i.val(), px_i.val(), px_j.val());
        self()->EvalAfterLocalLoop(px_i.val(), px_j.val());
      }
    }
  }

  template <class SE, class I, class J, class Self>
  void IncrementalLocalAlgorithm<SE, I, J, Self>::Execute()
  {
    if (!this->m_se.is_incremental())
    {
      mln::trace::warn("[Performance] The accumulator is not incremental.");
      this->LocalAlgorithm<SE, I, J, Self>::Execute();
      return;
    }


    mln_entering("LocalAlgorithm::Execute (Incremental)");
    auto inc = this->m_se.inc();
    auto dec = this->m_se.dec();

    auto zz = ranges::view::zip(this->m_i.new_pixels(), this->m_j.new_pixels());
    for (auto rows : ranges::rows(zz))
    {
      self()->ExecuteAtLineStart();

      auto b = ::ranges::begin(rows);
      auto e = ::ranges::end(rows);

      // The first iteration is a normal one
      if (b != e)
      {
        auto [px_i, px_j] = *b;
        self()->EvalBeforeLocalLoop(px_i.val(), px_j.val());
        for (auto nx : this->m_se(px_i))
          self()->EvalInLocalLoop(nx.val(), px_i.val(), px_j.val());
        self()->EvalAfterLocalLoop(px_i.val(), px_j.val());
      }

      // Next iterations
      for (++b; b != e; ++b)
      {
        auto [px_i, px_j] = *b;

        // Untake part
        for (auto nx : dec(px_i))
          self()->EvalInDecLoop(nx.val(), px_i.val(), px_j.val());

        // Take part
        for (auto nx : inc(px_i))
          self()->EvalInIncLoop(nx.val(), px_i.val(), px_j.val());

        self()->EvalAfterLocalLoop(px_i.val(), px_j.val());
      }
    }
  }

} // namespace mln::canvas
