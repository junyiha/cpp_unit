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

    class FaceRecognition
    {
    public:
        FaceRecognition()
        {
            fd_modelPath = "/data/home/user/workspace/cpp_unit/data/yunet.onnx";
            fr_modelPath = "/data/home/user/workspace/cpp_unit/data/face_recognition_sface_2021dec.onnx";
            detector = cv::FaceDetectorYN::create(fd_modelPath, "", cv::Size(320, 320), scoreThreshold, nmsThreshold, topK);
        }
        ~FaceRecognition()
        {

        }

        int Detect(cv::Mat& image1)
        {
            cv::TickMeter tm;
            int imageWidth = int(image1.cols * scale);
            int imageHeight = int(image1.rows * scale);
            cv::resize(image1, image1, cv::Size(imageWidth, imageHeight));
            tm.start();

            detector->setInputSize(image1.size());
            cv::Mat faces1;
            detector->detect(image1, faces1);
            if (faces1.rows < 1)
            {
                LOG(INFO) << "Cannot find a face\n";
                return 1;
            }
            tm.stop();
            visualize(image1, -1, faces1, tm.getFPS());

            return 0;
        }

    private:
        void visualize(cv::Mat& input, int frame, cv::Mat& faces, double fps, int thickness = 2)
        {
            std::string fpsString = cv::format("FPS : %.2f", (float)fps);
            if (frame >= 0)
            {
                LOG(INFO) << "Frame: " << frame << ", ";
            }
            LOG(INFO) << "FPS: " << fpsString << "\n";

            for (int i = 0; i < faces.rows; i++)
            {
                LOG(INFO) << "Face " << i 
                        << ", top-left coordinates: (" << faces.at<float>(i, 0) << ", " << faces.at<float>(i, 1) << "), "
                        << "box width: " << faces.at<float>(i, 2) << ", box height: " << faces.at<float>(i, 3) << ", " 
                        << "score: " << cv::format("%.2f", faces.at<float>(i, 14)) << "\n";
                
                cv::rectangle(input, cv::Rect2i(int(faces.at<float>(i, 0)), int(faces.at<float>(i, 1)), int(faces.at<float>(i, 2)), int(faces.at<float>(i, 3))), cv::Scalar(0, 255, 0), thickness);

                circle(input, cv::Point2i(int(faces.at<float>(i, 4)), int(faces.at<float>(i, 5))), 2, cv::Scalar(255, 0, 0), thickness);
                circle(input, cv::Point2i(int(faces.at<float>(i, 6)), int(faces.at<float>(i, 7))), 2, cv::Scalar(0, 0, 255), thickness);
                circle(input, cv::Point2i(int(faces.at<float>(i, 8)), int(faces.at<float>(i, 9))), 2, cv::Scalar(0, 255, 0), thickness);
                circle(input, cv::Point2i(int(faces.at<float>(i, 10)), int(faces.at<float>(i, 11))), 2, cv::Scalar(255, 0, 255), thickness);
                circle(input, cv::Point2i(int(faces.at<float>(i, 12)), int(faces.at<float>(i, 13))), 2, cv::Scalar(0, 255, 255), thickness);
            }
            cv::putText(input, fpsString, cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
        }

    private:
        std::string fd_modelPath;
        std::string fr_modelPath;
        float scoreThreshold{0.9};
        float nmsThreshold{0.3};
        int topK{5000};
        bool save{false};
        float scale{1.0};
        double cosine_similar_thresh {0.363};
        double l2norm_similar_thresh {1.128};
        cv::Ptr<cv::FaceDetectorYN> detector;
    };
}  // namespace ffmpeg_unit

#endif  // MEDIA_PLAYER_HPP