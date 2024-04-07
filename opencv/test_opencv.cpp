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
        {"opencv_basic_example", opencv_basic_example}
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