// Copyright (C) 2013, 2014, 2015 EPITA Research and Development
// Laboratory (LRDE)
// Confidential - All Rights Reserved.

#ifndef VIDEO_TOOLS_HH
#define	VIDEO_TOOLS_HH

extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

}


#include <stdlib.h>
#include <stdio.h>
#include <iostream>
// #include <mln/value/int_u8.hh>
// #include <mln/value/rgb8.hh>
// #include <mln/core/image/image2d.hh>
// #include <mln/io/magick/save.hh>

//using namespace mln;
using namespace std;


void display_avformatcontext(AVFormatContext *context);
void display_avcodeccontext(AVCodecContext* context);
void display_avstream(AVStream* context);

unsigned int get_nb_frames(int ind_start, int ind_end, AVFormatContext *pFormatCtx_Dec, AVCodecContext* Ctx_Dec);
AVFormatContext *init_format_context_decode(const bool is_save_step_video, const char* input_filename, int &videoStream, AVFrame **pFrame_inYUV, AVFrame **pFrame_inRGB, AVCodecContext** Ctx_Dec);
AVFormatContext *init_format_context_encode(const bool is_save_step_video, const char* output_filename, AVCodecContext* Ctx_Dec, AVFormatContext *pFormatCtx_Dec, AVFrame **pFrame_outYUV, AVFrame **pFrame_outRGB, AVCodecContext** Ctx_Enc);

void init_stream(AVFormatContext* pFormatCtx_Enc);
void allocate_frames(AVCodecContext* Ctx, AVFrame **pFrame_YUV, AVFrame **pFrame_RGB);

AVFormatContext* get_format_context_enc(const char* filename);
AVCodecContext* get_codec_context_enc(AVCodecContext* dec, AVFormatContext* format_enc, AVFormatContext* format_dec);
AVFormatContext* get_format_context_dec(const char* filename);
AVCodecContext* get_codec_context_dec(AVFormatContext *pFormatCtx_Dec, int& videoStream);

// AVFrame* copy_image2d_rgb_to_AvFame(image2d<value::rgb8> image_out);
// void copy_image2d_rgb_to_AvFame_without_alloc(AVFrame **pFrame_out, image2d<value::rgb8> image_out);
// image2d<value::rgb8> copy_AvFrame_to_image2d_rgb(AVFrame *pFrame);
// AVFrame * copy_image2d_bool_to_AvFame(image2d<bool> image_out);
// image2d<value::int_u8> copy_AvFrame_to_image2d_intu(AVFrame * pFrame_inYUV);

void encode_video(int& skipped, AVFormatContext** pFormatCtx_Bin, const char* filename, AVFrame** pFrame_binRGB,
        AVFrame** pFrame_binYUV, int i, AVCodecContext** Ctx_Bin, AVPacket& packet_bin, struct SwsContext** convert_ctx);

void encode_skipped_frames(int nb_skipped, int i, AVFormatContext** pFormatCtx, const char* filename, AVFrame** pFrame_outYUV,
        AVCodecContext** Ctx, AVPacket& packet);

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);

void close_free(AVFormatContext** pFormatCtx_Enc, AVFormatContext** pFormatCtx_Dec, AVFormatContext** pFormatCtx_Bin,
        AVCodecContext** Ctx_Dec, AVCodecContext** Ctx_Enc, AVCodecContext** Ctx_Bin, AVFrame** pFrame_inYUV, AVFrame** pFrame_inRGB,
        AVFrame** pFrame_outYUV, AVFrame** pFrame_binYUV);

void close_free_decode(AVFormatContext** pFormatCtx_Dec, AVCodecContext** Ctx_Dec, AVFrame** pFrame_inYUV, AVFrame** pFrame_inRGB);

void close_free_encode(AVFormatContext** pFormatCtx_Enc, AVCodecContext** Ctx_Enc, AVFrame** pFrame_outYUV, AVFrame** pFrame_outRGB);

AVFrame* alloc_frame(unsigned int w, unsigned int h);

#endif	/* VIDEO_TOOLS_HH */

