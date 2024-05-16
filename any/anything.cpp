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
        Device():m_thread_v2(std::bind(&Device::PrintCallBack, this))
        {

        }
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

        void PrintCallBack()
        {
            while(true)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                LOG(INFO) << "m_ctx: " << m_ctx << "\n";
            }
        }

        void PrintV3()
        {
            
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
    // d.PrintV2();
    // LOG(INFO) << "print v3\n";
    // d.PrintV3();

    class A
    {
    public:
        void CreateTask(std::string id)
        {
            task_table[id] = std::thread(&A::ExecuteTask, this);
        }

    private:
        void ExecuteTask()
        {
            while (true)
            {
                LOG(INFO) << "execute task\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

    public:
        std::map<std::string, std::thread> task_table;
    };

    A a;
    std::string task{"aaa"};
    a.CreateTask(task);

    auto it = a.task_table.find(task);
    if (it != a.task_table.end())
    {
        it->second.join();
    }
    else 
    {
        LOG(INFO) << "not found task: " << task << "\n";
    }
    
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

int list_all_mp4_file()
{
    const std::string dir{"/mnt/remote/190-mnt/样本采集/车冲/cz002/video/wash/"};

    std::vector<std::string> path_container;

    for (const auto& entry : boost::filesystem::directory_iterator(dir))
    {
        if (boost::filesystem::is_directory(entry.path()))
        {
            for (const auto& iter_entry : boost::filesystem::directory_iterator(entry.path()))
            {
                for (const auto& tmp_entry : boost::filesystem::directory_iterator(iter_entry.path()))
                {
                    if (boost::filesystem::is_regular_file(tmp_entry.status()))
                    {
                        path_container.push_back(tmp_entry.path().string());
                    }
                }
            }
        }
    }

    const std::string destination_dir{"/mnt/remote/190-mnt/zhangjunyi/Videos/vehcile/car-wash/"};
    for (auto& path : path_container)
    {
        LOG(INFO) << path << "\n";
        std::stringstream os_str;
        os_str << "cp " << path << " " << destination_dir;
        std::system(os_str.str().c_str());
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

int httplib_request_token()
{
    const std::string host{"test.ticdata.cn"};
    // const std::string host{"wisdomsite.ticdata.cn"};
    const std::string path{"/zhgd-gateway/zhgd-cus/openApi/token"};
    const std::string body_type {"application/json"};

    httplib::SSLClient cli(host);
    cli.set_connection_timeout(3,0);  // 3 seconds
    cli.set_read_timeout(5, 0);  // 5 seconds
    cli.set_write_timeout(5, 0);  // 5 seconds
    cli.enable_server_certificate_verification(false);

    nlohmann::json send_data;
    // send_data["accountNo"] = "test";  // test
    send_data["accountNo"] = "qian001";
    send_data["password"] = "12345678";
    auto res = cli.Post(path, send_data.dump(), body_type);
    if (res.error() != httplib::Error::Success)
    {
        LOG(ERROR) << "failed to send post data: " << send_data.dump() << "\n"
                   << "request to url: " << host + path << "\n";
        return -1;
    }

    LOG(INFO) << "response body: " << res->body << "\n";

    return 0;
}

int httplib_upload_file()
{
    const std::string host{"www.norzoro.cn"};
    const std::string path{"/api/upload/"};

    httplib::SSLClient cli(host);
    cli.set_connection_timeout(3,0);  // 3 seconds
    cli.set_read_timeout(5, 0);  // 5 seconds
    cli.set_write_timeout(5, 0);  // 5 seconds
    cli.enable_server_certificate_verification(false);

    std::ifstream input_file("/home/user/Pictures/wallhaven-d58o2l_1920x1080.png", std::ios::in | std::ios::binary);
    std::ostringstream file_content;
    file_content << input_file.rdbuf();
    input_file.close();
    std::string file_data = file_content.str();

    httplib::MultipartFormDataItems items = {
        {"path", "media/ai_rk1126/8d514bd3-3cfc-a44f-355a-a91e597ed1f3/2024-03-15/18-40/1-169474517825712.jpg", "", ""},
        {"image", file_data, "1-1694745178257.jpg", "image/jpeg"}
    };

    auto res = cli.Post(path, items);
    if (res.error() != httplib::Error::Success)
    {
        LOG(ERROR) << "failed to send multipart/form-data data\n";
        return -1;
    }

    LOG(INFO) << "response body: " << res->body << "\n";

    return 0;
}

int httplib_push_record()
{
    // const std::string host{"test.ticdata.cn"};
    const std::string host{"wisdomsite.ticdata.cn"};
    const std::string path{"/zhgd-gateway/smart-mon/openApi/addAiData"};
    const std::string body_type {"application/json"};
    const std::string image_url{"https://www.norzoro.cn/media/ai_rk1126/8d514bd3-3cfc-a44f-355a-a91e597ed1f3/2024-03-15/18-40/1-169474517825712.jpg"};
    nlohmann::json send_data;

    send_data["AiDatoDto"]["deviceNo"] = "364f6ecb-bc33-b351-8bd2-95143bb48f39";
    send_data["AiDatoDto"]["warnType"] = 1;
    send_data["AiDatoDto"]["warnAt"] = "123";
    send_data["AiDatoDto"]["warnPic"] = image_url;

    httplib::SSLClient cli(host);
    cli.set_connection_timeout(3,0);  // 3 seconds
    cli.set_read_timeout(5, 0);  // 5 seconds
    cli.set_write_timeout(5, 0);  // 5 seconds
    cli.enable_server_certificate_verification(false);
    cli.set_basic_auth("qian001", "12345678");

    auto res = cli.Post(path, send_data.dump(), body_type);
    if (res.error() != httplib::Error::Success)
    {
        LOG(ERROR) << "failed to send post data: " << send_data.dump() << "\n"
                   << "request to url: " << host + path << "\n";
        return -1;
    }

    LOG(INFO) << "response body: " << res->body << "\n";

    return 0;
}

std::string request_token_test(const std::string host, const std::string username, const std::string passwd)
{
    bool success{false};
    nlohmann::json parsed_data;
    std::string token;
    // const std::string host{"test.ticdata.cn"};
    const std::string path{"/zhgd-gateway/zhgd-cus/openApi/token"};
    // const std::string path{"/zhgd-cus/openApi/token"};
    const std::string body_type {"application/json"};

    httplib::SSLClient cli(host);
    cli.set_connection_timeout(3,0);  // 3 seconds
    cli.set_read_timeout(5, 0);  // 5 seconds
    cli.set_write_timeout(5, 0);  // 5 seconds
    cli.enable_server_certificate_verification(false);

    nlohmann::json send_data;
    send_data["accountNo"] = username;
    send_data["password"] = passwd;
    auto res = cli.Post(path, send_data.dump(), body_type);
    if (res.error() != httplib::Error::Success)
    {
        LOG(ERROR) << "failed to send post data: " << send_data.dump() << "\n"
                   << "request to url: " << host + path << "\n";
        return token;
    }

    LOG(INFO) << "response body: " << res->body << "\n";
    
    try
    {
        parsed_data = nlohmann::json::parse(res->body);
        success = parsed_data["success"];
        if (success)
        {
            token = parsed_data["data"]["token"];
        }
    }
    catch(const nlohmann::json::parse_error& e)
    {
        LOG(ERROR) << "parse error\n";
    }
    catch(const nlohmann::json::type_error& e)
    {
        LOG(ERROR) << "type error\n";
    }    

    return token;
}

std::tuple<std::string, std::string> get_image_saved_path(const std::string prefix_path, const std::string image_path)
{
    std::string save_path;

    save_path = prefix_path;

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    auto now_tm = std::localtime(&timestamp);

    std::stringstream year_name;
    year_name << std::put_time(now_tm, "%Y-%m-%d");
    std::stringstream hour_name;
    hour_name << std::put_time(now_tm, "%H-%M");

    save_path += year_name.str() + "/" + hour_name.str() + "/";

    auto pos = image_path.find_last_of('/');
    std::string file_name = image_path.substr(pos + 1);

    save_path += file_name;

    return std::make_tuple(file_name, save_path);
}

int httplib_upload_file_and_push_record()
{
    const std::string upload_host{"www.norzoro.cn"};
    const std::string upload_path{"/api/upload/"};
    const std::string prefix_path{"media/ai_rk1126/8d514bd3-3cfc-a44f-355a-a91e597ed1f3/"};
    const std::string image_path{"/home/user/Pictures/wallhaven-d58o2l_1920x1080.png"};

    std::string file_name;
    std::string saved_path;

    std::tie(file_name, saved_path) = get_image_saved_path(prefix_path, image_path);

    httplib::SSLClient upload_cli(upload_host);
    upload_cli.set_connection_timeout(3,0);  // 3 seconds
    upload_cli.set_read_timeout(5, 0);  // 5 seconds
    upload_cli.set_write_timeout(5, 0);  // 5 seconds
    upload_cli.enable_server_certificate_verification(false);

    std::ifstream input_file(image_path, std::ios::in | std::ios::binary);
    std::ostringstream file_content;
    file_content << input_file.rdbuf();
    input_file.close();
    std::string file_data = file_content.str();

    httplib::MultipartFormDataItems items = {
        {"path", saved_path, "", ""},
        {"image", file_data, file_name, "image/jpeg"}
    };

    auto res = upload_cli.Post(upload_path, items);
    if (res.error() != httplib::Error::Success)
    {
        LOG(ERROR) << "failed to send multipart/form-data data\n";
        return -1;
    }

    LOG(INFO) << "response body: " << res->body << "\n";

    nlohmann::json parsed_upload_data;
    int response_code;
    std::string response_message;
    try
    {
        parsed_upload_data = nlohmann::json::parse(res->body);
        response_code = parsed_upload_data["code"];
        response_message = parsed_upload_data["message"];
    }
    catch (nlohmann::json::parse_error& e)
    {
        LOG(ERROR) << "parse json error\n";
        return -1;
    }
    catch (nlohmann::json::type_error& e)
    {
        LOG(ERROR) << "type json error\n";
        return -1;
    }

    if (response_code != 1)
    {
        LOG(ERROR) << "failed to upload file\n";
        return -1;
    }

    const std::string push_host{"test.ticdata.cn"};
    // const std::string push_host{"wisdomsite.ticdata.cn"};
    // const std::string push_path{"/smart-mon/openApi/addAiData"};
    const std::string push_path{"/zhgd-gateway/smart-mon/openApi/addAiData"};
    const std::string push_body_type {"application/json"};
    const std::string push_image_url{"https://www.norzoro.cn/media/ai_rk1126/8d514bd3-3cfc-a44f-355a-a91e597ed1f3/2024-03-15/18-40/1-169474517825712.jpg"};
    const std::string username{"qian001"};
    const std::string passwd{"12345678"};
    std::string push_token;
    nlohmann::json push_send_data;

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    auto now_tm = std::localtime(&timestamp);
    std::stringstream warn_time;
    warn_time << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
    LOG(INFO) << "warning time: " << warn_time.str() << "\n";

    push_send_data["deviceNo"] = "10e2a024-211f-1d29-472c-ac4c6d591ccd";
    push_send_data["warnType"] = 1;
    push_send_data["warnAt"] = warn_time.str();
    push_send_data["warnPic"] = push_image_url;

    push_token = request_token_test(push_host, username, passwd);
    if (push_token.empty())
    {
        LOG(ERROR) << "get token failed\n";
        return -1;
    }

    httplib::SSLClient push_cli(push_host);
    push_cli.set_connection_timeout(3,0);  // 3 seconds
    push_cli.set_read_timeout(5, 0);  // 5 seconds
    push_cli.set_write_timeout(5, 0);  // 5 seconds
    push_cli.enable_server_certificate_verification(false);
    // push_cli.set_basic_auth(username, passwd);
    // push_cli.set_bearer_token_auth(push_token);
    httplib::Headers headers;
    headers.emplace("Authorization", push_token);
    headers.emplace("Content-Type", push_body_type);

    LOG(INFO) << "push data: " << push_send_data.dump() << "\n";

    auto push_res = push_cli.Post(push_path, headers, push_send_data.dump(), push_body_type);
    if (push_res.error() != httplib::Error::Success)
    {
        LOG(ERROR) << "failed to send post data: " << push_send_data.dump() << "\n"
                   << "request to url: " << push_host + push_path << "\n";
        return -1;
    }

    LOG(INFO) << "response body: " << push_res->body << "\n";
    nlohmann::json parsed_push_data;
    std::string rsp_timestamp;
    std::string rsp_path;
    int rsp_status;
    std::string rsp_error;
    std::string rsp_message;
    std::string rsp_request_id;
    try
    {
        parsed_push_data = nlohmann::json::parse(push_res->body);
        rsp_timestamp = parsed_push_data["timestamp"];
        rsp_path = parsed_push_data["path"];
        rsp_status = parsed_push_data["status"];
        rsp_error = parsed_push_data["error"];
        rsp_request_id = parsed_push_data["requestId"];
    }
    catch (nlohmann::json::parse_error& e)
    {
        LOG(ERROR) << "parse json error\n";
        return -1;
    }
    catch (nlohmann::json::type_error& e)
    {
        LOG(ERROR) << "type json error\n";
        return -1;
    }

    if (rsp_status != 200)
    {
        LOG(ERROR) << "failed to push record\n";
        return -1;
    }

    LOG(INFO) << "success to push record\n";

    return 0;
}

int get_year_month_day_hour_minute()
{
    auto now = std::chrono::system_clock::now();

    auto ms = std::chrono::time_point_cast<std::chrono::microseconds>(now);

    auto timestamp = ms.time_since_epoch().count();

    auto now_tm = std::localtime(&timestamp);
    
    LOG(INFO) << "now: " << timestamp << "\n"
              << "year: " << now_tm->tm_year + 1900 << ", "
              << "month: " << now_tm->tm_mon + 1 << ", "
              << "day: " << now_tm->tm_mday << "\n"
              << "hour: " << now_tm->tm_hour << ", "
              << "minute: " << now_tm->tm_min << ", "
              << "second: " << now_tm->tm_sec << "\n";
    
    LOG(INFO) << "currently time: " << std::put_time(now_tm, "%Y-%m-%d %H:%M") << "\n";

    std::stringstream year_name;
    year_name << std::put_time(now_tm, "%Y-%m-%d");
    std::stringstream hour_name;
    hour_name << std::put_time(now_tm, "%H-%M");
    LOG(INFO) << "year_name: " << year_name.str() << ", hour_name: " << hour_name.str() << "\n"; 

    return 0;
}

int get_save_path()
{
    const std::string file_path{"/home/user/Pictures/wallhaven-d58o2l_1920x1080.png"};
    std::string save_path{"media/ai_rk1126/8d514bd3-3cfc-a44f-355a-a91e597ed1f3/"};

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    auto now_tm = std::localtime(&timestamp);

    std::stringstream year_name;
    year_name << std::put_time(now_tm, "%Y-%m-%d");
    std::stringstream hour_name;
    hour_name << std::put_time(now_tm, "%H-%M");

    save_path += year_name.str() + "/" + hour_name.str() + "/";

    auto pos = file_path.find_last_of('/');
    std::string file_name = file_path.substr(pos + 1);

    save_path += file_name;
    LOG(INFO) << "save path: " << save_path << "\n";

    return 0;
}

int http_server_with_mongoose()
{
    struct HttpMessage
    {
        std::string path;
        std::string method;
        std::string body;

        std::string response_type;
        std::string response_body;
    };

    class HttpWithMongoose
    {
    private:
        struct mg_mgr mgr;
        struct mg_connection *connect;
        boost::asio::thread_pool tp;

    public:
        HttpWithMongoose() : tp(std::thread::hardware_concurrency())
        {
            mg_mgr_init(&mgr);
        }
        virtual ~HttpWithMongoose()
        {
            mg_mgr_free(&mgr);
        }

    public:
        void Start()
        {
            connect = mg_http_listen(&mgr, "0.0.0.0:9999", handle_event, this);

            while (true)
            {
                mg_mgr_poll(&mgr, 50);
            }
        }

    private:
        void Dispath(HttpMessage& http_message)
        {
            LOG(INFO) << "path: " << http_message.path << "\n"
                      << "method: " << http_message.method << "\n"
                      << "body: " << http_message.body << "\n";
            
            http_message.response_type = "Content-type: application/json";

            nlohmann::json reply_data;
            reply_data["hello"] = "http with mongoose!";
            http_message.response_body = reply_data.dump();
        }

    private:
        static void handle_event(mg_connection *connect, int ev, void *ev_data, void *fn_data)
        {
            HttpWithMongoose* this_ptr = static_cast<HttpWithMongoose *>(fn_data);
            struct mg_http_message* hm = static_cast<struct mg_http_message*>(ev_data);

            switch (ev)
            {
                case MG_EV_HTTP_MSG:
                {
                    HttpMessage http_message;

                    http_message.path = std::string(hm->uri.ptr, hm->uri.len);
                    http_message.method = std::string(hm->method.ptr, hm->method.len);
                    http_message.body = std::string(hm->body.ptr, hm->body.len);

                    // std::thread tmp_thread = std::thread([](HttpMessage http_message, HttpWithMongoose* this_ptr, struct mg_connection* connect)
                    // {
                    //     this_ptr->Dispath(http_message);

                    //     mg_http_reply(connect, 200, http_message.response_type.c_str(), http_message.response_body.c_str());
                    //     connect->is_draining = 1;
                    // }, http_message, this_ptr, connect);
                    // tmp_thread.detach();

                    auto dpcp = [](HttpMessage http_message, HttpWithMongoose* this_ptr, struct mg_connection* connect)
                    {
                        this_ptr->Dispath(http_message);

                        mg_http_reply(connect, 200, http_message.response_type.c_str(), http_message.response_body.c_str());
                        connect->is_draining = 1;
                    };
                    boost::asio::post(this_ptr->tp, std::bind(dpcp, http_message, this_ptr, connect));

                    break;
                }
            }
        }
    };

    HttpWithMongoose http_server;

    http_server.Start();

    return 0;
}

int test_son_call_parent_func()
{
    class Parent
    {
    public:
        void parentFunction()
        {
            LOG(INFO) << "this is a parent function\n";
        }
    };

    class child : public Parent
    {
    public:
        void childFunction()
        {
            LOG(INFO) << "this is the child function \n";
        }

        void callParentFunction()
        {
            parentFunction();
        }
    };

    child c;
    c.callParentFunction();

    return 0;
}

int decorator_sample()
{
    class Coffee
    {
    public:
        virtual void serve() const = 0;
    };

    class SimpleCoffee : public Coffee
    {
    public:
        void serve() const override 
        {
            LOG(INFO) << "Simple Coffee\n";
        };
    };

    class CoffeeDecorator : public Coffee
    {
    protected:
        Coffee* coffee;

    public:
        CoffeeDecorator(Coffee* coffee) : coffee(coffee) 
        {

        }
        
        void serve() const override 
        {  
            coffee->serve();
        }
    };

    class MilkDecorator : public CoffeeDecorator 
    {
    public:
        MilkDecorator(Coffee* coffee) : CoffeeDecorator(coffee)
        {

        }

        void serve() const override
        {
            CoffeeDecorator::serve();
            LOG(INFO) << " + milk\n";
        }
    };

    Coffee* coffee = new SimpleCoffee();
    coffee->serve();

    Coffee* coffeeWithMilk = new MilkDecorator(coffee);
    coffeeWithMilk->serve();

    delete coffee;
    delete coffeeWithMilk;

    return 0;
}

int test_task_use_service()
{
    class Service;

    class Task
    {
    private:
        Service* service;

    public:
        void Robot()
        {

        }
    };

    class Service
    {
    private:
        Task m_task;

    public:
        void SendRobot()
        {
            LOG(INFO) << "in service, send to robot\n";
        }
    };

    Service service;

    service.SendRobot();

    class Base 
    {
    public:
        virtual void foo()
        {
            LOG(INFO) << "Base::foo()\n";
        }
    };

    class Derived : public Base 
    {
    public:
        void foo() override
        {
            LOG(INFO) << "Derived::foo()\n";
        }
    };
    
    Base* base_ptr = new Derived();
    base_ptr->foo();
    delete base_ptr;

    return 0;
}

int token_and_push()
{
    const std::string addr{"https://test.ticdata.cn/zhgd-gateway/zhgd-cus/openApi/token&https://test.ticdata.cn/zhgd-gateway/smart-mon/openApi/addAiData"};

    std::string token_addr;
    std::string push_addr;

    auto pos = addr.find_first_of('&');
    token_addr = addr.substr(0, pos);
    push_addr = addr.substr(pos + 1);

    LOG(INFO) << "unprocess address: " << addr << "\n"
              << "token address: " << token_addr << "\n"
              << "push address: " << push_addr << "\n";

    return 0;
}

int test_ffmpeg_example()
{
    AVFormatContext* fm_ctx{nullptr};
    AVCodec* codec{nullptr};
    AVStream* stream{nullptr};
    AVPacket avpkt;
    AVFrame* frame;

    return 0;
}

int opencv_sample()
{
    const std::string path{"/home/user/zjy-190/Documents/rk_1126/rk_release/static/warning_pictures/ujy94-frame-48569.jpg"};
    const std::string box_str = R"(
        {"label":10001,"score":71,"x1":18,"x2":535,"y1":169,"y2":715}
    )";

    cv::Mat img;
    img = cv::imread(path);
    int x1,y1,x2,y2;

    nlohmann::json parse_data;
    try
    {
        parse_data = nlohmann::json::parse(box_str);
        x1 = parse_data["x1"];
        y1 = parse_data["y1"];
        x2 = parse_data["x2"];
        y2 = parse_data["y2"];
    }
    catch(nlohmann::json::parse_error& e)
    {
        LOG(ERROR) << "parse error, message: " << e.what() << "\n";
    }
    catch(nlohmann::json::type_error& e)
    {
        LOG(ERROR) << "type error, message: " << e.what() << "\n";
    }    

    cv::Point top_left(x1, y1);
    cv::Point bottom_right(x2, y2);

    cv::rectangle(img, top_left, bottom_right, cv::Scalar(0, 0, 255), 2);

    std::vector<uchar> buffer;
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 95};
    cv::imencode(".jpg", img, buffer, params);

    const std::string upload_host{"www.norzoro.cn"};
    const std::string upload_path{"/api/upload/"};
    const std::string prefix_path{"media/ai_rk1126/8d514bd3-3cfc-a44f-355a-a91e597ed1f3/"};
    std::string file_name;
    std::string saved_path;

    std::tie(file_name, saved_path) = get_image_saved_path(prefix_path, path);
    LOG(INFO) << "saved_path: " << saved_path << "\n";

    httplib::SSLClient upload_cli(upload_host);
    upload_cli.set_connection_timeout(3,0);  // 3 seconds
    upload_cli.set_read_timeout(5, 0);  // 5 seconds
    upload_cli.set_write_timeout(5, 0);  // 5 seconds
    upload_cli.enable_server_certificate_verification(false);

    std::string file_data(buffer.begin(), buffer.end());

    httplib::MultipartFormDataItems items = {
        {"path", saved_path, "", ""},
        {"image", file_data, file_name, "image/jpeg"}
    };

    auto res = upload_cli.Post(upload_path, items);
    if (res.error() != httplib::Error::Success)
    {
        LOG(ERROR) << "failed to send multipart/form-data data\n";
        return -1;
    }

    LOG(INFO) << "response body: " << res->body << "\n";

    cv::imshow("tmp", img);
    cv::waitKey(0);

    return 0;
}

// 回调函数，用于处理收到的数据
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *stream) {
    size_t total_size = size * nmemb;
    stream->append((char *)contents, total_size);
    return total_size;
}

int curl_example()
{
    CURL* curl;
    CURLcode res;
    std::string response;
    nlohmann::json body_data;

    body_data["id"] = "1001";

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.169.0.152:13001/api/robot/status");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_data.dump().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            LOG(ERROR) << "failed to perform request: " << curl_easy_strerror(res) << "\n";
        }
        LOG(INFO) << response << "\n";

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}

int ffmpeg_record_video()
{
    avformat_network_init();

    AVFormatContext* p_format_ctx = avformat_alloc_context();
    AVDictionary* options{nullptr};
    av_dict_set(&options, "rtsp_transport", "tcp", 0);

    int result;
    std::string rtsp_url{"rtsp://admin:abcd1234@192.169.8.153"};
    std::string output_file{"/tmp/output.mp4"};
    int duration{600};

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

int call_command_tool()
{
    std::stringstream cmd_stream;
    std::string body_str{R"({"sn": "ls20://0201EE5E4D31","type": "req","name": "songs_queue_list"})"};

    cmd_stream << "curl -X POST http://192.168.0.101:8888 --header 'Content-Type: application/json' --data-raw '" << body_str << "'";
    std::cout << "cmd_stream: " << cmd_stream.str();

    std::system(cmd_stream.str().c_str());

    return 0;
}

// 函数用于进行 base64 解码
unsigned char *base64_decode(const char *input, int length) {
    BIO *bio, *b64;
    unsigned char *buffer = (unsigned char *)malloc(length);
    memset(buffer, 0, length);

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new_mem_buf((void *)input, length);
    bio = BIO_push(b64, bio);

    BIO_read(bio, buffer, length);

    BIO_free_all(bio);

    return buffer;
}

int test_master_camera_get_rgb()
{
    CURL* curl;
    CURLcode res;
    std::string response;
    nlohmann::json body_data;

    body_data["id"] = "d9b040968baf4d57152f3a99b3dee3ed";

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:13001/api/camera/getRGB");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        struct curl_slist *headers{nullptr};
        headers = curl_slist_append(headers, "User-Agent: Apifox/1.0.0 (https://apifox.com)");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: */*");
        headers = curl_slist_append(headers, "Host: 127.0.0.1:13001");
        headers = curl_slist_append(headers, "Connection: keep-alive");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        const char *data = "{\n    \"id\": \"d9b040968baf4d57152f3a99b3dee3ed\"\n}";
        LOG(INFO) << body_data.dump().c_str() << "\n";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            LOG(ERROR) << "failed to perform request: " << curl_easy_strerror(res) << "\n";
        }
        LOG(INFO) << response << "\n";

        curl_easy_cleanup(curl);
    }

    nlohmann::json parsed_data = nlohmann::json::parse(response);
    std::string image_str = parsed_data["image"];
    
    auto data_ptr = base64_decode(image_str.c_str(), image_str.size());
    std::ofstream output_file("/tmp/aaa.jpg", std::ios::binary);
    output_file.write(reinterpret_cast<char *>(data_ptr), image_str.size());
    output_file.close();

    curl_global_cleanup();

    return 0;
}

int ffmpeg_record_to_jpg()
{
    int result;
    std::string rtsp_url{"rtsp://admin:abcd1234@192.169.8.153"};

    AVFormatContext* p_format_ctx = avformat_alloc_context();
    AVDictionary* options{nullptr};
    av_dict_set(&options, "rtsp_transport", "tcp", 0);

    avformat_network_init();
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

    int video_stream{-1};
    for (unsigned int i = 0; i < p_format_ctx->nb_streams; i++)
    {
        if (p_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream = i;
            break;
        }
    }
    if (video_stream == -1)
    {
        LOG(ERROR) << "no video stream found in the input file\n";
        return result;
    }

    AVCodecParameters* codec_parameters = p_format_ctx->streams[video_stream]->codecpar;
    AVCodec* p_codec = const_cast<AVCodec*>(avcodec_find_decoder(codec_parameters->codec_id));
    if (!p_codec)
    {
        LOG(ERROR) << "codec not found\n";
        return result;
    }

    AVCodecContext* p_codec_ctx = avcodec_alloc_context3(p_codec);
    avcodec_parameters_to_context(p_codec_ctx, codec_parameters);
    avcodec_open2(p_codec_ctx, p_codec, nullptr);

    AVFrame* p_frame = av_frame_alloc();
    int frame_count{0};
    AVPacket packet;
    while (av_read_frame(p_format_ctx, &packet) >= 0)
    {
        if (packet.stream_index == video_stream)
        {
            avcodec_send_packet(p_codec_ctx, &packet);
            avcodec_receive_frame(p_codec_ctx, p_frame);

            SwsContext* sws_ctx = sws_getContext(p_codec_ctx->width, p_codec_ctx->height, p_codec_ctx->pix_fmt, p_codec_ctx->width, p_codec_ctx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);
            AVFrame* rgb_frame = av_frame_alloc();
            uint8_t* rgb_buffer = new uint8_t[av_image_get_buffer_size(AV_PIX_FMT_RGB24, p_codec_ctx->width, p_codec_ctx->height, 1)];
            sws_scale(sws_ctx, p_frame->data, p_frame->linesize, 0, p_codec_ctx->height, rgb_frame->data, rgb_frame->linesize);
            sws_freeContext(sws_ctx);

            std::string file_name = "/tmp/frame_" + std::to_string(frame_count) + ".jpg";
            std::ofstream output_file(file_name, std::ios::binary);
            if (output_file.is_open())
            {
                AVCodec* jpeg_codec = const_cast<AVCodec*>(avcodec_find_encoder(AV_CODEC_ID_MJPEG));
                AVCodecContext* jpeg_codec_ctx = avcodec_alloc_context3(jpeg_codec);
                jpeg_codec_ctx->width = p_codec_ctx->width;
                jpeg_codec_ctx->height = p_codec_ctx->height;
                jpeg_codec_ctx->pix_fmt = AV_PIX_FMT_RGB24;
                avcodec_open2(jpeg_codec_ctx, jpeg_codec, nullptr);

                AVPacket jpeg_packet;

                int ret = avcodec_send_frame(jpeg_codec_ctx, rgb_frame);
                if (ret >= 0)
                {
                    ret = avcodec_receive_packet(jpeg_codec_ctx, &jpeg_packet);
                }

                if (ret >= 0)
                {
                    output_file.write(reinterpret_cast<char *>(jpeg_packet.data), jpeg_packet.size);
                }

                output_file.close();
                if (!jpeg_packet.buf)
                {
                    av_packet_unref(&jpeg_packet);
                }
                avcodec_free_context(&jpeg_codec_ctx);
            }

            av_frame_free(&rgb_frame);
            delete[] rgb_buffer;

            frame_count++;
        }

        av_packet_unref(&packet);
    }

    av_frame_free(&p_frame);
    avcodec_close(p_codec_ctx);
    avcodec_free_context(&p_codec_ctx);
    avformat_close_input(&p_format_ctx);

    return result;
}

int robot_pose_compute()
{
    const Eigen::Vector3d axis_x(1, 0, 0);
    const Eigen::Vector3d axis_y(0, 1, 0);
    const Eigen::Vector3d axis_z(0, 0, 1);
    std::vector<double> pose_offset{180, 0, 90};

    std::transform(pose_offset.begin(), pose_offset.end(), pose_offset.begin(), [](double val){return val * M_PI / 180.0;});

    Eigen::AngleAxisd angle_axisd_x(pose_offset.at(0), axis_x);
    Eigen::Matrix3d rotation_matrix_x = angle_axisd_x.matrix();

    Eigen::AngleAxisd angle_axisd_y(pose_offset.at(1), axis_y);
    Eigen::Matrix3d rotation_matrix_y = angle_axisd_y.matrix();

    Eigen::AngleAxisd angle_axisd_z(pose_offset.at(2), axis_z);
    Eigen::Matrix3d rotation_matrix_z = angle_axisd_z.matrix();

    Eigen::Matrix3d rotation_target = rotation_matrix_z * rotation_matrix_y * rotation_matrix_x;
    Eigen::AngleAxisd angle_axisd_target(rotation_target);
    
    Eigen::Vector3d rotate_vector_target = angle_axisd_target.angle() * angle_axisd_target.axis();

    LOG(INFO) << "rotate_vector: \n"
              << rotate_vector_target.x() << ", " 
              << rotate_vector_target.y() << ", " 
              << rotate_vector_target.z() << "\n" ;

    return 0;
}

int multi_images_to_video_in_opencv()
{
    const std::string dir{"/data/home/user/workspace/cpp_unit/data/images/"};
    const std::string output_video_file{"/tmp/bbb.mp4"};
    std::vector<std::string> image_path_container;

    try
    {
        if (boost::filesystem::exists(dir) && boost::filesystem::is_directory(dir))
        {
            for (const auto& entry : boost::filesystem::directory_iterator(dir))
            {
                if (boost::filesystem::is_regular_file(entry.status()))
                {
                    LOG(INFO) << "file: " << entry.path().filename() << "\n";
                    image_path_container.push_back(dir + entry.path().filename().string());
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

    std::vector<cv::Mat> image_container;
    int height, width;
    for (auto& it : image_path_container)
    {
        LOG(INFO) << "image: " << it << "\n";
        cv::Mat tmp_img = cv::imread(it);
        image_container.push_back(tmp_img);
        height = tmp_img.rows;
        width = tmp_img.cols;
    }

    cv::VideoWriter video_writer(output_video_file, cv::VideoWriter::fourcc('m','p','4','v'), 2, cv::Size(width, height));
    if (!video_writer.isOpened())
    {
        LOG(ERROR) << "could not open the output video file for writing\n";
        return -1;
    }

    for (auto& it : image_container)
    {
        video_writer.write(it);
    }

    LOG(INFO) << "video has been successfully create and saved as: " << output_video_file << "\n";

    return 0;
}

int thread_and_move()
{
    auto func = []()
    {
        for (int i = 0; i < 100; i++)
        {
            LOG(INFO) << "aaa message\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };

    std::thread thread1 = std::thread(func);

    std::thread thread2 = std::move(thread1);

    if (thread2.joinable())
    {
        thread2.join();
    }

    return 0;
}

int three_dimensionality_roi()
{
    std::vector<std::vector<double>> roi = 
    {
        {0, 0, 0},
        {5, 5, 5},
        {0, 5, 0},
        {5, 0, 0},
        {5, 5, 0},
        {0, 0, 5},
        {5, 0, 5},
        {0, 5, 5}
    };

    std::vector<double> target1 = {1, 1, 1};
    std::vector<double> target2 = {10, 10, 10};

    std::vector<double> x_container;
    std::vector<double> y_container;
    std::vector<double> z_container;
    
    std::transform(roi.begin(), roi.end(), std::back_inserter(x_container), [](std::vector<double> tmp){ return tmp.at(0);});
    std::transform(roi.begin(), roi.end(), std::back_inserter(y_container), [](std::vector<double> tmp){ return tmp.at(1);});
    std::transform(roi.begin(), roi.end(), std::back_inserter(z_container), [](std::vector<double> tmp){ return tmp.at(2);});

    std::sort(x_container.begin(), x_container.end());
    std::sort(y_container.begin(), y_container.end());
    std::sort(z_container.begin(), z_container.end());

    LOG(INFO) << "max point(x,y,z): " << x_container.front() << ", " << y_container.front() << ", " << z_container.front() << "\n"
              << "min point(x,y,z): " << x_container.back() << ", " << y_container.back() << ", " << z_container.back() << "\n";

    if (target1.at(0) < x_container.back() && target1.at(0) > x_container.front() &&
        target1.at(1) < y_container.back() && target1.at(1) > y_container.front() &&
        target1.at(2) < z_container.back() && target1.at(2) > z_container.front()
        )
    {
        LOG(INFO) << "the point: " << target1.at(0) << ", " << target1.at(1) << ", " << target1.at(2) << " is inside\n";
    }
    else 
    {
        LOG(ERROR) << "the point: " << target1.at(0) << ", " << target1.at(1) << ", " << target1.at(2) << " is not inside\n";
    }

    if (target2.at(0) < x_container.back() && target2.at(0) > x_container.front() &&
        target2.at(1) < y_container.back() && target2.at(1) > y_container.front() &&
        target2.at(2) < z_container.back() && target2.at(2) > z_container.front()
        )
    {
        LOG(INFO) << "the point: " << target2.at(0) << ", " << target2.at(1) << ", " << target2.at(2) << " is inside\n";
    }
    else 
    {
        LOG(ERROR) << "the point: " << target2.at(0) << ", " << target2.at(1) << ", " << target2.at(2) << " is not inside\n";
    }


    return 0;
}

int test_nlohmann_json_double_array()
{
    std::string data{R"(
{
            "argument_static_catch_grap_area": [
                [
                    -0.15,
                    -0.2,
                    0
                ],
                [
                    -0.15,
                    -0.6,
                    0
                ],
                [
                    0.45,
                    -0.2,
                    0.1
                ],
                [
                    0.45,
                    -0.6,
                    0.1
                ]
            ]
}
    )"};


    nlohmann::json parsed_data = nlohmann::json::parse(data);
    std::vector<std::vector<double>> area = parsed_data["argument_static_catch_grap_area"].get<std::vector<std::vector<double>>>();

    for (auto& it : area)
    {
        LOG(INFO) << it.at(0) << ", " << it.at(1) << ", " << it.at(2) << "\n";
    }

    return 0;
}

int sort_compare_function()
{
    std::vector<double> data_container{3, 1, 4, 2, 6, 234, 546, 1};

    LOG(INFO) << "初始的算法:\n";
    std::sort(data_container.begin(), data_container.end());
    for (auto& it : data_container)
    {
        LOG(INFO) << it << "\n";
    }

    LOG(INFO) << "大于号算法: \n";
    std::sort(data_container.begin(), data_container.end(), [](double a, double b){ return a > b;});
    for (auto& it : data_container)
    {
        LOG(INFO) << it << "\n";
    }

    LOG(INFO) << "小于号算法: \n";
    std::sort(data_container.begin(), data_container.end(), [](double a, double b){ return a < b;});
    for (auto& it : data_container)
    {
        LOG(INFO) << it << "\n";
    }

    LOG(INFO) << "反转算法: \n";
    std::sort(data_container.begin(), data_container.end());
    std::reverse(data_container.begin(), data_container.end());
    for (auto& it : data_container)
    {
        LOG(INFO) << it << "\n";
    }

    return 0;
}

int video_to_image_with_opencv()
{
    const std::string video_file{"/tmp/p8H2e-frame-309973.avi"};
    const std::string output_video_file{"/tmp/aaa.avi"};

    cv::VideoCapture cap(video_file);
    if (!cap.isOpened())
    {
        LOG(ERROR) << "error opening video file\n";
        return -1;
    }

    std::vector<cv::Mat> image_container;
    int height, width;
    while (true)
    {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
        {
            break;
        }
        height = frame.rows;
        width = frame.cols;
        image_container.push_back(frame);
    }

    height *= 0.5;
    width *= 0.5;
    for (auto& it : image_container)
    {
        cv::resize(it, it, cv::Size(width, height));
    }

    cv::VideoWriter video_writer(output_video_file, cv::VideoWriter::fourcc('M','J','P','G'), 1, cv::Size(width, height));
    if (!video_writer.isOpened())
    {
        LOG(ERROR) << "could not open the output video file for writing\n";
        return -1;
    }

    // 定义要添加的文本内容以及其他绘制参数
    std::string text = "hhhleave job event, leave time: 60(s)";
    cv::Point org(350, 25); // 文本的起始位置
    int fontFace = cv::FONT_HERSHEY_SIMPLEX; // 字体类型
    double fontScale = 0.5; // 字体缩放系数
    cv::Scalar color(0, 0, 255); // 文本颜色
    int thickness = 2; // 文本粗细

    for (auto& it : image_container)
    {
        cv::putText(it, text, org, fontFace, fontScale, color, thickness);
        cv::imshow("aaa", it);
        cv::waitKey(0);
        video_writer.write(it);
    }
    LOG(INFO) << "video has been successfully create and saved as: " << output_video_file << "\n";

    return 0;
}

int mkdtemp_command()
{
    char *path;
    char name[] = "/tmp/dirXXXXXX";
    path = mkdtemp(name);
    if (path == NULL)
    {
        perror("mkdtemp");
        return -1;
    }

    LOG(INFO) << path << "\n";
    rmdir(path);

    return 0;
}

int test_vector_copy_assign()
{
    std::vector<float> arr1{1, 2, 3, 4, 5, 6};
    std::vector<float> pose{123, 132, 4355};

    std::for_each(arr1.begin(), arr1.end(), [](float it){LOG(INFO) << it << "\n";});
    std::for_each(pose.begin(), pose.end(), [](float it){LOG(INFO) << it << "\n";});

    arr1.erase(arr1.begin() + 3, arr1.end());
    std::copy(pose.begin(), pose.end(), std::back_inserter(arr1));

    std::for_each(arr1.begin(), arr1.end(), [](float it){LOG(INFO) << it << "\n";});

    return 0;
}

int test_boost_thread_pool()
{
    boost::asio::thread_pool tp(4);

    for (int i = 0; i < 4; i++)
    {
        boost::asio::post(tp, [i](){
            LOG(INFO) << "Task: " << i << " executed in thread: " << std::this_thread::get_id() << "\n";
        });
    }

    auto tmp = tp.get_executor();

    tp.join();

    return 0;
}


int test_boost_asio_deadline_timer()
{
    boost::timer::auto_cpu_timer t;
    boost::asio::io_context io;

    boost::asio::deadline_timer timer(io, boost::posix_time::seconds(5));
    boost::system::error_code ec;
    // timer.wait(ec);
    timer.async_wait([](const boost::system::error_code&){
        LOG(INFO) << "Timer expired!\n";
    });

    std::size_t num_events = io.run_for(std::chrono::seconds(2));

    LOG(INFO) << "Handled " << num_events << " events.\n";

    return 0;
}

int test_interruptible_sleeper()
{
    class InterruptibleSleeper
    {
    public:
        bool wait_for(std::chrono::duration<double> const& time)
        {
            std::unique_lock<std::mutex> lock(m);
            return !cv.wait_for(lock, time, [&]{return terminate;});
        }

        void interrupt()
        {
            std::unique_lock<std::mutex> lock(m);
            terminate = true;
            cv.notify_all();
        }

    private:
        std::condition_variable cv;
        std::mutex m;
        bool terminate{false};
    };

    InterruptibleSleeper sleeper;

    auto tmp = [&sleeper](){
        while (true)
        {
            LOG(INFO) << "working\n";
            sleeper.wait_for(std::chrono::duration<double>(3));
            LOG(INFO) << "next loop\n";
        }
        LOG(INFO) << "clean up\n";
    };

    std::thread t(tmp);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    sleeper.interrupt();

    t.join();

    return 0;
}

// 定义告警结构体
struct Alarm {
    std::string message;
    std::string timestamp;

    // 重载小于运算符，用于告警的比较
    bool operator<(const Alarm& other) const {
        return std::tie(message, timestamp) < std::tie(other.message, other.timestamp);
    }
};

// 去重函数
void removeDuplicateAlarms(std::set<Alarm>& alarms) {
    // 无需额外处理，std::set会自动去除重复
}

int test_duplicate_alarms()
{
    std::set<Alarm> alarms;

    // 模拟添加告警
    alarms.insert({"Temperature too high", "2024-05-07 10:15:00"});
    alarms.insert({"Temperature too high", "2024-05-07 10:15:00"}); // 重复告警
    alarms.insert({"Connection lost", "2024-05-07 10:20:00"});
    alarms.insert({"Temperature too high", "2024-05-07 10:15:00"}); // 又一个重复告警
    alarms.insert({"Power failure", "2024-05-07 10:25:00"});

    // 去除重复告警
    removeDuplicateAlarms(alarms);

    // 打印去重后的告警
    std::cout << "Unique alarms:" << std::endl;
    for (const auto& alarm : alarms) {
        std::cout << alarm.message << " at " << alarm.timestamp << std::endl;
    }

    return 0;
}

int test_tracking_algorithm()
{
    namespace bg = boost::geometry;
    namespace bgi = boost::geometry::index;

    // 定义二维点
    typedef bg::model::point<double, 2, bg::cs::cartesian> point_type;

    // 定义运动目标结构体
    struct MotionTarget {
        int id; // 目标ID
        point_type position; // 当前位置
    };

    // 定义追踪器类
    class Tracker {
    private:
        std::vector<MotionTarget> targets; // 存储目标
        bgi::rtree<std::pair<point_type, int>, bgi::quadratic<16>> rtree; // R树，用于空间索引

    public:
        // 添加新目标
        void addTarget(const MotionTarget& target) {
            targets.push_back(target);
            rtree.insert({target.position, target.id});
        }

        // 更新目标位置
        void updateTargetPosition(int id, const point_type& newPosition) {
            auto it = std::find_if(targets.begin(), targets.end(), [id](const MotionTarget& t) { return t.id == id; });
            if (it != targets.end()) {
                it->position = newPosition;
                rtree.remove({it->position, it->id});
                rtree.insert({it->position, it->id});
            }
        }

        // 查询附近的目标
        std::vector<MotionTarget> queryNearbyTargets(const point_type& position, double radius) const {
            std::vector<MotionTarget> nearbyTargets;
            std::vector<std::pair<point_type, int>> result;
            rtree.query(bgi::nearest(position, radius), std::back_inserter(result));

            for (const auto& entry : result) {
                nearbyTargets.push_back(targets[entry.second]);
            }

            return nearbyTargets;
        }
    };

    Tracker tracker;

    // 添加一些目标
    tracker.addTarget({1, point_type(0, 0)});
    tracker.addTarget({2, point_type(5, 5)});
    tracker.addTarget({3, point_type(-3, 2)});

    // 更新目标位置
    tracker.updateTargetPosition(1, point_type(1, 1));
    tracker.updateTargetPosition(2, point_type(4, 4));

    // 查询附近的目标
    std::vector<MotionTarget> nearbyTargets = tracker.queryNearbyTargets(point_type(0, 0), 3.0);

    // 输出结果
    std::cout << "Nearby targets:" << std::endl;
    for (const auto& target : nearbyTargets) {
        std::cout << "ID: " << target.id << ", Position: (" << bg::get<0>(target.position) << ", "
                  << bg::get<1>(target.position) << ")" << std::endl;
    }

    return 0;
}

int test_pseudo_random_numbers()
{
    srand(time(NULL));

    for (int i = 0; i < 100; i++)
    {
        LOG(INFO) << rand() << "\n";
    }

    return 0;
}

int test_modern_random_numbers()
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::vector<int> number_container;

    std::uniform_int_distribution<int> dist(1, 10000);

    for (int i{0}; i < 1000; ++i)
    {
        number_container.push_back(dist(rng));
    }

    std::ofstream output_file("/tmp/aaa.txt", std::ios::out);
    for (auto& it : number_container)
    {
        output_file << it << "\n";
    }
    output_file.close();

    std::vector<int> unsort_numbers;
    std::ifstream input_file("/tmp/aaa.txt", std::ios::in);
    while (input_file.good())
    {
        int value;
        input_file >> value;
        LOG(INFO) << value << "\n";
        unsort_numbers.push_back(value);
    }
    std::sort(unsort_numbers.begin(), unsort_numbers.end());
    std::for_each(unsort_numbers.begin(), unsort_numbers.end(), [](int val){LOG(INFO) << val << "\n";});

    return 0;
}

int test_thread_id()
{
    std::thread::id master_thread;

    if (std::this_thread::get_id() == master_thread)
    {
        LOG(INFO) << "this is master thread\n";
    }

    return 0;
}

int test_glog_vector()
{
    std::vector<int> arr{1, 2, 3, 4};

    std::for_each(arr.begin(), arr.end(), [](int val){LOG(INFO) << val << ", ";});

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

    std::map<std::string, std::function<int()>> func_table =
    {
        {"abstract-factory", abstract_factory},
        {"parse-yaml-config", parse_yaml_config},
        {"class-with-thread", class_with_thread},
        {"class-delegating-constructor", class_delegating_constructor},
        {"file-to-memory", file_to_memory},
        {"list-files", list_files},
        {"list_all_mp4_file", list_all_mp4_file},
        {"request-token", request_token},
        {"httplib-request-token", httplib_request_token},
        {"httplib-upload-file", httplib_upload_file},
        {"httplib-push-record", httplib_push_record},
        {"httplib-upload-file-and-push-record", httplib_upload_file_and_push_record},
        {"get-year-month-day-hour-minute", get_year_month_day_hour_minute},
        {"get-save-path", get_save_path},
        {"http-server-with-mongoose", http_server_with_mongoose},
        {"test-son-call-parent-func", test_son_call_parent_func},
        {"decorator-sample", decorator_sample},
        {"test-task-use-service", test_task_use_service},
        {"token-and-push", token_and_push},
        {"opencv-sample", opencv_sample},
        {"curl-example", curl_example},
        {"ffmpeg-record-video", ffmpeg_record_video},
        {"test-master-camera-get-rgb", test_master_camera_get_rgb},
        {"ffmpeg-record-to-jpg", ffmpeg_record_to_jpg},
        {"robot-pose-compute", robot_pose_compute},
        {"multi-images-to-video-in-opencv", multi_images_to_video_in_opencv},
        {"thread-and-move", thread_and_move},
        {"three_dimensionality_roi", three_dimensionality_roi},
        {"test_nlohmann_json_double_array", test_nlohmann_json_double_array},
        {"sort_compare_function", sort_compare_function},
        {"video_to_image_with_opencv", video_to_image_with_opencv},
        {"mkdtemp_command", mkdtemp_command},
        {"test_vector_copy_assign", test_vector_copy_assign},
        {"test_boost_thread_pool", test_boost_thread_pool},
        {"test_boost_asio_deadline_timer", test_boost_asio_deadline_timer},
        {"test_interruptible_sleeper", test_interruptible_sleeper},
        {"test_duplicate_alarms", test_duplicate_alarms},
        {"test_tracking_algorithm", test_tracking_algorithm},
        {"test_pseudo_random_numbers", test_pseudo_random_numbers},
        {"test_modern_random_numbers", test_modern_random_numbers},
        {"test_thread_id", test_thread_id},
        {"test_glog_vector", test_glog_vector}
    };

    auto it = func_table.find(FLAGS_module);
    if (it != func_table.end())
    {
        it->second();
    }
    else 
    {
        LOG(ERROR) << "invalid argument: " << FLAGS_module << "\n";
    }

    google::ShutDownCommandLineFlags();
    google::ShutdownGoogleLogging();
    return 0;
}