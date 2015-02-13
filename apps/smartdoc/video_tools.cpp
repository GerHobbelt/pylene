// Copyright (C) 2013, 2014, 2015 EPITA Research and Development
// Laboratory (LRDE)
// Confidential - All Rights Reserved.

#include "video_tools.hh"

void display_avformatcontext(AVFormatContext *context) {
  cout << "pFormatCtx->start_time = " << context->start_time << endl;
  cout << "pFormatCtx->duration = " << context->duration << endl;
  cout << "pFormatCtx->bit_rate = " << context->bit_rate << endl;
  cout << "pFormatCtx->duration = " << context->duration << endl;
}

void display_avcodeccontext(AVCodecContext* context) {
  cout << "CodecCtx->bit_rate = " << context->bit_rate << endl;
  cout << "CodecCtx->timebase = " << context->time_base.num << "/" << context->time_base.den << endl;
  cout << "CodecCtx->ticks_per_frame = " << context->ticks_per_frame << endl;
  //  cout << "CodecCtx->framerate = " << context->framerate.num << "/" << context->framerate.den << endl;
  cout << "CodecCtx->pkt_timebase = " << context->pkt_timebase.num << "/" << context->pkt_timebase.den << endl;
   cout << "CodecCtx->sample_rate = " << context->sample_rate << endl;
}

void display_avstream(AVStream* context) {
  cout << "Stream->start_time = " << context->start_time << endl;
  cout << "Stream->duration = " << context->duration << endl;
  cout << "Stream->agv_frame_rate = " << context->avg_frame_rate.num << "/" << context->avg_frame_rate.den << endl;
  cout << "Stream->timebase = " << context->time_base.num << "/" << context->time_base.den << endl;
}


unsigned int get_nb_frames(int ind_start, int ind_end, AVFormatContext *pFormatCtx_Dec, AVCodecContext* Ctx_Dec) {

    unsigned int nb_frames_total = pFormatCtx_Dec->duration * Ctx_Dec->time_base.den / Ctx_Dec->time_base.num / 1000000;

//    cout << "pFormatCtx_Dec->duration = " << pFormatCtx_Dec->duration << endl;
//    cout << "Ctx_Dec->time_base.den = " << Ctx_Dec->time_base.den << endl;
//    cout << "Ctx_Dec->time_base.num = " << Ctx_Dec->time_base.num << endl;
//    cout << "pFormatCtx_Dec->bit_rate = " << pFormatCtx_Dec->bit_rate << endl;
//    cout << "pFormatCtx_Dec->fps_probe_size = " << pFormatCtx_Dec->fps_probe_size << endl;
//
//    cout << "pFormatCtx_Dec->streams->codec_info_nb_frames = " << pFormatCtx_Dec->streams[0]->codec_info_nb_frames << endl;
//    cout << "pFormatCtx_Dec->streams->codec_info_nb_frames = " << pFormatCtx_Dec->streams[1]->codec_info_nb_frames << endl;

    if (ind_end < ind_start) {
        cout << endl << "       Error in command line : -end must be superior to -start value ! " << endl;
    }

    if (ind_end) {
        cout << endl << "                                  Frames processed from " << ind_start << " to " << ind_end - 1 << " on " << nb_frames_total - 1 << " total " << endl;
    } else {
        cout << endl << "                                  Processing the whole video : From frames 0 to " << nb_frames_total - 1 << endl;
    }
    return nb_frames_total;
}

AVFormatContext *init_format_context_decode(const bool is_save_step_video, const char* input_filename, int &videoStream, AVFrame **pFrame_inYUV, AVFrame **pFrame_inRGB, AVCodecContext** Ctx_Dec) {
    AVFormatContext *pFormatCtx_Dec = get_format_context_dec(input_filename);
    pFormatCtx_Dec->probesize*=10;
    printf("De CODEC %p\n", pFormatCtx_Dec);
    (*Ctx_Dec) = get_codec_context_dec(pFormatCtx_Dec, videoStream);
    if (is_save_step_video)
        av_dump_format(pFormatCtx_Dec, 0, input_filename, 0); // pFormatCtx_Dec contient "interleaved video" ?
    allocate_frames(*Ctx_Dec, pFrame_inYUV, pFrame_inRGB);
    return pFormatCtx_Dec;
}

AVFormatContext *init_format_context_encode(const bool is_save_step_video, const char* output_filename, AVCodecContext* Ctx_Dec, AVFormatContext *pFormatCtx_Dec, AVFrame **pFrame_outYUV, AVFrame **pFrame_outRGB, AVCodecContext** Ctx_Enc) {
    if (output_filename) {
        AVFormatContext *pFormatCtx_Enc = get_format_context_enc(output_filename);
        *Ctx_Enc = get_codec_context_enc(Ctx_Dec, pFormatCtx_Enc, pFormatCtx_Dec);
        if (is_save_step_video)
            av_dump_format(pFormatCtx_Enc, 0, output_filename, 1);
        init_stream(pFormatCtx_Enc);
        allocate_frames(*Ctx_Enc, pFrame_outYUV, pFrame_outRGB);
        return pFormatCtx_Enc;
    }
    return NULL;
}

AVFrame* alloc_frame(unsigned int w, unsigned int h) {

    AVFrame *pFrame_out;
    pFrame_out = avcodec_alloc_frame();

    //pFrame_out->linesize[0] = w * 3;
    pFrame_out->width = w;
    pFrame_out->height = h;
    pFrame_out->format = PIX_FMT_RGB24;
    //unsigned nrows = pFrame_out->height = h;

    av_image_alloc(pFrame_out->data, pFrame_out->linesize, w, h, PIX_FMT_RGB24, 32);

    return pFrame_out;
}

#if 0

AVFrame* copy_image2d_rgb_to_AvFame(image2d<value::rgb8> image_out) {

    int w = image_out.ncols();
    int h = image_out.nrows();

    AVFrame *pFrame_out;
    pFrame_out = avcodec_alloc_frame();

    pFrame_out->format = PIX_FMT_RGB24;
    /*pFrame_out->linesize[0] = w * 3;*/
    pFrame_out->width = w;
    //pFrame_out->height = h;
 /*pFrame_out->data[0] = 0;
pFrame_out->data[1] = 0;
pFrame_out->data[2] = 0;
pFrame_out->linesize[0] = 0;
pFrame_out->linesize[1] = 0;
pFrame_out->linesize[2] = 0;*/
    unsigned nrows = pFrame_out->height = h;

    /*cout << "Avant " << " pFrame_out->linesize " << pFrame_out->linesize[0]  << " w " <<  w << " h " << h <<  std::endl;
    int ret = av_image_alloc(pFrame_out->data, pFrame_out->linesize, w, h, PIX_FMT_RGB24, 32);
    cout << "av_image_alloc ret " << " " << ret << " pFrame_out->linesize " << pFrame_out->linesize[0]  << " w " <<  w << " h " << h <<  std::endl;*/
    av_image_alloc(pFrame_out->data, pFrame_out->linesize, w, h, PIX_FMT_RGB24, 32);
    uint8_t *frame_ptr = pFrame_out->data[0];

    //memset(pFrame_inYUV->data, 0, ret);


    value::rgb8 *ima_ptr = &image_out(image_out.domain().pmin());
    unsigned ima_nextline = image_out.delta_offset(dpoint2d(+1, 0));

    for (unsigned int row = 0; row < nrows; ++row) {
        std::memcpy(frame_ptr, ima_ptr, w * 3);
        ima_ptr += ima_nextline;
        frame_ptr += pFrame_out->linesize[0];
    }

    return pFrame_out;
}

void copy_image2d_rgb_to_AvFame_without_alloc(AVFrame **pFrame_out, image2d<value::rgb8> image_out) {

    int w = image_out.ncols();
    int h = image_out.nrows();
    //
    //(*pFrame_out)->linesize[0] = w * 3;
    (*pFrame_out)->width = w;
    unsigned nrows = (*pFrame_out)->height = h;
    (*pFrame_out)->format = PIX_FMT_RGB24;
    av_image_alloc((*pFrame_out)->data, (*pFrame_out)->linesize, w, h, PIX_FMT_RGB24, 32);

    uint8_t *frame_ptr = (*pFrame_out)->data[0];

    value::rgb8 *ima_ptr = &image_out(image_out.domain().pmin());
    unsigned ima_nextline = image_out.delta_offset(dpoint2d(+1, 0));

    for (unsigned int row = 0; row < nrows; ++row) {
        std::memcpy(frame_ptr, ima_ptr, w * 3);
        ima_ptr += ima_nextline;
        frame_ptr += (*pFrame_out)->linesize[0];
    }
}


AVFrame * copy_image2d_bool_to_AvFame(image2d<bool> image_out) {

    int w = image_out.ncols();
    int h = image_out.nrows();

    AVFrame *pFrame_out;
    pFrame_out = avcodec_alloc_frame();

    //pFrame_out->linesize[0] = w * 3;
    pFrame_out->width = w;
    pFrame_out->height = h;
    pFrame_out->format = PIX_FMT_RGB24;
    av_image_alloc(pFrame_out->data, pFrame_out->linesize, w, h, PIX_FMT_RGB24, 32);

    uint8_t *frame_ptr = pFrame_out->data[0];

    mln_fwd_piter_(image2d<bool>) p(image_out.domain());

    for_all(p) {
        if (image_out(p)) {
            *frame_ptr = 255;
            frame_ptr++;
            *frame_ptr = 255;
            frame_ptr++;
            *frame_ptr = 255;
            frame_ptr++;
        } else {
            *frame_ptr = 0;
            frame_ptr++;
            *frame_ptr = 0;
            frame_ptr++;
            *frame_ptr = 0;
            frame_ptr++;
        }
    }

    return pFrame_out;
}

image2d<value::rgb8> copy_AvFrame_to_image2d_rgb(AVFrame * pFrame_in) {
    image2d<value::rgb8> image_in(pFrame_in->height, pFrame_in->width);

    value::rgb8 *ima_ptr = &image_in(image_in.domain().pmin());
    unsigned ima_nextline = image_in.delta_offset(dpoint2d(+1, 0));
    unsigned nrows = geom::nrows(image_in);
    uint8_t* frame_ptr = pFrame_in->data[0];

    for (unsigned int row = 0; row < nrows; ++row) {
        std::memcpy(ima_ptr, frame_ptr, pFrame_in->width * 3);
        ima_ptr += ima_nextline;
        frame_ptr += pFrame_in->linesize[0];
    }

    return image_in;
}

image2d<value::int_u8> copy_AvFrame_to_image2d_intu(AVFrame * pFrame_in) {

    image2d<value::int_u8> image_in(pFrame_in->height, pFrame_in->width);
    value::int_u8 *ima_ptr = &image_in(image_in.domain().pmin());

    unsigned ima_nextline = image_in.delta_offset(dpoint2d(+1, 0));
    unsigned nrows = geom::nrows(image_in);
    uint8_t* frame_ptr = pFrame_in->data[0];

    for (unsigned int row = 0; row < nrows; ++row) {
        std::memcpy(ima_ptr, frame_ptr, pFrame_in->width);
        ima_ptr += ima_nextline;
        frame_ptr += pFrame_in->linesize[0];
    }

    return image_in;

}

#endif

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {

    FILE *pFile;
    char szFilename[32];
    int y;

    // Open file
    sprintf(szFilename, "%d.ppm", iFrame);
    pFile = fopen(szFilename, "wb");
    if (pFile == NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    fflush(pFile);
    // Write pixel data
    for (y = 0; y < height; y++)
        fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);

    // Close file
    fclose(pFile);
}

AVFormatContext* get_format_context_enc(const char* filename) {

    AVFormatContext *result;

    AVOutputFormat *fmt = av_guess_format(NULL, filename, NULL);
    if (!fmt) {
      std::cout << filename << std::endl;
      printf("Could not deduce output format from file extension: using MPEG.\n");
      fmt = av_guess_format("mpeg", NULL, NULL);
    }
    if (!fmt) {
        fprintf(stderr, "Could not find suitable output format\n");
        exit(1);
    }

    result = avformat_alloc_context();
    if (!result) {
        printf("  bad allocation of AVFormatContext\n ");
        exit(1);
    }

    fmt->audio_codec = CODEC_ID_NONE;
    result->oformat = fmt;

    snprintf(result->filename, sizeof (result->filename), "%s", filename);

    return result;
}

AVCodecContext* get_codec_context_enc(AVCodecContext* dec, AVFormatContext* format_context_enc, AVFormatContext* format_context_dec) {

    AVCodec *encoder = 0;
    AVStream *st = avformat_new_stream(format_context_enc, encoder);

    if (!st) {
        fprintf(stderr, "pb dans avstream");
        exit(1);
    }

    int videoStream = 0;
    unsigned int i;

    for (i = 0; i < format_context_dec->nb_streams; i++)
        if (format_context_dec->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }

    // JO
     st->start_time = format_context_dec->streams[i]->start_time;

       st->avg_frame_rate = format_context_dec->streams[videoStream]->avg_frame_rate;
       st->r_frame_rate  = format_context_dec->streams[videoStream]->avg_frame_rate;
       st->sample_aspect_ratio = dec->sample_aspect_ratio;//format_context_dec->streams[videoStream]->sample_aspect_ratio;
       st->time_base = format_context_dec->streams[i]->time_base;
       st->r_frame_rate = format_context_dec->streams[i]->r_frame_rate;
       st->duration = format_context_dec->streams[i]->duration;
       st->start_time = format_context_dec->streams[i]->start_time;
       //st->avg_frame_rate = dec->avg_frame_rate;
       //	st->sample_aspect_ratio = dec->sample_aspect_ratio;


     //cout << "  st->sample_aspect_ratio" << st->sample_aspect_ratio.den << endl;
    // cout << "  st->sample_aspect_ratio" << st->sample_aspect_ratio.num << endl;

       //       cout << "videoStream" << videoStream << endl;
       //cout << "  st->sample_aspect_ratio" << st->sample_aspect_ratio.den << endl;
       //cout << "  st->sample_aspect_ratio" << st->sample_aspect_ratio.num << endl;
       //cout << "  st->avg_frame_rate" << st->avg_frame_rate.den << endl;
       //cout << "  st->avg_frame_rate" << st->avg_frame_rate.num << endl;
       //cout << "  st->r_frame_rate" << st->r_frame_rate.den << endl;
       //cout << "  st->r_frame_rate" << st->r_frame_rate.num << endl;
       //cout << "  st->time_base" << st->time_base.num << endl;
       //cout << "  st->time_base" << st->time_base.den << endl;
       //cout << "  st->duration" << st->duration << endl;


    AVCodecContext* enc = st->codec;
    // cout << "Ctx_Enc->codec_id " << enc->codec_id << " CODEC_ID_H264 " << CODEC_ID_H264 << endl;

    // parameters for encoding
    //cout << " dec->bit_rate = " << dec->bit_rate << endl;
    //enc->bit_rate = dec->bit_rate; //2000000; // 400000; //find good tradeoff between compression and video quality: bit/s
    //cout << " decode biterate" << dec->bit_rate << endl; //gives always 0, i don t know why

    enc->bit_rate = format_context_dec->bit_rate;
    //cout << " decode biterate" << format_context_dec->bit_rate << endl;     //Gives the correct value


    enc->width = dec->width;
    enc->height = dec->height;
    //Ctx_Enc->time_base = (AVRational){1, 25};
    enc->gop_size = 10; // emit one intra frame every ten frames
    enc->max_b_frames = 1;
    enc->pix_fmt = PIX_FMT_YUV420P;
    enc->codec_type = AVMEDIA_TYPE_VIDEO;
    enc->codec_id = format_context_enc->oformat->video_codec;
    //Ctx_Enc->time_base.den = Ctx_Dec->time_base.den;
    //Ctx_Enc->time_base.num = Ctx_Dec->time_base.num;
    //cout << " fps" <<  dec->time_base.den << " " << dec->time_base.num << " " <<  dec->ticks_per_frame << endl;

    //if (enc->codec_id == AV_CODEC_ID_H264)
    //  av_opt_set(enc->priv_data, "preset", "slow", 0);

    //enc->time_base.den = 25; // do not force a special frame rate
    //enc->time_base.num = 1;
    //enc->ticks_per_frame = dec->ticks_per_frame;
    //enc->ticks_per_frame = dec->ticks_per_frame;
    //enc->time_base.den = dec->time_base.den;
    double fps;
    if ( format_context_dec->streams[videoStream]->avg_frame_rate.num <1 && format_context_dec->streams[videoStream]->avg_frame_rate.den <1 ) {
       fps = dec->ticks_per_frame*dec->time_base.den/(float)dec->time_base.num;
       st->avg_frame_rate.num = 1;
       st->avg_frame_rate.den = fps;
    } else {
      fps = format_context_dec->streams[videoStream]->avg_frame_rate.num / format_context_dec->streams[videoStream]->avg_frame_rate.den;
    }
    //cout << "FPS " << format_context_dec->streams[videoStream]->avg_frame_rate.num << " " << format_context_dec->streams[videoStream]->avg_frame_rate.den << endl;
    //cout << "FPS bis " << dec->ticks_per_frame << " " << dec->time_base.num << " "  << dec->time_base.den << endl;
    //cout << "Flux " << videoStream << endl;
    //cout << "fps " << fps << endl;
    //double fps = format_context_dec->streams[videoStream]->avg_frame_rate.num / format_context_dec->streams[videoStream]->avg_frame_rate.den;

    //enc->time_base.den = dec->time_base.den/dec->ticks_per_frame; // do not force a special frame rate
    //enc->time_base.den = (fps/dec->ticks_per_frame); // do not force a special frame rate
    enc->time_base.num = 1;
    enc->time_base.den = fps;
    //enc->time_base.num = dec->time_base.num;
    enc->ticks_per_frame = 1;//dec->ticks_per_frame;

    //enc->ticks_per_frame = dec->ticks_per_frame;


    enc->sample_rate = dec->sample_rate;
    //cout <<  "sample_rate"  << enc->sample_rate << endl;
    enc->sample_aspect_ratio = dec->sample_aspect_ratio;

    //cout <<  "AR dec"  << dec->sample_aspect_ratio.den << " " << dec->sample_aspect_ratio.num << endl;
    //cout <<  "AR enc "  << enc->sample_aspect_ratio.den << " " << enc->sample_aspect_ratio.num << endl;
    // cout << " Ctx_Enc->ticks_per_frame " << enc->ticks_per_frame << endl;

    //    cout <<  "AR dec"  << dec->sample_aspect_ratio.den << " " << dec->sample_aspect_ratio.num << endl;
    //    cout <<  "AR enc "  << enc->sample_aspect_ratio.den << " " << enc->sample_aspect_ratio.num << endl;

    //    cout <<  "AR enc time"  <<enc->time_base.den  << " " << enc->time_base.num << endl;
    //    cout << " enc->ticks_per_frame " << enc->ticks_per_frame << endl;

    //Ctx_Enc->me_method=2; motion compensation?
    //  cout << " decode den" << dec->time_base.den << endl;
    //  cout << "decode num" << dec->time_base.num << endl;

    //    To avoid
    //        [libx264 @ 0x1328b60] broken ffmpeg default settings detected
    //        [libx264 @ 0x1328b60] use an encoding preset (e.g. -vpre medium)
    //        [libx264 @ 0x1328b60] preset usage: -vpre <speed> -vpre <profile>
    //        [libx264 @ 0x1328b60] speed presets are listed in x264 --help
    //        [libx264 @ 0x1328b60] profile is optional; x264 defaults to high
    //                      @see http://home.51.com/xiaoyi247173/diary/item/10048495.html

    enc->me_method = 5;
    enc->me_range = 16;
    enc->max_qdiff = 4;
    // Qmin stands for the setting to be used for achieving lowest quality and Qmax for highest. These are not just lowest and highest values.
    enc->qmin = 10;
    enc->qmax = 51;
    enc->qcompress = 0.6;

    enc->profile = dec->profile;
    enc->level = 12; //dec->level; // FIXME: hand made
    //cout <<  "profile"  << enc->profile << endl;
    //cout <<  "level"  << enc->level << endl;

    if (st) {
        // searching for encoder of video stream
        encoder = avcodec_find_encoder(enc->codec_id);
        if (encoder == NULL) {
            fprintf(stderr, "Unsupported codec ENCODER!\n");
            exit(1); // Codec not found
        }
        /* open the codec */
        if (avcodec_open2(enc, encoder, NULL) < 0) {
            fprintf(stderr, "could not open codec\n");
            exit(1);
        }
    }

    // some formats want stream headers to be separate
    if (format_context_enc->oformat->flags & AVFMT_GLOBALHEADER)
        enc->flags |= CODEC_FLAG_GLOBAL_HEADER;

    //    format_context_enc->sample_aspect_ratio = format_context_dec->sample_aspect_ratio;
    //format_context_enc->sample_aspect_ratio = format_context_dec->sample_aspect_ratio;
    //format_context_enc->avg_frame_rate = format_context_dec->avg_frame_rate;

    return enc;
}

void init_stream(AVFormatContext* pFormatCtx_Enc) {

    if (!(pFormatCtx_Enc->oformat->flags & AVFMT_NOFILE)) {
        int m;
        if ((m = avio_open(&pFormatCtx_Enc->pb, pFormatCtx_Enc->filename, AVIO_FLAG_WRITE)) < 0) {
            char errbuf[50000];
            fprintf(stderr, "Could not open '%s'\n", pFormatCtx_Enc->filename);
            av_strerror(m, errbuf, 50000);
            cerr << errbuf << endl;
            exit(1);
        }
    }

    //  cout << "pFormatCtx_Enc->pb " << pFormatCtx_Enc->pb << endl;
    // cout << "avant write header" << endl;

    int m = avformat_write_header(pFormatCtx_Enc, NULL);

    if (m != 0) {
        char errbuf[50000];
        av_strerror(m, errbuf, 50000);
        cout << "pb ds write header" << endl << errbuf << endl;
    }

}

void allocate_frames(AVCodecContext* Ctx, AVFrame **pFrame_YUV, AVFrame **pFrame_RGB) {

  // Allocate video frame in and out
  if (pFrame_YUV) {
    *pFrame_YUV = avcodec_alloc_frame();
    av_image_alloc((*pFrame_YUV)->data, (*pFrame_YUV)->linesize, Ctx->width, Ctx->height, Ctx->pix_fmt, 32);
    (*pFrame_YUV)->format = Ctx->height;
    (*pFrame_YUV)->width = Ctx->width;
    (*pFrame_YUV)->height = Ctx->height;
  }

  if (pFrame_RGB) {
    *pFrame_RGB = avcodec_alloc_frame();
    av_image_alloc((*pFrame_RGB)->data, (*pFrame_RGB)->linesize, Ctx->width, Ctx->height, PIX_FMT_RGB24, 32);

    (*pFrame_RGB)->width = Ctx->width;
    (*pFrame_RGB)->height = Ctx->height;
    (*pFrame_RGB)->format = PIX_FMT_RGB24;
  }
}

AVFormatContext* get_format_context_dec(const char* filename) {

    AVFormatContext *pFormatCtx_Dec =  avformat_alloc_context(); // buffer containing pointers to all the file stream
    if (avformat_open_input(&pFormatCtx_Dec, filename, NULL, NULL) != 0) // Open video file
    {
        printf("Couldn't open file \n ");
        exit(1);
    }


    if (avformat_find_stream_info(pFormatCtx_Dec, NULL) < 0) // Retrieve stream information
    {
        printf("Couldn't find stream information \n ");
        exit(1);
    }

    return pFormatCtx_Dec;
}

AVCodecContext* get_codec_context_dec(AVFormatContext *pFormatCtx_Dec, int& videoStream) {

    AVCodecContext* Ctx_Dec;
    AVCodec *decoder = 0;

    unsigned int i;

    for (i = 0; i < pFormatCtx_Dec->nb_streams; i++)
        if (pFormatCtx_Dec->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }

    if (videoStream == -1) {
        printf("Didn't find a video stream\n");
        exit(1);
    }

    // if there is a video stream we get the codec

    /******************   FIXME   ************************/
    // FUCK IT => Pas foutu de mettre le bon codec !!
    Ctx_Dec = pFormatCtx_Dec->streams[videoStream]->codec;
    decoder = avcodec_find_decoder(Ctx_Dec->codec_id);

    // searching for decoder of video stream
    // decoder = avcodec_find_decoder(AV_CODEC_ID_MPEG4);

    if (decoder == NULL) {
        fprintf(stderr, "Unsupported codec DECODER!\n");
        exit(1); // Codec not found
    }

    // auto stream =  pFormatCtx_Dec->streams[videoStream];
    // Ctx_Dec = avcodec_alloc_context3(decoder);
    // Ctx_Dec->extradata = stream->codec->extradata;
    // Ctx_Dec->extradata_size = stream->codec->extradata_size;

    av_dump_format(pFormatCtx_Dec, videoStream, "video", 0);

    // Open contexte decode
    if (avcodec_open2(Ctx_Dec, decoder, NULL) < 0) {
        printf("Could not open codec decode \n");
        exit(1);
    }

    return Ctx_Dec;
}

void encode_video(int& skipped_bin, AVFormatContext** pFormatCtx_Bin, const char* filename, AVFrame** pFrame_binRGB, AVFrame** pFrame_binYUV, int i, AVCodecContext** Ctx_Bin, AVPacket& packet_bin, struct SwsContext** convert_ctx) {

    int ret, got_packet;

    if (filename) {

      //cout << "1 " << (*pFrame_binRGB)->linesize[0] << " " << (*pFrame_binRGB)->height << " " << (*pFrame_binRGB)->width << " " <<
      //                    (*pFrame_binYUV)->linesize[0] << " " << (*pFrame_binYUV)->height << " " << (*pFrame_binYUV)->width << std::endl;

        sws_scale(*convert_ctx, (const uint8_t * const*) (*pFrame_binRGB)->data,
                (*pFrame_binRGB)->linesize, 0, (*pFrame_binRGB)->height, (*pFrame_binYUV)->data, (*pFrame_binYUV)->linesize);

        av_init_packet(&packet_bin);

        (*pFrame_binYUV)->pts = i; //- 1;
        ret = avcodec_encode_video2(*Ctx_Bin, &packet_bin, *pFrame_binYUV, &got_packet);
        cout << "  got packet BINA  " << got_packet << " pts " << packet_bin.pts << "dts " << packet_bin.dts << "st index" << packet_bin.stream_index << endl;

        if (ret < 0) {
            fprintf(stderr, "Video encoding failed BINA.\n");
            exit(1);
        }

        if (ret == 0) {
            if (got_packet == 1) {
              //cout << " >> PTS" << packet_bin.pts << " DTS " << packet_bin.dts << " duration " << packet_bin.duration << endl;
              //cout << " (*Ctx_Bin)->time_base " <<(*Ctx_Bin)->time_base.num << " " <<  (*Ctx_Bin)->time_base.den << endl;
              //cout << " (*pFormatCtx_Bin)->streams[0]->time_base " <<(*pFormatCtx_Bin)->streams[0]->time_base.num << " " <<  (*pFormatCtx_Bin)->streams[0]->time_base.den << endl;

                packet_bin.stream_index = (*pFormatCtx_Bin)->streams[0]->index;
                if (packet_bin.pts != AV_NOPTS_VALUE && packet_bin.pts>0) {
                    packet_bin.pts = av_rescale_q(packet_bin.pts, (*Ctx_Bin)->time_base, (*pFormatCtx_Bin)->streams[0]->time_base);
                    //packet_bin.pts = packet_bin.pts*(12800/25.0);
                    //packet_bin.duration = 12800/25.0;
                }
                if (packet_bin.dts != AV_NOPTS_VALUE && packet_bin.dts>0) {
                    packet_bin.dts = av_rescale_q(packet_bin.dts, (*Ctx_Bin)->time_base, (*pFormatCtx_Bin)->streams[0]->time_base);
                    //packet_bin.dts = packet_bin.dts*(12800/25.0);
                }
                if (packet_bin.duration > 0) {
                  packet_bin.duration = av_rescale_q(packet_bin.duration, (*Ctx_Bin)->time_base, (*pFormatCtx_Bin)->streams[0]->time_base);
                  //packet_bin.duration = 12800/25.0;
                }
                //cout << " << PTS" << packet_bin.pts << " DTS " << packet_bin.dts << " duration " << packet_bin.duration << endl;
                int ret2 = av_write_frame(*pFormatCtx_Bin, &packet_bin);

                //printf(" got packet after frame %3d (size=%5d) BINA\n", i, packet_bin.size);

                if (ret2 < 0) {
                    char errbuf[50000];
                    fprintf(stderr, "Video encoding failed write frame BINA .\n");
                    av_strerror(ret2, errbuf, 50000);
                    cerr << errbuf << endl;
                    exit(1);
                }

                //fwrite(packet_bin.data, 1, spacket.size, f);
                av_destruct_packet(&packet_bin);
            } else skipped_bin++;
        }
    }
}

void encode_skipped_frames(int nb_skipped, int i, AVFormatContext** pFormatCtx, const char* filename, AVFrame** pFrame_outYUV,
        AVCodecContext** Ctx, AVPacket& packet) {

    int ret, got_packet;

    //  cout << "nb skip in encode ski" << nb_skipped << endl;

    if (filename) //on encode
    {
        for (; nb_skipped >= 0; nb_skipped--) {
            // cout << " ----------------------------------------------------------------------------SKIPPED FRAME: " << nb_skipped << endl;
            i++;
            (*pFrame_outYUV)->pts = i; //- 1;

            ret = avcodec_encode_video2(*Ctx, &packet, *pFrame_outYUV, &got_packet);
            //printf("SKIPPED %d !! ret et gopacket %d %d \n", ret, got_packet, skipped);

            if (ret < 0) {
                fprintf(stderr, "Video encoding failed.\n");
                exit(1);
            }

            if (ret == 0) {
                if (got_packet == 1) {

              packet.stream_index = (*pFormatCtx)->streams[0]->index;
                if (packet.pts != AV_NOPTS_VALUE && packet.pts>0) {
                    packet.pts = av_rescale_q(packet.pts, (*Ctx)->time_base, (*pFormatCtx)->streams[0]->time_base);
                    //packet_bin.pts = packet_bin.pts*(12800/25.0);
                    //packet_bin.duration = 12800/25.0;
                }
                if (packet.dts != AV_NOPTS_VALUE && packet.dts>0) {
                    packet.dts = av_rescale_q(packet.dts, (*Ctx)->time_base, (*pFormatCtx)->streams[0]->time_base);
                    //packet_bin.dts = packet_bin.dts*(12800/25.0);
                }
                if (packet.duration > 0) {
                  packet.duration = av_rescale_q(packet.duration, (*Ctx)->time_base, (*pFormatCtx)->streams[0]->time_base);
                  //packet_bin.duration = 12800/25.0;
                }
                    int ret2 = av_write_frame(*pFormatCtx, &packet);

                    if (ret2 < 0) {
                        char errbuf[50000];
                        fprintf(stderr, "Video encoding failed write frame .\n");
                        av_strerror(ret2, errbuf, 50000);
                        cerr << errbuf << endl;
                        exit(1);
                    }

                    av_destruct_packet(&packet);
                } else nb_skipped++;
            }
        }
    }
}

void close_free(AVFormatContext** pFormatCtx_Enc, AVFormatContext** pFormatCtx_Dec, AVFormatContext** pFormatCtx_Bin,
        AVCodecContext** Ctx_Dec, AVCodecContext** Ctx_Enc, AVCodecContext** Ctx_Bin, AVFrame** pFrame_inYUV, AVFrame** pFrame_inRGB,
        AVFrame** pFrame_outYUV, AVFrame** pFrame_binYUV) {

    unsigned int i;
    av_write_trailer(*pFormatCtx_Enc);
    av_write_trailer(*pFormatCtx_Bin);

    //close codec
    avcodec_close(*Ctx_Dec);
    avcodec_close(*Ctx_Enc);
    avcodec_close(*Ctx_Bin);

    //close media file+ close stream
    avformat_close_input(pFormatCtx_Dec);

    //free AVFrame
    av_free(*pFrame_inYUV);
    av_free(*pFrame_inRGB);
    av_free(*pFrame_outYUV);
    av_free(*pFrame_binYUV);


    // free the streams
    for (i = 0; i < (*pFormatCtx_Enc)->nb_streams; i++) {
        av_freep(&((*pFormatCtx_Enc)->streams[i]->codec));
        av_freep(&((*pFormatCtx_Enc)->streams[i]));
    }

    // free the streams
    for (i = 0; i < (*pFormatCtx_Bin)->nb_streams; i++) {
        av_freep(&((*pFormatCtx_Bin)->streams[i]->codec));
        av_freep(&((*pFormatCtx_Bin)->streams[i]));
    }

    if (!((*pFormatCtx_Enc)->oformat->flags & AVFMT_NOFILE)) {
        avio_close((*pFormatCtx_Enc)->pb); // close the output file
    }

    if (!((*pFormatCtx_Bin)->oformat->flags & AVFMT_NOFILE)) {
        avio_close((*pFormatCtx_Bin)->pb); // close the output file
    }

    // free the stream
    av_free(*pFormatCtx_Enc);
    av_free(*pFormatCtx_Bin);

}

void close_free_decode(AVFormatContext** pFormatCtx_Dec, AVCodecContext** Ctx_Dec, AVFrame** pFrame_inYUV, AVFrame** pFrame_inRGB) {

    //close codec
    avcodec_close(*Ctx_Dec);

    //close media file+ close stream
    avformat_close_input(pFormatCtx_Dec);

    //free AVFrame
    av_free(*pFrame_inYUV);
    av_free(*pFrame_inRGB);

}

void close_free_encode(AVFormatContext** pFormatCtx_Enc, AVCodecContext** Ctx_Enc, AVFrame** pFrame_outYUV, AVFrame** pFrame_outRGB) {

    // unsigned int i;
    av_write_trailer(*pFormatCtx_Enc);

    //close codec
    avcodec_close(*Ctx_Enc);

    //close media file+ close stream
    avformat_close_input(pFormatCtx_Enc);

    //free AVFrame
    av_free(*pFrame_outYUV);
    av_free(*pFrame_outRGB); //NEED?

    // free the streams
    //    cout << " nb streams = " << (*pFormatCtx_Enc)->nb_streams << endl;
    //
    //    for (i = 0; i < (*pFormatCtx_Enc)->nb_streams; i++) {
    //        av_freep(&((*pFormatCtx_Enc)->streams[i]->codec));
    //        av_freep(&((*pFormatCtx_Enc)->streams[i]));
    //    }

    //    if (!((*pFormatCtx_Enc)->oformat->flags & AVFMT_NOFILE)) {
    //        avio_close((*pFormatCtx_Enc)->pb); // close the output file
    //    }

    // free the stream
    av_free(*pFormatCtx_Enc);
}
