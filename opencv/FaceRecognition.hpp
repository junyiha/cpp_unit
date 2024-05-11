/**
 * @file FaceRecognition.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once 
#ifndef FACE_RECOGNITION_HPP
#define FACE_RECOGNITION_HPP

#include "protocol.hpp"

namespace opencv_unit
{
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
}  // namespace opencv_unit
#endif  // FACE-RECONGNITION_HPP