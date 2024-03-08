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
#include "yaml-cpp/yaml.h"

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

int parse_yaml_config()
{
    enum class RobotClass 
    {
        SIM = 1,
        TaiKe
    };

    struct RobotMeta_t
    {
        std::string id;
        std::string ip;
        std::size_t port;
        std::string product;
        std::string vendor;
    };

    class Robot
    {
    public:
        virtual void Print() = 0;

    };

    class SIMRobot : public Robot
    {
    public:
        virtual void Print() override 
        {
            LOG(INFO) << "I'm SIMRobot!\n";
        };
    };

    class TaiKeRobot : public Robot 
    {
        virtual void Print() override 
        {
            LOG(INFO) << "I'm TaiKeRobot!\n";
        };
    };

    std::map<std::string, RobotClass> robot_class_map = 
    {
        {"SIMRobot", RobotClass::SIM},
        {"TaiKeRobot", RobotClass::TaiKe}
    };

    std::unordered_map<Robot*, RobotMeta_t> ctx_map;

    const std::string file{"/data/vcr/Configurations/robot.yaml"};

    YAML::Node config = YAML::LoadFile(file);
    std::vector<std::string> ids = config["id"].as<std::vector<std::string>>();
    for (const auto& id : ids)
    {
        LOG(INFO) << "id: " << id << "\n";
        LOG(INFO) << "ip: " << config[id]["ip"].as<std::string>() << "\n"
                  << "port: " << config[id]["port"].as<std::string>() << "\n"
                  << "product: " << config[id]["product"].as<std::string>() << "\n"
                  << "vendor: " << config[id]["vendor"].as<std::string>() << "\n\n";
        RobotMeta_t robot_meta;
        robot_meta.id = id;
        robot_meta.ip = config[id]["ip"].as<std::string>();
        robot_meta.port = config[id]["port"].as<std::size_t>();
        robot_meta.product = config[id]["product"].as<std::string>();
        robot_meta.vendor = config[id]["vendor"].as<std::string>();
        
        auto it = std::find_if(robot_class_map.begin(), robot_class_map.end(), [robot_meta](const std::pair<std::string, RobotClass>& robot){return robot_meta.product == robot.first;});
        if (it == robot_class_map.end())
        {
            LOG(ERROR) << "invalid product: " << robot_meta.product << ", id: " << robot_meta.id << "\n";
            continue;
        }
        Robot* robot_ptr;
        switch (it->second)
        {
            case RobotClass::SIM:
            {
                robot_ptr = new SIMRobot;
                break;
            }
            case RobotClass::TaiKe:
            {
                robot_ptr = new TaiKeRobot;
                break;
            }
        }
        ctx_map[robot_ptr] = robot_meta;
    }

    LOG(INFO) << "input id: ";
    std::string cmd;
    std::cin >> cmd;
    auto it = std::find_if(ctx_map.begin(), ctx_map.end(), [cmd](const std::pair<Robot*, RobotMeta_t>& ctx){return ctx.second.id == cmd;});
    if (it == ctx_map.end())
    {
        LOG(ERROR) << "invalid input id: " << cmd << "\n";
        return 1;
    }
    LOG(INFO) << "ip: " << it->second.ip << "\n"
              << "port: " << it->second.port << "\n"
              << "product: " << it->second.product << "\n"
              << "vendor: " << it->second.vendor << "\n\n";
    it->first->Print();

    return 0;
}

int class_with_thread()
{
    class Device
    {
    private:
        const std::string m_ctx{"aaa"};

    public:
        std::thread m_thread;

        void Print()
        {
            auto tmp = [this]()
            {
                while(true)
                {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    LOG(INFO) << "m_ctx: " << this->m_ctx << "\n";
                }
            };

            m_thread = std::thread(tmp);
        }
    };

    Device d;

    d.Print();

    return 0;
}

int class_delegating_constructor()
{
    class MyClass
    {
    public:
        MyClass(int value)
        {
            LOG(INFO) << "value: " << value << "\n";
        }

        MyClass() : MyClass(42)
        {
            LOG(INFO) << "委托构造函数被调用";
        }
    };

    MyClass obj;

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
    else if (FLAGS_module == "parse-yaml-config")
    {
        parse_yaml_config();
    }
    else if (FLAGS_module == "class-with-thread")
    {
        class_with_thread();
    }
    else if (FLAGS_module == "class-delegating-constructor")
    {
        class_delegating_constructor();
    }
    else 
    {
        LOG(ERROR) << "invalid module argument: " << FLAGS_module << "\n";
    }

    google::ShutDownCommandLineFlags();
    google::ShutdownGoogleLogging();
    return 0;
}