/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "protocol.hpp"
#include "abstract_factory.hpp"

void InitGlog(const char *program_path)
{
    google::InitGoogleLogging(program_path);
    
    FLAGS_log_dir = "/data/home/user/workspace/cpp_unit/data/log/";
    FLAGS_log_year_in_prefix = false;
    FLAGS_log_utc_time = false;
    FLAGS_log_link = false;
    FLAGS_log_prefix = false;
    FLAGS_logbufsecs = 0;
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    FLAGS_max_log_size = 10;  // 1MB
    FLAGS_minloglevel = google::GLOG_INFO;
    FLAGS_stop_logging_if_full_disk = true;
    FLAGS_log_file_header = false;
}

int abstract_factory()
{
    enum class FactoryEnum : unsigned int 
    {
        WIN = 1,
        MAC
    };

    AbstractFactory::AbstractFactory* fac;
    FactoryEnum style{FactoryEnum::WIN};
    
    switch (style)
    {
        case FactoryEnum::WIN:
        {
            fac = new AbstractFactory::WinFactory;
            break;
        }
        case FactoryEnum::MAC:
        {
            fac = new AbstractFactory::MacFactory;
            break;
        }
        default:
            LOG(ERROR) << "invalid style\n";
    }

    AbstractFactory::Button* button = fac->CreateButton();
    AbstractFactory::Border* border = fac->CreateBorder();

    return 0;
}

DEFINE_string(module, "design", "module layer");

int main(int argc, char* argv[])
{
    InitGlog(argv[0]);
    gflags::SetUsageMessage("cpp unit project!");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    LOG(INFO) << "--cpp unit--" << std::endl;
    LOG(INFO) << gflags::ProgramUsage() << std::endl;

    if (FLAGS_module == "abstract-factory")
    {
        abstract_factory();
    }
    else 
    {
        LOG(ERROR) << "invalid module argument: " << FLAGS_module << "\n";
    }

    google::ShutDownCommandLineFlags();
    google::ShutdownGoogleLogging();
    return 0;
}