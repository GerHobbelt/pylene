#include <vector>

#include "filter.hpp"

namespace scribo::internal
{
  struct Buckets
  {
    const size_t bucket_size;
    const size_t bucket_count;

    std::vector<std::vector<Filter>> container;

    Buckets(size_t n_max, size_t bucket_size)
      : bucket_size(std::min(bucket_size, n_max))
      , bucket_count(n_max / bucket_size + (n_max % bucket_size == 0 ? 0 : 1))
    {
      container = std::vector<std::vector<Filter>>();
      for (size_t i = 0; i < bucket_count; i++)
        container.push_back(std::vector<Filter>());
    }

    size_t get_bucket_number(size_t n) { return std::min(bucket_count - 1, n / bucket_size); }

    size_t get_bucket_number(Filter& f) { return get_bucket_number(f.get_position()); }

    void insert(Filter&& filter) { container[get_bucket_number(filter)].push_back(std::move(filter)); }

    void fill(std::vector<Filter>& filters)
    {
      for (auto& filter : filters)
        insert(std::move(filter));
      filters.clear();
    }

    void empty(std::vector<Filter>& filters)
    {
      for (size_t i = 0; i < bucket_count; i++)
      {
        for (size_t j = 0; j < container[i].size(); j++)
          filters.push_back(std::move(container[i][j]));
        container[i].clear();
      }
    }

    std::vector<Filter>& get_bucket(size_t i) { return container[i]; }
  };
} // namespace scribo::internal