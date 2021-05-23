#include "mln/contrib/segdet/evaluation.hpp"
#include "mln/core/algorithm/fill.hpp"
#include "mln/core/range/foreach.hpp"
#include <map>
#include <mln/io/imsave.hpp>

namespace mln::contrib::segdet
{
  size_t image_max(mln::image2d<u_int8_t> img)
  {
    uint8_t max = 0;

    mln_foreach (auto v, img.values())
    {
      if (v > max)
        max = v;
    }

    return max;
  }

  mln::image2d<u_int8_t> image_where_equal(mln::image2d<u_int8_t> img, u_int8_t value, int& count)
  {
    image2d<uint8_t> output = image2d<uint8_t>(img.width(), img.height());

    mln_foreach (auto d, img.domain())
    {
      if (img(d) == value)
      {
        count++;
        output(d) = 1;
      }
      else
        output(d) = 0;
    }

    return output;
  }

  mln::image2d<u_int8_t> image_where_equal(mln::image2d<u_int8_t> img, u_int8_t value)
  {
    int k = 0;
    return image_where_equal(img, value, k);
  }

  int image_count_value(mln::image2d<u_int8_t> img, uint8_t value)
  {
    int count = 0;

    mln_foreach (auto v, img.values())
    {
      if (v == value)
        count++;
    }

    return count;
  }

  mln::image2d<u_int8_t> image_intersect(mln::image2d<uint8_t> img1, mln::image2d<uint8_t> img2)
  {
    image2d<uint8_t> out = image2d<uint8_t>(img1.width(), img1.height());

    mln_foreach (auto d, img1.domain())
      out(d) = img1(d) * img2(d);

    return out;
  }

  std::map<u_int8_t, int> image_unique(mln::image2d<u_int8_t> img)
  {
    std::map<u_int8_t, int> ret = {};

    mln_foreach (auto v, img.values())
    {
      if (ret.contains(v))
        ret[v]++;
      else
        ret.insert(std::make_pair(v, 1));
    }

    ret.erase(0);
    ret.erase(1);
    ret.erase(2);

    return ret;
  }

  std::vector<double> score_detection(mln::ndbuffer_image ref_buf, mln::ndbuffer_image output_buf)
  {
    mln::image2d<uint8_t> ref    = ref_buf.__cast<uint8_t, 2>();
    mln::image2d<uint8_t> output = output_buf.__cast<uint8_t, 2>();

    std::vector<double> seg_recall    = {};
    std::vector<double> seg_precision = {};
    std::vector<double> seg_fscore    = {};

    auto width  = ref.width();
    auto height = ref.height();

    size_t wanted_count_segment = image_max(ref);

    for (size_t i = 3; i < wanted_count_segment + 1; i++)
    {
      int  wanted_count      = 0;
      auto wanted_pixels     = image_where_equal(ref, i, wanted_count);
      auto not_wanted_pixels = image_where_equal(wanted_pixels, 0);

      auto true_positive_img    = image_intersect(wanted_pixels, output);
      auto true_positive_unique = image_unique(true_positive_img);

      int true_positive_count = 0;
      for (auto& kvp : true_positive_unique)
        true_positive_count += kvp.second;

      auto false_positive_count = 0;
      for (auto& kvp : true_positive_unique)
      {
        if (kvp.second / (double)true_positive_count < 0.10)
          continue;

        auto labelled_only_segment = image_where_equal(output, kvp.first);
        auto seg_intersection      = image_intersect(labelled_only_segment, not_wanted_pixels);
        false_positive_count += width * height - image_count_value(seg_intersection, 0);
      }

      auto srecall = (double) true_positive_count / (double) wanted_count;

      double sprecision = 0;
      if (true_positive_count + false_positive_count != 0)
        sprecision = ((double) true_positive_count) / (double) (true_positive_count + false_positive_count);

      double sfscore = 0;
      if (srecall + sprecision != 0)
        sfscore = 2 * (srecall * sprecision) / (srecall + sprecision);

      seg_recall.push_back(srecall);
      seg_precision.push_back(sprecision);
      seg_fscore.push_back(sfscore);
    }

    double recall    = 0;
    double precision = 0;
    double fscore    = 0;

    auto size = seg_precision.size();
    for (size_t i = 0; i < size; i++)
    {
      recall += seg_recall[i];
      precision += seg_precision[i];
      fscore += seg_fscore[i];
    }

    recall /= (double) size;
    precision /= (double) size;
    fscore /= (double) size;

    return {recall, precision, fscore};
  }
} // namespace mln::contrib::segdet
