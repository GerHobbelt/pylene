#pragma once

#include "tracker.hpp"
#include <vector>

namespace scribo::internal
{
  struct Buckets
  {
  private:
    const size_t                      m_bucket_size;
    const size_t                      m_bucket_count;
    std::vector<std::vector<Tracker>> m_container;


  public:
    Buckets(size_t n_max, size_t bucket_size);
    size_t get_bucket_count() const { return m_bucket_count; }
    size_t get_bucket_size() const { return m_bucket_size; }
    size_t get_bucket_number(int n) const noexcept;
    size_t get_bucket_number(const Tracker& f) const noexcept;
    void   insert(Tracker&& tracker);
    void   acquire(std::vector<Tracker>& trackers);
    void   release(std::vector<Tracker>& trackers);

    ///
    /// @brief Move the elements that verifies a given predicate from the i-th bucket to the end of the list \p out.
    ///
    /// @param i
    /// @param pred
    /// @param out
    ///
    void remove_if(size_t i, std::function<bool(const Tracker&)> pred, std::vector<Tracker>& out);

    ///
    /// @brief Applies the callback \p action on each item of the i-th bucket
    ///
    /// @param i
    /// @param action
    ///
    void for_each_tracker(size_t i, std::function<void(Tracker&)> action);

    // std::vector<Tracker>& get_bucket(size_t i);
  };
} // namespace scribo::internal