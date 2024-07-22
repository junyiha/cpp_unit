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
#include "MediaPlayer.hpp"
#include "dlib/include/dlib/image_processing/frontal_face_detector.h"
#include "dlib/include/dlib/gui_widgets.h"
#include "dlib/include/dlib/image_io.h"

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

int ffmpeg_images_to_video(Protocol::Message& message)
{
    std::string image_dir{"/tmp"};
    std::vector<std::string> image_path_container;

    try 
    {
        if (boost::filesystem::exists(image_dir) && boost::filesystem::is_directory(image_dir))
        {
            for (const auto& entry : boost::filesystem::directory_iterator(image_dir))
            {
                if (boost::filesystem::is_regular_file(entry.status()))
                {
                    image_path_container.push_back(entry.path().string());
                }
            }
        }
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG(ERROR) << e.what() << "\n";
    }

    // std::sort(image_path_container.begin(), image_path_container.end(), [](std::string a, std::string b){
    //     auto a_pos_1 = a.find_first_of('-');
    //     auto a_pos_2 = a.find_first_of('.');
    //     auto a_index = std::stoi(a.substr(a_pos_1 + 1, a_pos_2 - a_pos_1 - 1));

    //     auto b_pos_1 = b.find_first_of('-');
    //     auto b_pos_2 = b.find_first_of('.');
    //     auto b_index = std::stoi(b.substr(b_pos_1 + 1, b_pos_2 - b_pos_1 - 1));

    //     return a_index < b_index;
    // });

    for (auto& it : image_path_container)
    {
        LOG(INFO) << "file: " << it << "\n";
    }

    int res{-1};
    int width = 640;
    int height = 360;
    int bit_rate = 4000;
    int frame_rate = 20;
    std::string output_filename{"/tmp/aaa.mp4"};
    int frame_count{0};
    AVPacket* packet_ptr;
    AVCodec* codec_ptr{nullptr};
    AVCodecContext* codec_context_ptr{nullptr};
    AVStream* stream_ptr{nullptr};
    AVFrame* frame_ptr{nullptr};
    AVFrame* av_frame_ptr{nullptr};
    SwsContext* sws_context_ptr{nullptr};
    AVFormatContext* format_context_ptr{nullptr};

    avformat_alloc_output_context2(&format_context_ptr, nullptr, nullptr, output_filename.c_str());
    if (!format_context_ptr)
    {
        LOG(ERROR) << "could not allocate output context\n";
        goto __RETURN_DATA;
    }

    codec_ptr = const_cast<AVCodec *>(avcodec_find_encoder(AV_CODEC_ID_H264));
    if (!codec_ptr)
    {
        LOG(ERROR) << "codec not found\n";
        goto __RETURN_DATA;
    }

    stream_ptr = avformat_new_stream(format_context_ptr, codec_ptr);
    if (!stream_ptr)
    {
        LOG(ERROR) << "could not allocate stream\n";
        goto __RETURN_DATA;
    }

    codec_context_ptr = avcodec_alloc_context3(codec_ptr);
    if (!codec_context_ptr)
    {
        LOG(ERROR) << "could not allocate codec context\n";
        goto __RETURN_DATA;
    }

    stream_ptr->codecpar->codec_id = codec_ptr->id;
    stream_ptr->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    stream_ptr->codecpar->width = width;
    stream_ptr->codecpar->height = height;
    stream_ptr->codecpar->format = AV_PIX_FMT_YUV420P;
    stream_ptr->codecpar->bit_rate = bit_rate;

    avcodec_parameters_to_context(codec_context_ptr, stream_ptr->codecpar);
    codec_context_ptr->time_base = (AVRational){1, frame_rate};

    res = avcodec_open2(codec_context_ptr, codec_ptr, nullptr);
    if (res < 0)
    {
        LOG(ERROR) << "could not open codec\n";
        goto __RETURN_DATA;
    }

    if (!(format_context_ptr->oformat->flags & AVFMT_NOFILE))
    {
        res = avio_open(&format_context_ptr->pb, output_filename.c_str(), AVIO_FLAG_WRITE);
        if (res < 0)
        {
            LOG(ERROR) << "could not open output file\n";
            goto __RETURN_DATA;
        }
    }

    res = avformat_write_header(format_context_ptr, nullptr);
    if (res < 0)
    {
        LOG(ERROR) << "could not write header\n";
        goto __RETURN_DATA;
    }

    frame_ptr = av_frame_alloc();
    if (!frame_ptr)
    {
        LOG(ERROR) << "could not allocate video frame\n";
        goto __RETURN_DATA;
    }

    frame_ptr->format = codec_context_ptr->pix_fmt;
    frame_ptr->width = codec_context_ptr->width;
    frame_ptr->height = codec_context_ptr->height;

    res = av_frame_get_buffer(frame_ptr, 32);
    if (res < 0)
    {
        LOG(ERROR) << "could not allocate the video frame data\n";
        goto __RETURN_DATA;
    }

    for (auto& it : image_path_container)
    {
        cv::Mat image = cv::imread(it);
        if (image.empty())
        {
            break;
        }

        cv::resize(image, image, cv::Size(width, height));
        av_frame_ptr = av_frame_alloc();
        av_frame_ptr->width = image.cols;
        av_frame_ptr->height = image.rows;
        av_frame_ptr->format = AV_PIX_FMT_BGR24;

        av_frame_get_buffer(av_frame_ptr, 32);
        av_image_fill_arrays(av_frame_ptr->data, av_frame_ptr->linesize, image.data, AV_PIX_FMT_BGR24, image.cols, image.rows, 1);

        sws_context_ptr = sws_getContext(image.cols, image.rows, AV_PIX_FMT_BGR24, codec_context_ptr->width, codec_context_ptr->height, codec_context_ptr->pix_fmt, SWS_BILINEAR, nullptr, nullptr, nullptr);

        sws_scale(sws_context_ptr, av_frame_ptr->data, av_frame_ptr->linesize, 0, image.rows, frame_ptr->data, frame_ptr->linesize);
        
        av_frame_free(&av_frame_ptr);
        sws_freeContext(sws_context_ptr);

        frame_ptr->pts = frame_count++;
        packet_ptr = av_packet_alloc();
        
        res = avcodec_send_frame(codec_context_ptr, frame_ptr);
        if (res < 0)
        {
            LOG(ERROR) << "encoding frame failed\n";
            goto __RETURN_DATA;
        }

        res = avcodec_receive_packet(codec_context_ptr, packet_ptr);
        if (res < 0)
        {
            LOG(ERROR) << "encoding frame failed\n";
            goto __RETURN_DATA;
        }

        packet_ptr->stream_index = stream_ptr->index;
        av_packet_rescale_ts(packet_ptr, codec_context_ptr->time_base, stream_ptr->time_base);
        packet_ptr->pos = -1;

        av_interleaved_write_frame(format_context_ptr, packet_ptr);
        av_packet_free(&packet_ptr);
    }
    av_write_trailer(format_context_ptr);

__RETURN_DATA:
    avio_close(format_context_ptr->pb);
    avformat_free_context(format_context_ptr);
    av_frame_free(&frame_ptr);

    return 0;
}

int ffmpeg_images_to_video_in_command(Protocol::Message& message)
{
    std::string image_dir{"/data/home/user/workspace/cpp_unit/data/images"};
    std::vector<std::string> image_path_container;

    try 
    {
        if (boost::filesystem::exists(image_dir) && boost::filesystem::is_directory(image_dir))
        {
            for (const auto& entry : boost::filesystem::directory_iterator(image_dir))
            {
                if (boost::filesystem::is_regular_file(entry.status()))
                {
                    image_path_container.push_back(entry.path().string());
                }
            }
        }
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG(ERROR) << e.what() << "\n";
    }

    std::sort(image_path_container.begin(), image_path_container.end(), [](std::string a, std::string b){
        auto a_pos_1 = a.find_first_of('-');
        auto a_pos_2 = a.find_first_of('.');
        auto a_index = std::stoi(a.substr(a_pos_1 + 1, a_pos_2 - a_pos_1 - 1));

        auto b_pos_1 = b.find_first_of('-');
        auto b_pos_2 = b.find_first_of('.');
        auto b_index = std::stoi(b.substr(b_pos_1 + 1, b_pos_2 - b_pos_1 - 1));

        return a_index < b_index;
    });

    std::vector<cv::Mat> img_container;
    for (auto& it : image_path_container)
    {
        LOG(INFO) << "file: " << it << "\n";
        cv::Mat tmp = cv::imread(it);
        if (tmp.empty())
        {
            continue;
        }
        img_container.push_back(tmp);
    }
    char *path;
    char name[] = "/tmp/dirXXXXXX";
    path = mkdtemp(name);
    if (path == NULL)
    {
        perror("mkdtemp");
        return -1;
    }

    LOG(INFO) << path << "\n";
    std::vector<std::string> temporary_file_container;
    for (int i = 0; i < img_container.size(); i++)
    {
        std::string file_name = std::string(path) + "/" + std::to_string(i) + ".jpg";
        cv::imwrite(file_name, img_container.at(i));
        temporary_file_container.push_back(file_name);
    }

    int frame_rate{2};
    std::stringstream os_command;
    os_command << ". /data/home/user/workspace/cpp_unit/build/source.sh && /data/usr/local/ffmpeg/bin/ffmpeg -framerate " << frame_rate << " -i " << path << "/%d.jpg " << "-c:v libx264 /tmp/output.mp4 -y";
    system(os_command.str().c_str());
    LOG(INFO) << "ffmpeg command: " << os_command.str() << "\n";

    for (auto& it : temporary_file_container)
    {
        remove(it.c_str());
    }

    rmdir(path);
    return 0;
}

int test_ffmpeg_pull_rtsp_object(Protocol::Message& message)
{
    ffmpeg_unit::Media mediaA;
    // ffmpeg_unit::Media mediaB;

    mediaA.Open("rtsp://admin:abcd1234@192.169.8.153");
    // mediaB.Open("rtsp://admin:a1234567@192.169.10.112");

    mediaA.Play();
    // mediaB.Play();

    ffmpeg_unit::FaceRecognition face_recognition;

    while (true)
    {
        auto imgA = mediaA.GetImage();
        // auto imgB = mediaB.GetImage();
        if (imgA.empty())
        {
            LOG(WARNING) << "empty image...\n";
            continue;
        }
        face_recognition.Detect(imgA);
        // face_recognition.Detect(imgB);
        cv::imshow("imgA", imgA);
        // cv::imshow("imgB", imgB);
        cv::waitKey(500);
        // cv::imwrite("/tmp/imgA.jpg", imgA);
        // cv::imwrite("/tmp/imgB.jpg", imgB);

        // LOG(INFO) << "input command:(s to save image, q to quit)\n";
        // char cmd;
        // std::cin >> cmd;
        // if (cmd == 's')
        // {
        //     auto imgA = mediaA.GetImage();
        //     auto imgB = mediaB.GetImage();
        //     cv::imwrite("/tmp/imgA.jpg", imgA);
        //     cv::imwrite("/tmp/imgB.jpg", imgB);
        //     face_recognition.Detect(imgA);
        //     face_recognition.Detect(imgB);
        //     cv::imshow("imgA", imgA);
        //     cv::imshow("imgB", imgB);
        //     cv::waitKey(0);
        // }
        // else if (cmd == 'q')
        // {
        //     break;
        // }
        // else 
        // {
        //     LOG(WARNING) << "invalid command: " << cmd << "\n";
        // }
    }

    return 0;
}

int test_dlib_face_detect(Protocol::Message& message)
{
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::image_window win;
    std::string image_path{"/mnt/remote/190-mnt/faceNet/data11/tmp9055.jpg"};

    dlib::array2d<unsigned char> img;
    dlib::load_image(img, image_path);

    dlib::pyramid_up(img);

    std::vector<dlib::rectangle> dets = detector(img);
    LOG(INFO) << "number of faces detected: " << dets.size() << "\n";

    win.clear_overlay();
    win.set_image(img);
    win.add_overlay(dets, dlib::rgb_pixel(255, 0, 0));

    LOG(INFO) << "Hit enter to process the next image...\n";
    std::cin.get();

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
        {"ffmpeg_rtsp_to_image", ffmpeg_rtsp_to_image},
        {"ffmpeg_images_to_video", ffmpeg_images_to_video},
        {"ffmpeg_images_to_video_in_command", ffmpeg_images_to_video_in_command},
        {"test_ffmpeg_pull_rtsp_object", test_ffmpeg_pull_rtsp_object},
        {"test_dlib_face_detect", test_dlib_face_detect}
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