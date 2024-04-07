/**
 * @file test_ffmpeg.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "protocol.hpp"

void InitGlog(const char *program_path)
{
    google::InitGoogleLogging(program_path);
    
    FLAGS_log_dir = "/data/home/user/workspace/cpp_unit/data/log/";
    FLAGS_log_year_in_prefix = false;
    FLAGS_log_utc_time = false;
    FLAGS_log_link = false;
    FLAGS_log_prefix = true;
    FLAGS_logbufsecs = 0;
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    FLAGS_max_log_size = 10;  // 1MB
    FLAGS_minloglevel = google::GLOG_INFO;
    FLAGS_stop_logging_if_full_disk = true;
    FLAGS_log_file_header = false;
}

int ffmpeg_record_video(Protocol::Message& message)
{
    avformat_network_init();

    AVFormatContext* p_format_ctx = avformat_alloc_context();
    AVDictionary* options{nullptr};
    av_dict_set(&options, "rtsp_transport", "tcp", 0);

    int result;
    std::string rtsp_url{"rtsp://admin:abcd1234@192.169.8.153"};
    std::string output_file{"/tmp/output.mp4"};
    int duration{60};

    result = avformat_open_input(&p_format_ctx, rtsp_url.c_str(), nullptr, &options);
    if (result != 0)
    {
        LOG(ERROR) << "open rtsp failed\n";
        return result;
    }

    result = avformat_find_stream_info(p_format_ctx, nullptr);
    if (result < 0)
    {
        LOG(ERROR) << "get stream information failed\n";
        return result;
    }

    AVFormatContext* p_output_format_ctx;
    result = avformat_alloc_output_context2(&p_output_format_ctx, nullptr, nullptr, output_file.c_str());
    if (result < 0)
    {
        LOG(ERROR) << "config output format failed\n";
        return result;
    }

    AVStream* p_stream = avformat_new_stream(p_output_format_ctx, nullptr);
    if (!p_stream)
    {
        LOG(ERROR) << "create output stream failed\n";
        return result;
    }

    result = avcodec_parameters_copy(p_stream->codecpar, p_format_ctx->streams[0]->codecpar);
    if (result < 0)
    {
        LOG(ERROR) << "copy codec argument failed\n";
        return result;
    }

    if (!(p_output_format_ctx->oformat->flags & AVFMT_NOFILE))
    {
        result = avio_open(&p_output_format_ctx->pb, output_file.c_str(), AVIO_FLAG_WRITE);
        if (result < 0)
        {
            LOG(ERROR) << "open output file failed\n";
            return result;
        }
    }

    result = avformat_write_header(p_output_format_ctx, nullptr);

    AVPacket packet;
    int64_t start_time = av_gettime();
    int64_t end_time = start_time + duration * 1000'000;

    while (av_read_frame(p_format_ctx, &packet) >= 0)
    {
        if (av_gettime() >= end_time)
        {
            break;
        }

        av_packet_rescale_ts(&packet, p_format_ctx->streams[0]->time_base, p_stream->time_base);
        packet.stream_index = p_stream->index;

        av_interleaved_write_frame(p_output_format_ctx, &packet);
        av_packet_unref(&packet);
    }

    av_write_trailer(p_output_format_ctx);

    avformat_close_input(&p_format_ctx);
    avio_closep(&p_output_format_ctx->pb);
    avformat_free_context(p_output_format_ctx);

    av_dict_free(&options);

    return 0;
}

int ffmpeg_rtsp_to_image(Protocol::Message& message)
{
    LOG(INFO) << "ffmpeg basic using function\n";
    avformat_network_init();
    int res{-1};
    int decode_result{-1};
    int video_stream_index{-1};
    AVCodecParameters* codec_params_ptr{nullptr};
    AVCodec* codec_ptr{nullptr};
    AVPacket packet;
    cv::Mat image;
    int cv_linesize[1];
    int count{0};
    std::string save_path;
    AVCodecContext* codec_context_ptr{nullptr};
    AVFrame* frame_ptr = av_frame_alloc();
    AVFrame* frame_rgb_ptr{nullptr};
    AVFormatContext* format_context_ptr = avformat_alloc_context();
    AVDictionary* options{nullptr};
    SwsContext* sws_context_ptr{nullptr};
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    const char* rtsp_url = "rtsp://admin:abcd1234@192.169.8.153";

    res = avformat_open_input(&format_context_ptr, rtsp_url, nullptr, &options);
    if (res != 0)
    {
        LOG(ERROR) << "open rtsp failed\n";
        goto __RETURN_DATA;        
    }

    res = avformat_find_stream_info(format_context_ptr, nullptr);
    if (res != 0)
    {
        LOG(ERROR) << "find stream information failed\n";
        goto __RETURN_DATA;        
    }

    for (int i = 0; i < format_context_ptr->nb_streams; i++)
    {
        if (format_context_ptr->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1)
    {
        LOG(ERROR) << "find video stream index failed\n";
        goto __RETURN_DATA;
    }
    codec_params_ptr = format_context_ptr->streams[video_stream_index]->codecpar;
    codec_ptr = const_cast<AVCodec *>(avcodec_find_decoder(codec_params_ptr->codec_id));
    if (codec_ptr == nullptr)
    {
        LOG(ERROR) << "find video decoder failed\n";
        goto __RETURN_DATA;
    }

    codec_context_ptr = avcodec_alloc_context3(codec_ptr);
    avcodec_parameters_to_context(codec_context_ptr, codec_params_ptr);
    res = avcodec_open2(codec_context_ptr, nullptr, nullptr);
    if (res < 0)
    {
        LOG(ERROR) << "open video decoder context failed\n";
        goto __RETURN_DATA;
    }

    sws_context_ptr = sws_getContext(codec_context_ptr->width, codec_context_ptr->height, codec_context_ptr->pix_fmt, codec_context_ptr->width, codec_context_ptr->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

    while (true)
    {
        res = av_read_frame(format_context_ptr, &packet);
        if (res < 0)
        {
            break;
        }

        if (packet.stream_index != video_stream_index)
        {
            LOG(WARNING) << "this is not video data\n";
            continue;
        }

        avcodec_send_packet(codec_context_ptr, &packet);
        decode_result = avcodec_receive_frame(codec_context_ptr, frame_ptr);
        if (decode_result < 0)
        {
            LOG(WARNING) << "decode failed\n";
            continue;
        }
        image = cv::Mat(frame_ptr->height, frame_ptr->width, CV_8UC3);
        cv_linesize[0] = image.step1();
        sws_scale(sws_context_ptr, frame_ptr->data, frame_ptr->linesize, 0, frame_ptr->height, &image.data, cv_linesize);
        cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
        cv::imshow("test", image);
        cv::waitKey(10);
        save_path = "/tmp/image-";
        save_path += std::to_string(count);
        save_path += ".jpg";
        cv::imwrite(save_path, image);
        count++;

        av_packet_unref(&packet);
    }

__RETURN_DATA:
    avcodec_close(codec_context_ptr);
    av_frame_free(&frame_ptr);
    avcodec_free_context(&codec_context_ptr);
    avformat_close_input(&format_context_ptr);
    avformat_free_context(format_context_ptr);
    avformat_network_deinit();

    return 0;
}

DEFINE_string(module, "design", "module layer");
DEFINE_int64(id, 123, "module layer");

int main(int argc, char* argv[])
{
    InitGlog(argv[0]);
    gflags::SetUsageMessage("cpp unit project!");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    LOG(INFO) << "--cpp unit--" << std::endl;
    LOG(INFO) << gflags::ProgramUsage() << std::endl;

    Protocol::Message message;

    std::map<std::string, std::function<int(Protocol::Message&)>> func_table =
    {
        {"ffmpeg_record_video", ffmpeg_record_video},
        {"ffmpeg_rtsp_to_image", ffmpeg_rtsp_to_image}
    };

    auto it = func_table.find(FLAGS_module);
    if (it != func_table.end())
    {
        it->second(message);
    }
    else 
    {
        LOG(ERROR) << "invalid argument: " << FLAGS_module << "\n";
    }

    google::ShutDownCommandLineFlags();
    google::ShutdownGoogleLogging();
    return 0;
}