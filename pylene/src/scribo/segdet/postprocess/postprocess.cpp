#include "../detect_line.hpp"
#include "../segment_to_X.hpp"

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/range/foreach.hpp>
#include <scribo/segdet.hpp>

#include <cmath>

namespace scribo::internal
{
  /**
   * Binarize the image
   * @param img Image to binarize
   */
  void binarize_img(image2d<uint16_t> img)
  {
    mln_foreach (auto p, img.domain())
    {
      if (img(p) != 0)
        img(p) = 1;
      else
        img(p) = 0;
    }
  }

  /**
   * Compute the intersection between two images and store it in out
   * @param img first image
   * @param img2 second image
   * @param out image storing result
   */
  void intersect(image2d<std::uint16_t> img, image2d<std::uint16_t> img2, image2d<std::uint16_t> out)
  {
    mln_foreach (auto pt, img.domain())
      out(pt) = img(pt) * img2(pt);
  }

  /**
   * Remove duplication of segment
   * @param segments_to_compare
   * @param segments_removable
   * @param width
   * @param height
   */
  void remove_dup(const std::vector<Segment>& segments_to_compare, std::vector<Segment>& segments_removable, int width,
                  int height, const Descriptor& descriptor)
  {
    auto first_output  = std::get<0>(segment_to_label(segments_to_compare, width, height, false));
    auto second_output = std::get<0>(segment_to_label(segments_removable, width, height, false));

    binarize_img(first_output);

    image2d<std::uint16_t> intersection = image2d<std::uint16_t>(width, height);
    mln::fill(intersection, 0);

    intersect(first_output, second_output, intersection);

    std::vector<uint16_t> segments(segments_removable.size(), 0);

    mln_foreach (auto v, intersection.values())
    {
      if (v >= first_label)
        segments[v - first_label]++;
    }

    int k = 0;
    for (int i = 0; i < (int)segments.size(); i++)
    {
      float segments_ratio = 0;
      if (segments_removable[i - k].nb_pixels != 0)
        segments_ratio = segments[i] / static_cast<float>(segments_removable[i - k].nb_pixels);
      if (segments_removable[i - k].nb_pixels == 0 || segments_ratio > descriptor.threshold_intersection)
      {
        segments_removable.erase(segments_removable.begin() + i - k);
        k++;
      }
    }
  }

  /**
   * Call remove duplicates for horizontal and vertical segments
   * @param pair Pair (horizontal segments,vertical segments)
   * @param img_width Width of the image where segments were extract
   * @param img_height Height of the image where segments were extract
   * @param descriptor Descriptor of settings (used : traversal mode)
   */
  void remove_duplicates(std::pair<std::vector<Segment>, std::vector<Segment>>& pair, int img_width, int img_height,
                         const Descriptor& descriptor)
  {
    remove_dup(pair.first, pair.second, img_width, img_height, descriptor);
    remove_dup(pair.second, pair.first, img_width, img_height, descriptor);
  }

  /**
   * Only keep segments that are above a given length and combine pair
   * @param p Pair of vectors of segments
   * @param descriptor Descriptor of parameter (used : min_length)
   * @return
   */
  std::vector<Segment> filter_length(std::pair<std::vector<Segment>, std::vector<Segment>>& p,
                                     const Descriptor&                                      descriptor)
  {
    std::vector<Segment> res{};

    for (auto& seg : p.first)
    {
      if (seg.length >= descriptor.min_length)
        res.push_back(seg);
    }
    for (auto& seg : p.second)
    {
      if (seg.length >= descriptor.min_length)
        res.push_back(seg);
    }

    return res;
  }

  /**
   * Post process segments linking them and removing duplications
   * @param pair Pair (horizontal segments,vertical segments)
   * @param img_width Width of the image where segments were extract
   * @param img_height Height of the image where segments were extract
   */
  std::vector<Segment> post_process(std::vector<Segment>& hsegments, std::vector<Segment>& vsegments, int img_width,
                                    int img_height, const Descriptor& descriptor)
  {
    auto pair = std::make_pair(hsegments, vsegments);

    if (descriptor.remove_duplicates && descriptor.traversal_mode == scribo::SEGDET_PROCESS_TRAVERSAL_MODE_ENUM::HORIZONTAL_VERTICAL)
      remove_duplicates(pair, img_width, img_height, descriptor);

    auto segments = filter_length(pair, descriptor);

    return segments;
  }
} // namespace scribo::internal