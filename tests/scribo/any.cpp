#include <gtest/gtest.h>

#include "../../pylene/src/scribo/segdet/process/bucket.hpp"

TEST(Segdet, size_1)
{
    int n_max = 1;
    int bucket_size = 30;
    
    scribo::internal::Buckets buckets(n_max, bucket_size);

    EXPECT_EQ(buckets.bucket_size, 1);
    EXPECT_EQ(buckets.bucket_count, 1);
}

TEST(Segdet, size_100)
{
    int n_max = 100;
    size_t bucket_size = 30;
    
    scribo::internal::Buckets buckets(n_max, bucket_size);

    EXPECT_EQ(buckets.bucket_size, 30);
    EXPECT_EQ(buckets.bucket_count, 4);
    EXPECT_EQ(buckets.get_bucket_number(10), 0);
    EXPECT_EQ(buckets.get_bucket_number(30), 1);
    EXPECT_EQ(buckets.get_bucket_number(95), 3);
    EXPECT_EQ(buckets.get_bucket_number(45), 1);
}

TEST(Segdet, size_120)
{
    int n_max = 120;
    size_t bucket_size = 30;
    
    scribo::internal::Buckets buckets(n_max, bucket_size);

    EXPECT_EQ(buckets.bucket_size, 30);
    EXPECT_EQ(buckets.bucket_count, 4);
    EXPECT_EQ(buckets.get_bucket_number(10), 0);
    EXPECT_EQ(buckets.get_bucket_number(30), 1);
    EXPECT_EQ(buckets.get_bucket_number(95), 3);
    EXPECT_EQ(buckets.get_bucket_number(45), 1);
    EXPECT_EQ(buckets.get_bucket_number(119), 3);
    EXPECT_EQ(buckets.get_bucket_number(120), 3);
    EXPECT_EQ(buckets.get_bucket_number(121), 3);
}