#include <mln/core/image/image2d.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/trace.hpp>
#include <mln/io/imsave.hpp>
#include <boost/format.hpp>

#include <apps/g2/compute_ctos.hpp>
#include "video_tools.hh"
#include "smartdoc.hpp"
#include "export.hpp"


/****************************************************/
/**     CONFIG                                    ***/
/****************************************************/

const bool ENABLE_CACHING = true;



int main(int argc, char** argv)
{
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " input.mpg output.xml [output.mpg]\n";
    std::exit(1);
  }

  const char* input_path = argv[1];
  const char* output_path = argv[2];
  const char* output_video_path = (argc > 3) ? argv[3] : NULL;
  bool VIDEO_OUTPUT = argc > 3;

  using namespace mln;

  /***********************************************************************/
  /**           De/en-coding loop with ffmpeg                           **/
  /***********************************************************************/

  // Initialization of the library
  AVPacket packet_dec, packet_enc;
  av_register_all();
  avcodec_register_all();
  av_init_packet(&packet_dec);


  // Get info about input/output videos
  int videoStream = -1;
  AVFrame *pFrame_inYUV, *pFrame_inRGB, *pFrame_outYUV, *pFrame_outRGB;
  AVCodecContext *Ctx_Enc, *Ctx_Dec;
  AVFormatContext *pFormatCtx_Dec, *pFormatCtx_Enc;

  pFormatCtx_Dec = init_format_context_decode(trace::verbose,
                                              input_path,
                                              videoStream, &pFrame_inYUV, &pFrame_inRGB,
                                              &Ctx_Dec);

  if (VIDEO_OUTPUT) {
    pFormatCtx_Enc = init_format_context_encode(trace::verbose,
                                                output_video_path,
                                                Ctx_Dec, pFormatCtx_Dec,
                                                &pFrame_outYUV, NULL, &Ctx_Enc);

    // Because K0 -> bug alignement
    int w = (pFrame_inRGB->width + 2) * 2 - 1;
    int h = (pFrame_inRGB->height + 2) * 2 - 1;
    int ws = (w * 3);
    // Si on est sur 96 octets (aligné sur 32 octets et 3 octets en même temps OK)
    // Sinon on aloue un peu plus
    if (ws % 96 != 0)
      w = ((ws / 96) + 1) * (96 / 3);

    pFrame_outRGB = avcodec_alloc_frame();
    av_image_alloc(pFrame_outRGB->data, pFrame_outRGB->linesize,
                   w, h, PIX_FMT_RGB24, 32); // Car aligné sur 32 et 24 en même temps
    pFrame_outRGB->width = w;
    pFrame_outRGB->height = h;
    pFrame_outRGB->format = PIX_FMT_RGB24;
  }



  // Main decoding loop
  {

    // Definition of milena types (they use external memory management layout)
    image2d<rgb8> img_in, img_out;
    {
      size_t strides[2] = {(size_t)pFrame_inRGB->linesize[0], sizeof(rgb8)};
      img_in = image2d<rgb8>::from_buffer(pFrame_inRGB->data[0],
                                          box2d{{0,0},{(short)pFrame_inRGB->height, (short)pFrame_inRGB->width}},
                                          strides);
    }
    if (VIDEO_OUTPUT) {
      short w = (pFrame_inRGB->width + 2) * 2 - 1;
      short h = (pFrame_inRGB->height + 2) * 2 - 1;
      // std::cout << "Width: " << pFrame_outRGB->width << "(real: " << w << ")" << std::endl;
      // std::cout << pFrame_outRGB->linesize[0] << std::endl;
      size_t strides[2] = {(size_t)pFrame_outRGB->linesize[0], sizeof(rgb8)};
      img_out = image2d<rgb8>::from_buffer(pFrame_outRGB->data[0],
                                           box2d{{0,0},{h, w}},
                                           strides);
    }


    // Defintion of contexts from YUV-RGB conversion and K0 desimmersion
    struct SwsContext *convert_ctx_yuv2rgb, *convert_ctx_rgb2out;
    convert_ctx_yuv2rgb = sws_getContext(Ctx_Dec->width,
                                         Ctx_Dec->height,
                                         Ctx_Dec->pix_fmt,
                                         Ctx_Dec->width,
                                         Ctx_Dec->height,
                                         PIX_FMT_RGB24,
                                         SWS_BILINEAR, 0, 0, 0);

    if (VIDEO_OUTPUT) {
      convert_ctx_rgb2out = sws_getContext(pFrame_outRGB->width,
                                           pFrame_outRGB->height,
                                           PIX_FMT_RGB24,
                                           Ctx_Enc->width,
                                           Ctx_Enc->height,
                                           Ctx_Enc->pix_fmt,
                                           SWS_BILINEAR, 0, 0, 0);
    }

    tree_t tree;
    image2d<uint16> depth;
    int frameFinished = 0;
    int nbframe = 0;
    int skipped = 0; // ???
    std::vector<SmartDoc_t> results;
    image2d<rgb8>* ptr_img_out = (VIDEO_OUTPUT) ? &img_out : NULL;

    std::cout << "Decoding: " << std::endl;
    while (av_read_frame(pFormatCtx_Dec, &packet_dec) >= 0)
      if (packet_dec.stream_index == videoStream)
        {
          avcodec_decode_video2(Ctx_Dec, pFrame_inYUV, &frameFinished, &packet_dec);
          if (frameFinished)
            {
              std::cout << "Processing #" << nbframe << std::endl;

              /* convert from YUV to RGB24 to decode video */
              sws_scale(convert_ctx_yuv2rgb,
                        (const uint8_t * const*) pFrame_inYUV->data,
                        pFrame_inYUV->linesize,
                        0,
                        Ctx_Dec->height,
                        pFrame_inRGB->data,
                        pFrame_inRGB->linesize);


              // Process
              SmartDoc_t res;
              tree = compute_ctos(img_in, &depth);
              std::tie(res.good, res.quad) = process(tree, depth, ptr_img_out);

              // Output the video
              if (VIDEO_OUTPUT) {
                encode_video(skipped, &pFormatCtx_Enc, output_video_path,
                             &pFrame_outRGB, &pFrame_outYUV, nbframe,
                             &Ctx_Enc, packet_enc, &convert_ctx_rgb2out);

                io::imsave(img_out, (boost::format("out-%03d.png") % nbframe).str());
              }


              // Write results.
              results.push_back(res);

              nbframe++;
            }
        }

    export_xml(output_path, input_path, &results[0], results.size());


    if (VIDEO_OUTPUT) {
      encode_skipped_frames(skipped, nbframe, &pFormatCtx_Enc, output_video_path, &pFrame_outYUV, &Ctx_Enc, packet_enc);
      close_free_encode(&pFormatCtx_Enc, &Ctx_Enc, &pFrame_outYUV, &pFrame_outRGB);
    }
  }

  close_free_decode(&pFormatCtx_Dec, &Ctx_Dec, &pFrame_inYUV, &pFrame_inRGB);

}
