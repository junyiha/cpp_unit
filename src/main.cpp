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
#include "nlohmann/json.hpp"
#include "boost/filesystem.hpp"
#include "boost/beast.hpp"
#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"

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
        ~Device()
        {
            m_thread_v2.join();
        }

    public:
        std::thread* m_thread;
        std::thread m_thread_v2;

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

            m_thread = new std::thread(tmp);
        }

        void Join()
        {
            m_thread->join();
        }

        void PrintV2()
        {
            m_thread_v2 = std::thread(print, this);
        }

    private:
        static void print(Device *this_ptr)
        {
            while(true)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                LOG(INFO) << "m_ctx: " << this_ptr->m_ctx << "\n";
            }
        }
    };

    Device d;

    // d.Print();
    // d.Join();
    d.PrintV2();
    
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

int file_to_memory()
{
    const std::string path {"/data/home/user/workspace/videoprocess-vcr-job/build/task.json"};

    std::ifstream file_stream(path);

    file_stream.seekg(0, std::ios::end);
    auto file_size = file_stream.tellg();
    file_stream.seekg(0, std::ios::beg);
    std::vector<char> data_buf(file_size);
    file_stream.read(data_buf.data(), file_size);

    LOG(INFO) << data_buf.data() << "\n";
    return 0;
}

int list_files()
{
    const std::string dir{"/data/home/user/workspace/cpp_unit/"};

    try
    {
        if (boost::filesystem::exists(dir) && boost::filesystem::is_directory(dir))
        {
            for (const auto& entry : boost::filesystem::directory_iterator(dir))
            {
                if (boost::filesystem::is_regular_file(entry.status()))
                {
                    LOG(INFO) << "file: " << entry.path().filename() << "\n";
                }
            }
        }
        else 
        {
            LOG(ERROR) << "invalid directory\n";
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        LOG(ERROR) << e.what() << "\n";
    }
    
    return 0;
}

int request_token()
{
    namespace asio = boost::asio;
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    namespace ssl = boost::asio::ssl;
    using tcp = net::ip::tcp;

    std::string target_addr{"test.ticdata.cn"};
    std::string path{"/zhgd-gateway/zhgd-cus/openApi/token"};

    net::io_context ioc;
    ssl::context ctx(ssl::context::sslv23_client);

    ctx.set_verify_mode(ssl::verify_peer);
    ctx.set_default_verify_paths();

    tcp::resolver resolver(ioc);
    ssl::stream<tcp::socket> stream(ioc, ctx);

    auto result = resolver.resolve(target_addr, "https");
    asio::connect(stream.next_layer(), result.begin(), result.end());
    stream.handshake(ssl::stream_base::client);

    http::request<http::string_body> req{http::verb::post, path, 11};
    req.set(http::field::host, target_addr);
    req.set(http::field::connection, "keep-alive");
    req.set(http::field::accept, "*/*");
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::content_type, "application/json");
    nlohmann::json send_data;
    send_data["accountNo"] = "test";
    send_data["password"] = "12345678";
    req.body() = send_data.dump();
    LOG(INFO) << "request's body data: " << req.body() << "\n";
    LOG(INFO) << "request path: " << req.target() << "\n";

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::dynamic_body> res;
    http::read(stream, buffer, res);

    LOG(INFO) << beast::buffers_to_string(res.body().data()) << "\n";
    // asio::error_code ec;
    // stream.shutdown(ec);
    // if (ec == asio::error::eof)
    // {
    //     ec = {};
    // }

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
    else if (FLAGS_module == "file-to-memory")
    {
        file_to_memory();
    }
    else if (FLAGS_module == "list-files")
    {
        list_files();
    }
    else if (FLAGS_module == "request-token")
    {
        request_token();
    }
    else 
    {
        LOG(ERROR) << "invalid module argument: " << FLAGS_module << "\n";
    }

    google::ShutDownCommandLineFlags();
    google::ShutdownGoogleLogging();
    return 0;
}