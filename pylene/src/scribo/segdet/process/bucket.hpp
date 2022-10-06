#pragma once

#include <vector>
#include "filter.hpp"

namespace scribo::internal
{
  struct Buckets
  {
    private:
      const size_t m_bucket_size;
      const size_t m_bucket_count;
      std::vector<std::vector<Filter>> m_container;


    public:
      Buckets(size_t n_max, size_t bucket_size);
      size_t get_bucket_count() const { return m_bucket_count; } 
      size_t get_bucket_number(int n) const noexcept;
      size_t get_bucket_number(const Filter& f) const noexcept;
      void insert(Filter&& filter);
      void acquire(std::vector<Filter>& filters);
      void release(std::vector<Filter>& filters);

      ///
      /// @brief Move the elements that verifies a given predicate from the i-th bucket to the end of the list \p out. 
      /// 
      /// @param i 
      /// @param pred 
      /// @param out 
      ///
      void remove_if(int i, std::function<bool(const Filter&)> pred, std::vector<Filter>& out);

      ///
      /// @brief Applies the callback \p action on each item of the i-th bucket 
      /// 
      /// @param i 
      /// @param action 
      ///
      void for_each_filter(int i, std::function<void(Filter&)> action);

    //std::vector<Filter>& get_bucket(size_t i);
  };
} // namespace scribo::internal