#include "bucket.hpp"

namespace scribo::internal
{
  Buckets::Buckets(size_t n_max, size_t bs)
    : m_bucket_size(std::min(bs, n_max))
    , m_bucket_count((n_max + m_bucket_size - 1) / m_bucket_size)
    , m_container(m_bucket_count)
  {
  }

  size_t Buckets::get_bucket_number(int n) const noexcept
  {
    return std::min(m_bucket_count - 1, n / m_bucket_size);
  }
  size_t Buckets::get_bucket_number(const Tracker& f) const noexcept
  {
    return get_bucket_number(f.get_position());
  }

  void Buckets::insert(Tracker&& tracker)
  {
    auto k = get_bucket_number(tracker);
    m_container[k].push_back(std::move(tracker));
  }

  void Buckets::acquire(std::vector<Tracker>& trackers)
  {
    for (auto& tracker : trackers)
      insert(std::move(tracker));
    trackers.clear();
  }

  void Buckets::release(std::vector<Tracker>& trackers)
  {
    for (auto& c : m_container)
    {
      std::move(c.begin(), c.end(), std::back_inserter(trackers));
      c.clear();
    }
  }

  void Buckets::remove_if(size_t i, std::function<bool(const Tracker&)> pred, std::vector<Tracker>& out)
  {
    assert(i < m_bucket_count);
    auto& c           = m_container[i];
    auto [end1, end2] = std::partition_copy(std::move_iterator(c.begin()), std::move_iterator(c.end()),
                                            std::back_inserter(out), c.begin(), pred);
    c.erase(end2, c.end());
  }

  void Buckets::for_each_tracker(size_t i, std::function<void(Tracker&)> action)
  {
    assert(i < m_bucket_count);
    auto& c = m_container[i];
    std::for_each(c.begin(), c.end(), action);
  }

} // namespace scribo::internal