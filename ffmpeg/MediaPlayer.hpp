/**
 * @file MediaPlayer.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once 
#ifndef MEDIA_PLAYER_HPP
#define MEDIA_PLAYER_HPP

#include "protocol.hpp"

namespace ffmpeg_unit
{
    class Media
    {
    public:
        Media()
        {
            frame_ptr = av_frame_alloc();
            format_context_ptr = avformat_alloc_context();
            av_dict_set(&options, "rtsp_transport", "tcp", 0);
        }
        ~Media()
        {
            Close();

            avcodec_close(codec_context_ptr);
            av_frame_free(&frame_ptr);
            avcodec_free_context(&codec_context_ptr);
            avformat_close_input(&format_context_ptr);
            avformat_free_context(format_context_ptr);
            avformat_network_deinit();
        }

        bool Open(std::string rtsp_url)
        {
            int res{-1};
            int decode_result{-1};
            res = avformat_open_input(&format_context_ptr, rtsp_url.c_str(), nullptr, &options);
            if (res != 0)
            {
                return false;
            }

            res = avformat_find_stream_info(format_context_ptr, nullptr);
            if (res != 0)
            {
                return false;
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
                return false;
            }

            codec_params_ptr = format_context_ptr->streams[video_stream_index]->codecpar;
            codec_ptr = const_cast<AVCodec *>(avcodec_find_decoder(codec_params_ptr->codec_id));
            if (codec_ptr == nullptr)
            {
                return false;
            }

            codec_context_ptr = avcodec_alloc_context3(codec_ptr);
            avcodec_parameters_to_context(codec_context_ptr, codec_params_ptr);
            res = avcodec_open2(codec_context_ptr, nullptr, nullptr);
            if (res < 0)
            {
                return false;
            }

            sws_context_ptr = sws_getContext(codec_context_ptr->width, codec_context_ptr->height, codec_context_ptr->pix_fmt, codec_context_ptr->width, codec_context_ptr->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

            addr = rtsp_url;

            return true;
        }

        bool Play()
        {
            auto tmp = [this]()
            {
                int res{-1};
                int decode_result{-1};
                while (true)
                {
                    if (exit_flag)
                    {
                        break;
                    }

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
                    cv::imshow(addr, image);
                    cv::waitKey(10);
                    count++;

                    av_packet_unref(&packet);
                }
            };
            thread = std::thread(tmp);

            return true;
        }

        void Close()
        {
            exit_flag = true;
            if (thread.joinable())
            {
                thread.join();
            }
        }

        cv::Mat GetImage()
        {
            return image;
        }

    private:
        AVCodecParameters* codec_params_ptr;
        AVCodec* codec_ptr{nullptr};
        AVPacket packet;
        cv::Mat image;
        int cv_linesize[1];
        int count{0};
        AVCodecContext* codec_context_ptr{nullptr};
        AVFrame* frame_ptr{nullptr};
        AVFrame* frame_rgb_ptr{nullptr};
        AVFormatContext* format_context_ptr{nullptr};
        AVDictionary* options{nullptr};
        SwsContext* sws_context_ptr{nullptr};
        std::string addr;
        std::thread thread;
        bool exit_flag{false};
        int video_stream_index{-1};
    };
}  // namespace ffmpeg_unit

#endif  // MEDIA_PLAYER_HPP