#include <mln/core/image/image2d.hpp>
#include <mln/core/image/morphers/casted_image.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/trace.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/io/imsave.hpp>
#include <mln/colors/lab.hpp>
#include <mln/morpho/tos/ctos.hpp>
#include <mln/morpho/component_tree/compute_depth.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>
#include <mln/morpho/structural/opening.hpp>
#include <mln/morpho/structural/closing.hpp>

#include <tbb/pipeline.h>
#include <boost/format.hpp>
#include <mutex>
#include <apps/g2/compute_ctos.hpp>
#include <apps/tos/addborder.hpp>
#include <mln/io/imsave.hpp>
#include "video_tools.hh"
#include "smartdoc.hpp"
#include "export.hpp"


/****************************************************/
/**     CONFIG                                    ***/
/****************************************************/

const bool ENABLE_CACHING = true;
bool VIDEO_OUTPUT = false; // non-const modifiable by the cmdline

enum e_method { ON_LAB_L = 0, ON_LAB_B = 1, BACKUP_METHOD = 2, AUTO = 3, MTOS_METHOD = 4};
e_method APP_METHOD = AUTO;


/****************************************************/
/**     PROCESSORS                                ***/
/****************************************************/

std::mutex my_mutex;

/*
We have a pipeline with 3 filters:

Input filter -> Middle Filter -> Output Filter

The Input and Output filters are sequential and processes one item at a time
The Middle filter can process several items in parallel.

1. The input filter.
   Load a frame from a video and outputs a Milena image2d.

2. The Middle filter.
   Does the job. Gets an image2d and outputs an image2d.

3. The output filter.
   Write the frame. Gets an image2d and does nothing.
*/


///
class input_filter
{
public:

  input_filter(const char* filename)
  {
    m_videoStream = -1;
    m_nbframe = 0;

    av_init_packet(&packet_dec);

    pFormatCtx_Dec = init_format_context_decode(mln::trace::verbose,
                                                filename,
                                                m_videoStream,
                                                &pFrame_inYUV,
                                                &pFrame_inRGB,
                                                &Ctx_Dec);

    convert_ctx_yuv2rgb = sws_getContext(Ctx_Dec->width,
                                         Ctx_Dec->height,
                                         Ctx_Dec->pix_fmt,
                                         Ctx_Dec->width,
                                         Ctx_Dec->height,
                                         PIX_FMT_RGB24,
                                         SWS_BILINEAR, 0, 0, 0);

  }

  // No copy constructor !!
  input_filter(const input_filter& other) = delete;

  ~input_filter()
  {
    close_free_decode(&pFormatCtx_Dec, &Ctx_Dec, &pFrame_inYUV, &pFrame_inRGB);
  }

  mln::image2d<mln::rgb8>*
  operator () (tbb::flow_control& fc) const
  {
    /***********************************************************************/
    /**           De/en-coding loop with ffmpeg                           **/
    /***********************************************************************/
    int frameFinished = 0;
    while (av_read_frame(pFormatCtx_Dec, &packet_dec) >= 0)
      {
        if (packet_dec.stream_index == m_videoStream)
          {
            avcodec_decode_video2(Ctx_Dec, pFrame_inYUV, &frameFinished, &packet_dec);
            if (frameFinished)
              {
                mln::image2d<mln::rgb8>* out = new mln::image2d<mln::rgb8> ();
                *out = this->getCurrentFrame();
                ++m_nbframe;
                return out;
              }
          }
      }

    packet_dec.data = NULL;
    packet_dec.size = 0;
    packet_dec.stream_index = m_videoStream;

    while (avcodec_decode_video2(Ctx_Dec, pFrame_inYUV,
                                 &frameFinished, &packet_dec) >= 0)
      {
        if (frameFinished)
          {
            mln::image2d<mln::rgb8>* out = new mln::image2d<mln::rgb8> ();
            *out = this->getCurrentFrame();
            ++m_nbframe;
            return out;
          }
        else
          {
            break;
          }
      }
    fc.stop(); // NO MORE FRAME
    return NULL;
  }

  mln::image2d<mln::rgb8>
  getCurrentFrame() const
  {
    /* convert from YUV to RGB24 to decode video */
    sws_scale(convert_ctx_yuv2rgb,
              (const uint8_t * const*) pFrame_inYUV->data,
              pFrame_inYUV->linesize,
              0,
              Ctx_Dec->height,
              pFrame_inRGB->data,
              pFrame_inRGB->linesize);

    /* copy to mln image2d */

    size_t strides[2] = {(size_t)pFrame_inRGB->linesize[0], sizeof(mln::rgb8)};
    mln::box2d domain{{0,0},{(short)pFrame_inRGB->height,
          (short)pFrame_inRGB->width}};

    return mln::image2d<mln::rgb8>::from_buffer(pFrame_inRGB->data[0],
                                                domain, strides, true);
  }


  AVCodecContext*
  get_context()
  {
    return Ctx_Dec;
  }

  AVFormatContext*
  get_format_context()
  {
    return pFormatCtx_Dec;
  }

private:
  mutable AVPacket packet_dec;
  AVFrame *pFrame_inYUV, *pFrame_inRGB;
  AVCodecContext *Ctx_Dec;
  AVFormatContext *pFormatCtx_Dec;
  struct SwsContext *convert_ctx_yuv2rgb;


  int           m_videoStream;
  mutable int           m_nbframe;
};



struct middle_filter_result
{
  mln::image2d<mln::rgb8>               input;
  mln::image2d<mln::uint16>             depth;
  std::array<process_result_t, NQUAD>   res;
};


class middle_filter
{
public:

  middle_filter_result*
  operator() (mln::image2d<mln::rgb8>* input) const
  {
    using namespace mln;

    // Process
    middle_filter_result* mdr = new middle_filter_result;

    mdr->input = std::move(*input);
    delete input;

    // if (VIDEO_OUTPUT) {
    //   mln::box2d d;
    //   d.pmin = {0,0};
    //   d.pmax = (input->domain().pmax + 2) * 2 - 1;
    //   mdr->ima.resize(d);
    //   ptr_img_out = &(mdr->ima);
    // }

    typedef morpho::component_tree< unsigned, image2d<unsigned> > tree_t;
    tree_t tree;

    if (APP_METHOD == MTOS_METHOD)
      {
        tree = compute_ctos(mdr->input, &(mdr->depth));
      }
    else
      {
        //std::array<process_result_t, NQUAD>       resL, resB;
        //image2d< lab<float> > f = transform(mdr->input, [](rgb8 x) { return rgb2lab(x); });

        image2d<uint8> f;
        property_map<tree_t, unsigned> vmap;

        my_mutex.lock();
        if (APP_METHOD == AUTO)
          {
            std::array<process_result_t, NQUAD> resL, resB;

            // 1st try on LAB_B
            {
              f = transform(mdr->input, [](rgb8 x) -> uint8 {
                  lab<float> v = rgb2lab(x);
                  return (v[2] + 110) * 256 / 220;
                });

              f = addborder(f);
              tree = morpho::cToS(f, c4);
              resize(mdr->depth, tree._get_data()->m_pmap);
              vmap = morpho::compute_depth(tree);
              morpho::reconstruction(tree, vmap, mdr->depth);
              resB = process(tree, mdr->depth);
            }

            if (resB[0].energy > 0.78)
              {
                std::cout << "==> Using the B component." << std::endl;
                APP_METHOD = ON_LAB_B;
                mdr->res = resB;
                my_mutex.unlock();
                return mdr;
              }

            // 2nd try on LAB_L
            {
              f = transform(mdr->input, [](rgb8 x) -> uint8 {
                  lab<float> v = rgb2lab(x);
                  return v[0] * 256 / 100;
              });
              f = addborder(f);
              tree = morpho::cToS(f, c4);
              vmap = morpho::compute_depth(tree);
              morpho::reconstruction(tree, vmap, mdr->depth);
              resL = process(tree, mdr->depth);
            }
            if (resB[0].energy > 0.72 and resL[0].energy < 0.8)
              {
                std::cout << "==> Using the B component." << std::endl;
                APP_METHOD = ON_LAB_B;
                mdr->res = resB;
                my_mutex.unlock();
                return mdr;
              }
            else if (resL[0].energy > 0.8)
              {
                std::cout << "==> Using the L component." << std::endl;
                APP_METHOD = ON_LAB_L;
                mdr->res = resL;
                my_mutex.unlock();
                return mdr;
              }
            // 3. Backup
            {
              f = transform(mdr->input, [](rgb8 x) -> uint8 {
                  lab<float> v = rgb2lab(x);
                return (v[2] + 110) * 256 / 220;
                });

              rect2d r = make_rectangle2d(101, 1);
              f = morpho::structural::opening(f, r);
              f = morpho::structural::closing(f, r);
              f = addborder(f);
              tree = morpho::cToS(f, c4);
              vmap = morpho::compute_depth(tree);
              morpho::reconstruction(tree, vmap, mdr->depth);
              mdr->res = process(tree, mdr->depth);
            }
            {
              std::cout << "==> Using the BACKUP method." << std::endl;
              APP_METHOD = BACKUP_METHOD;
              my_mutex.unlock();
              return mdr;
            }
          }

        // We now which method to run OK.
        my_mutex.unlock();
        if (APP_METHOD == ON_LAB_B)
          {
            f = transform(mdr->input, [](rgb8 x) -> uint8 {
                lab<float> v = rgb2lab(x);
                return (v[2] + 110) * 256 / 220;
              });
          }
        else if (APP_METHOD == ON_LAB_L)
          {
            f = transform(mdr->input, [](rgb8 x) -> uint8 {
                lab<float> v = rgb2lab(x);
                return v[0] * 256 / 100;
              });
          }
        else if (APP_METHOD == BACKUP_METHOD)
          {
            f = transform(mdr->input, [](rgb8 x) -> uint8 {
                lab<float> v = rgb2lab(x);
                return (v[2] + 110) * 256 / 220;
              });

            rect2d r = make_rectangle2d(101, 1);
            f = morpho::structural::opening(f, r);
            f = morpho::structural::closing(f, r);
          }

        f = addborder(f);
        tree = morpho::cToS(f, c4);
        resize(mdr->depth, tree._get_data()->m_pmap);
        vmap = morpho::compute_depth(tree);
        morpho::reconstruction(tree, vmap, mdr->depth);
      }
    mdr->res = process(tree, mdr->depth);

    return mdr;
  }

};


class output_filter
{
public:
  output_filter(std::vector<SmartDoc_t>* results,
                const char* filename,
                AVCodecContext* Ctx_Dec,
                AVFormatContext* pFormatCtx_Dec)
  {
    if (VIDEO_OUTPUT) {
      pFormatCtx_Enc = init_format_context_encode(mln::trace::verbose,
                                                  filename,
                                                  Ctx_Dec, pFormatCtx_Dec,
                                                  &pFrame_outYUV, NULL, &Ctx_Enc);

      // Because the output image is in K0 + border
      // short w = (Ctx_Dec->width + 2) * 2 - 1;
      // short h = (Ctx_Dec->height + 2) * 2 - 1;
      short w = Ctx_Dec->width;
      short h = Ctx_Dec->height;


      pFrame_outRGB = avcodec_alloc_frame();
      av_image_alloc(pFrame_outRGB->data,
                     pFrame_outRGB->linesize,
                     w, h, PIX_FMT_RGB24, 32);
      pFrame_outRGB->width = w;
      pFrame_outRGB->height = h;
      pFrame_outRGB->format = PIX_FMT_RGB24;

      convert_ctx_rgb2out = sws_getContext(w,
                                           h,
                                           PIX_FMT_RGB24,
                                           Ctx_Enc->width,
                                           Ctx_Enc->height,
                                           Ctx_Enc->pix_fmt,
                                           SWS_BILINEAR, 0, 0, 0);

      m_output.resize(mln::box2d{{0,0}, {h,w}}, 5);
    }
    m_filename = filename;
    m_nbframe = 0;
    m_skipped = 0;
    m_results = results;
  }

  output_filter(const output_filter& other) = delete;

  ~output_filter()
  {
    if (VIDEO_OUTPUT) {
      encode_skipped_frames(m_skipped, m_nbframe, &pFormatCtx_Enc, m_filename,
                            &pFrame_outYUV, &Ctx_Enc, packet_enc);
      close_free_encode(&pFormatCtx_Enc, &Ctx_Enc, &pFrame_outYUV, &pFrame_outRGB);
    }
  }

  std::array<mln::point2d, 4>
  getquad(middle_filter_result* mdr) const
  {
    int selection = 0;
    bool dismissed[NQUAD] = {0,};
    if (not m_results->empty() and m_results->back().good)
      {
        auto lastr = m_results->back().quad;
        for (int i = 0; i < NQUAD; ++i)
          {
            bool dismiss = false;
            for (int j = 0; j < 4; ++j)
              if (l2dist(mdr->res[i].points[j], lastr[j]) > MAX_DISTANCE_INTER_FRAME) {
                dismiss = true;
                break;
              }
            dismissed[i] = dismiss;
            if (not dismiss) {
              selection = i;
              break;
            }
          }
      }


    std::cout << "### FRAME: " << m_nbframe << std::endl;
    for (int i = 0; i < NQUAD; ++i) {
      std::cout << "\t" << (selection == i ? '*' : ' ')
                << "Quad " << i << ": " << mdr->res[i].energy << " "
                << mdr->res[i].points[0]
                << mdr->res[i].points[1]
                << mdr->res[i].points[3]
                << mdr->res[i].points[2];
      if (dismissed[i])
        std::cout << " (dissmissed)";
      std::cout << std::endl;
    }
    return mdr->res[selection].points;
  }

  void
  operator() (middle_filter_result* mdr) const
  {
    std::array<mln::point2d, 4> quad = this->getquad(mdr);
    //mln::io::imsave(mdr->depth, "depth.tiff");

    if (VIDEO_OUTPUT)
      {
        // Copy the image to the output buffer.
        {
          //draw_quad_superimpose(quad, mdr->depth, m_output);
          m_output = mdr->input;
          draw_quad_superimpose(quad, m_output);

          char* ptrin = (char*) &(m_output.at(0,0));
          char* ptrout = (char*) pFrame_outRGB->data[0];
          size_t stride = m_output.strides()[0];
          for (unsigned i = 0; i < m_output.nrows(); ++i)
            {
              std::copy(ptrin, ptrin + (m_output.ncols() * sizeof(mln::rgb8)), ptrout);
              ptrin += stride;
              ptrout += pFrame_outRGB->linesize[0];
            }
        }
        const_cast<output_filter*>(this)->encode();
      }
    // Insert the smartdoc result in the vector.
    m_results->push_back({true, quad});
    delete mdr;
  }

private:
  void encode()
  {
    encode_video(m_skipped, &pFormatCtx_Enc, m_filename,
                 &pFrame_outRGB, &pFrame_outYUV, m_nbframe,
                 &Ctx_Enc, packet_enc, &convert_ctx_rgb2out);
    m_nbframe++;
  }


private:
  // For encoding
  AVPacket packet_enc;
  AVFrame *pFrame_outYUV, *pFrame_outRGB;
  AVCodecContext *Ctx_Enc;
  AVFormatContext *pFormatCtx_Enc;
  struct SwsContext *convert_ctx_rgb2out;

  // For other stuff
  mutable mln::image2d<mln::rgb8>  m_output;
  const char*              m_filename;
  mutable int              m_nbframe;
  mutable int              m_skipped;
  std::vector<SmartDoc_t>* m_results;
};






int main(int argc, char** argv)
{
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " input.mpg output.xml [method = 0] [number of threads=10] [output.mpg]\n"
      "method: (0) LAB_L, (1): LAB_B, (2): BACKUP, (3) AUTO, (4) MTOS\n";
    std::exit(1);
  }

  const char* input_path = argv[1];
  const char* output_path = argv[2];
  if (argc > 3) {
    APP_METHOD = (e_method) std::atoi(argv[3]);
  }

  int nthread = (argc > 4) ? std::atoi(argv[4]) : 10;
  const char* output_video_path = (argc > 5) ? argv[5] : NULL;
  VIDEO_OUTPUT = argc > 5;

  using namespace mln;

  // Initialization of the library
  av_register_all();
  avcodec_register_all();

  std::cout << input_path << std::endl;
  std::vector<SmartDoc_t> results;

  input_filter func1(input_path);
  output_filter func3(&results, output_video_path, func1.get_context(), func1.get_format_context());

  tbb::filter_t<void, image2d<rgb8>* > f1(tbb::filter::serial_in_order, std::ref(func1));

  tbb::filter_t<image2d<rgb8>*, middle_filter_result*> f2(tbb::filter::parallel, middle_filter ());

  tbb::filter_t<middle_filter_result*,void> f3(tbb::filter::serial_in_order, std::ref(func3));

  tbb::filter_t<void,void> f = f1 & f2 & f3;

  tbb::parallel_pipeline(nthread,f);

  export_xml(output_path, input_path, &results[0], results.size());
}
