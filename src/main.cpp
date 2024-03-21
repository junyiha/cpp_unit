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

    auto timestamp = std::chrono::system_clock::to_time_t(now);

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

    cv::rectangle(img, top_left, bottom_right, cv::Scalar(0, 255, 0), 2);

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
    else if (FLAGS_module == "httplib-request-token")
    {
        httplib_request_token();
    }
    else if (FLAGS_module == "httplib-upload-file")
    {
        httplib_upload_file();
    }
    else if (FLAGS_module == "httplib-push-record")
    {
        httplib_push_record();
    }
    else if (FLAGS_module == "httplib-upload-file-and-push-record")
    {
        httplib_upload_file_and_push_record();
    }
    else if (FLAGS_module == "get-year-month-day-hour-minute")
    {
        get_year_month_day_hour_minute();
    }
    else if (FLAGS_module == "get-save-path")
    {
        get_save_path();
    }
    else if (FLAGS_module == "http-server-with-mongoose")
    {
        http_server_with_mongoose();
    }
    else if (FLAGS_module == "test-son-call-parent-func")
    {
        test_son_call_parent_func();
    }
    else if (FLAGS_module == "decorator-sample")
    {
        decorator_sample();
    }
    else if (FLAGS_module == "test-task-use-service")
    {
        test_task_use_service();
    }
    else if (FLAGS_module == "token-and-push")
    {
        token_and_push();
    }
    else if (FLAGS_module == "opencv-sample")
    {
        opencv_sample();
    }
    else 
    {
        LOG(ERROR) << "invalid module argument: " << FLAGS_module << "\n";
    }

    google::ShutDownCommandLineFlags();
    google::ShutdownGoogleLogging();
    return 0;
}