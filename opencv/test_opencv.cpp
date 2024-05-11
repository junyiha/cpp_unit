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
#include "FaceRecognition.hpp"

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

int opencv_basic_example(Protocol::Message& message)
{
    LOG(INFO) << "opencv basic example\n";
    cv::String model = "/data/home/user/workspace/cpp_unit/data/yolov5s.pt";
    cv::String config = "/data/home/user/workspace/cpp_unit/data/yolov5s.yaml";
    cv::dnn::Net net = cv::dnn::readNet(model, config);
    cv::Mat image = cv::imread("/tmp/image-121.jpg");
    cv::Mat blob = cv::dnn::blobFromImage(image, 1.0 / 255.0, cv::Size(640, 640), cv::Scalar(0, 0, 0), true, false);
    net.setInput(blob);
    cv::Mat detections = net.forward();

    return 0;
}

static void visualize(cv::Mat& input, int frame, cv::Mat& faces, double fps, int thickness = 2)
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

int opencv_dnn_face_recognition(Protocol::Message& message)
{
    // std::string fd_modelPath{"/data/home/user/workspace/cpp_unit/data/face_detection_yunet_2023mar.onnx"};
    std::string fd_modelPath = "/data/home/user/workspace/cpp_unit/data/yunet.onnx";
    std::string fr_modelPath{"/data/home/user/workspace/cpp_unit/data/face_recognition_sface_2021dec.onnx"};
    float scoreThreshold{0.9};
    float nmsThreshold{0.3};
    int topK{5000};
    bool save{false};
    float scale{1.0};

    double cosine_similar_thresh = 0.363;
    double l2norm_similar_thresh = 1.128;

    cv::Ptr<cv::FaceDetectorYN> detector = cv::FaceDetectorYN::create(fd_modelPath, "", cv::Size(320, 320), scoreThreshold, nmsThreshold, topK);

    cv::TickMeter tm;

    std::string img_path{"/data/home/user/workspace/cpp_unit/data/images/image-136.jpg"};
    cv::Mat image1 = cv::imread(cv::samples::findFile(img_path));
    if (image1.empty())
    {
        LOG(ERROR) << "Cannot read image: " << img_path << "\n";
        return 1;
    }

    int imageWidth = int(image1.cols * scale);
    int imageHeight = int(image1.rows * scale);
    cv::resize(image1, image1, cv::Size(imageWidth, imageHeight));
    tm.start();

    detector->setInputSize(image1.size());

    cv::Mat faces1;
    detector->detect(image1, faces1);
    if (faces1.rows < 1)
    {
        LOG(INFO) << "Cannot find a face in " << img_path << "\n";
        return 1;
    }

    tm.stop();

    visualize(image1, -1, faces1, tm.getFPS());
    
    if (save)
    {
        LOG(INFO) << "Saving result.jpg...\n";
        cv::imwrite("/tmp/result.jpg", image1);
    }

    cv::imshow("image1", image1);
    cv::pollKey();

    LOG(INFO) << "Press any key to exit...\n";
    cv::waitKey(0);

    LOG(INFO) << "Done\n";

    return 0;
}

int opencv_dnn_face_recognition_class(Protocol::Message& message)
{
    opencv_unit::FaceRecognition face_recognition;

    std::string img_path{"/data/home/user/workspace/cpp_unit/data/images/image-136.jpg"};
    cv::Mat image1 = cv::imread(cv::samples::findFile(img_path));
    if (image1.empty())
    {
        LOG(ERROR) << "Cannot read image: " << img_path << "\n";
        return 1;
    }

    face_recognition.Detect(image1);
    cv::imshow("image1", image1);
    cv::waitKey(0);

    return 0;
}

int main(int argc, char* argv[])
{
    InitGlog(argv[0]);
    gflags::SetUsageMessage("cpp unit project!");
    cv::CommandLineParser parser(argc, argv, 
        "{module    |   |module layer}"
    );
    parser.printMessage();
    std::string module_cmd = parser.get<std::string>("module");

    LOG(INFO) << "--cpp unit--" << std::endl;
    LOG(INFO) << gflags::ProgramUsage() << std::endl;

    Protocol::Message message;

    std::map<std::string, std::function<int(Protocol::Message&)>> func_table =
    {
        {"opencv_basic_example", opencv_basic_example},
        {"opencv_dnn_face_recognition", opencv_dnn_face_recognition},
        {"opencv_dnn_face_recognition_class", opencv_dnn_face_recognition_class}
    };

    auto it = func_table.find(module_cmd);
    if (it != func_table.end())
    {
        it->second(message);
    }
    else 
    {
        LOG(ERROR) << "invalid argument: " << module_cmd << "\n";
    }

    google::ShutdownGoogleLogging();
    return 0;
}