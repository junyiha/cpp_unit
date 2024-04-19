/**
 * @file network.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "protocol.hpp"
#include "zmq.hpp"

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

int get_video_file_with_httplib()
{
    httplib::Client cli("http://127.0.0.1:8000");

    auto res = cli.Get("/api/setting/warning_record/view?id=11334");
    if (res)
    {
        std::ofstream output_file("/tmp/bbb.avi", std::ios::binary);
        output_file.write(res->body.data(), res->body.size());
        output_file.close();
    }
    else 
    {
        LOG(ERROR) << "request failed\n";
    }

    return 0;
}

int input_id_and_get_video_file_with_httplib(Protocol::Message& message)
{
    httplib::Client cli("http://127.0.0.1:8000");

    std::string path = "/api/setting/warning_record/view?id=";
    path += std::to_string(message.id);

    auto res = cli.Get(path);
    if (res)
    {
        std::ofstream output_file("/tmp/bbb.avi", std::ios::binary);
        output_file.write(res->body.data(), res->body.size());
        output_file.close();
    }
    else 
    {
        LOG(ERROR) << "request failed\n";
    }

    return 0;
}

int get_base64_save_image(Protocol::Message& message)
{
    httplib::Client cli("http://192.169.5.63:8000");

    std::string path = "/api/setting/warning_record/info";

    nlohmann::json request_data;
    request_data["id"] = message.id;

    auto res = cli.Post(path, request_data.dump(), "Content-Type: application/json");
    if (res)
    {
        nlohmann::json parsed_data = nlohmann::json::parse(res->body);
        std::string img_str = parsed_data["image"];

        auto buffer = base64_decode(img_str.data(), img_str.size());
        std::ofstream output_file("/tmp/aba.jpg", std::ios::binary);
        output_file.write(reinterpret_cast<char *>(buffer), img_str.size());
        output_file.close();
    }
    else 
    {
        LOG(ERROR) << "request failed\n";
    }

    return 0;
}

int mongoose_minimal_static_server(Protocol::Message& message)
{
    class EventDispatcher
    {

    private:
        void Dispather(struct mg_connection* c, int ev, struct mg_http_message* hm)
        {
            if (mg_http_match_uri(hm, "/"))
            {
                StaticServer(c, ev, hm);
            }
            else if (mg_http_match_uri(hm, "/api/hello"))
            {
                mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "Hello, %s\n", "world");
            }
            else if (mg_http_match_uri(hm, "/api/image"))
            {
                Image(c, ev, hm);
            }
            else if (mg_http_match_uri(hm, "/api/video"))
            {
                Video(c, ev, hm);
            }
            else if (mg_http_match_uri(hm, "/api/file"))
            {
                File(c, ev, hm);
            }
            else if (mg_http_match_uri(hm, "/api/zipFile"))
            {
                ZipFile(c, ev, hm);
            }
            else 
            {
                mg_http_reply(c, 404, "Content-Type: text/plain\r\n", "uri not found");
            }
        }

        void StaticServer(struct mg_connection* c, int ev, struct mg_http_message* hm)
        {
            struct mg_http_serve_opts opts = {.root_dir = "/tmp"};
            mg_http_serve_dir(c, hm, &opts);
        }

        void Image(struct mg_connection* c, int ev, struct mg_http_message* hm)
        {
            const std::string path{"/home/user/Pictures/wallhaven-4dqdrm_1920x1080.png"};

            struct mg_str data;
            data.ptr = mg_file_read(&mg_fs_posix, path.c_str(), &(data.len));
            mg_printf(c, "%s", "HTTP/1.0 200 OK\r\nCache-Control: no-cache\r\nParagma: no-cache\r\nExpires: Thu, 01 Dec 1994 16:00:00 GMT\r\nContent-Type: multipart/x-mixed-replace; boundary=--foo\r\n\r\n");
            mg_printf(c, "--foo\r\nContent-Type: image/png\r\nContent-Length: %lu\r\n\r\n", data.len);
            mg_send(c, data.ptr, data.len);
            mg_send(c, "\r\n", 2);
            c->is_draining = 1;
            free((void*)data.ptr);
        }

        void Video(struct mg_connection* c, int ev, struct mg_http_message* hm)
        {
            const std::string path{"/home/user/Pictures/view"};
            struct mg_str data;
            data.ptr = mg_file_read(&mg_fs_posix, path.c_str(), &(data.len));
            mg_printf(c, "%s", "HTTP/1.0 200 OK\r\nTransfer-Encoding:chunked\r\n\r\n");
            mg_http_write_chunk(c, data.ptr, data.len);
            mg_send(c, "", 0);
            LOG(INFO) << "data.len: " << data.len << "\n";
            c->is_draining = 1;
            free((void*)data.ptr);
        }

        void File(struct mg_connection* c, int ev, struct mg_http_message* hm)
        {
            const std::string path{"/home/user/Downloads/张俊意的体检报告.pdf"};
            struct mg_str data;
            data.ptr = mg_file_read(&mg_fs_posix, path.c_str(), &(data.len));
            mg_printf(c, "%s", "HTTP/1.0 200 OK\r\nCache-Control: no-cache\r\nParagma: no-cache\r\nExpires: Thu, 01 Dec 1994 16:00:00 GMT\r\nContent-Type: application/pdf\r\n\r\n");
            mg_printf(c, "Content-Length: %lu\r\n\r\n", data.len);
            mg_send(c, data.ptr, data.len);
            mg_send(c, "\r\n", 2);
            LOG(INFO) << "data.len: " << data.len << "\n";
            c->is_draining = 1;
            free((void*)data.ptr);
        }

        void ZipFile(struct mg_connection* c, int ev, struct mg_http_message* hm)
        {
            const std::string path{"/home/user/Downloads/dist.zip"};
            struct mg_str data;
            data.ptr = mg_file_read(&mg_fs_posix, path.c_str(), &(data.len));
            mg_printf(c, "%s", "HTTP/1.0 200 OK\r\nCache-Control: no-cache\r\nParagma: no-cache\r\nExpires: Thu, 01 Dec 1994 16:00:00 GMT\r\nContent-Type: application/octet-stream\r\n\r\n");
            mg_printf(c, "Content-Length: %lu\r\n\r\n", data.len);
            mg_send(c, data.ptr, data.len);
            mg_send(c, "\r\n", 2);
            LOG(INFO) << "data.len: " << data.len << "\n";
            c->is_draining = 1;
            free((void*)data.ptr);
        }

    public:
        static void EventHandler (struct mg_connection* c, int ev, void* ev_data, void* fn_data)
        {
            if (ev == MG_EV_HTTP_MSG)
            {
                EventDispatcher* event_dispather_ptr = static_cast<EventDispatcher *>(fn_data);
                event_dispather_ptr->Dispather(c, ev, (struct mg_http_message *)ev_data);
            }
        };
    };
    
    EventDispatcher event_dispatcher;
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    mg_http_listen(&mgr, "http://localhost:8001", (mg_event_handler_t)EventDispatcher::EventHandler, std::addressof(event_dispatcher));

    for (;;)
    {
        mg_mgr_poll(&mgr, 1000);
    }

    mg_mgr_free(&mgr);

    return 0;
}

int test_zmq_producer(Protocol::Message& message)
{
    zmq::context_t context(1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    
    publisher.bind("tcp://*:5556");

    std::string msg{"Hello, ZeroMQ!"};
    while (true)
    {
        publisher.send(zmq::buffer(msg), zmq::send_flags::none);
        LOG(INFO) << "Send: " << msg << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

int test_zmq_consumer(Protocol::Message& message)
{
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5556");
    subscriber.set(zmq::sockopt::subscribe, "");

    while (true)
    {
        zmq::message_t message;
        zmq::recv_result_t recv_result = subscriber.recv(message);
        if (recv_result.has_value())
        {
            LOG(INFO) << "value: " << recv_result.value() << "\n";
        }
        else 
        {
            LOG(INFO) << "no value assigned\n";
        }
        std::string msg = std::string(static_cast<char *>(message.data()), message.size());
        LOG(INFO) << "Received: " << msg << "\n";
    }

    return 0;
}

int test_zmq_consumer_2(Protocol::Message& message)
{
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5556");
    subscriber.set(zmq::sockopt::subscribe, "");

    while (true)
    {
        zmq::message_t message;
        zmq::recv_result_t recv_result = subscriber.recv(message);
        if (recv_result.has_value())
        {
            LOG(INFO) << "value: " << recv_result.value() << "\n";
        }
        else 
        {
            LOG(INFO) << "no value assigned\n";
        }
        std::string msg = std::string(static_cast<char *>(message.data()), message.size());
        LOG(INFO) << "Received: " << msg << "\n";
    }

    return 0;
}

DEFINE_string(module, "design", "module layer");
DEFINE_int32(id, 13322, "module layer");

int main(int argc, char* argv[])
{
    InitGlog(argv[0]);
    gflags::SetUsageMessage("network unit project!");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    LOG(INFO) << "--network unit--" << std::endl;
    LOG(INFO) << gflags::ProgramUsage() << std::endl;

    std::map<std::string, std::function<int()>> func_table = 
    {
        {"request-token", request_token},
        {"httplib-request-token", httplib_request_token},
        {"httplib-upload-file", httplib_upload_file},
        {"httplib-push-record", httplib_push_record},
        {"httplib-upload-file-and-push-record", httplib_upload_file_and_push_record},
        {"http-server-with-mongoose", http_server_with_mongoose},
        {"token-and-push", token_and_push},
        {"curl-example", curl_example},
        {"get-video-file-with-httplib", get_video_file_with_httplib}
    };

    std::map<std::string, std::function<int(Protocol::Message&)>> func_with_argument_table = 
    {
        {"input_id_and_get_video_file_with_httplib", input_id_and_get_video_file_with_httplib},
        {"get_base64_save_image", get_base64_save_image},
        {"mongoose_minimal_static_server", mongoose_minimal_static_server},
        {"test_zmq_producer", test_zmq_producer},
        {"test_zmq_consumer", test_zmq_consumer},
        {"test_zmq_consumer_2", test_zmq_consumer_2}
    };

    Protocol::Message message;

    message.id = FLAGS_id;

    auto it = func_table.find(FLAGS_module);
    if (it != func_table.end())
    {
        LOG(INFO) << "function: " << it->first << "\n";
        it->second();
    }

    auto tmp_it = func_with_argument_table.find(FLAGS_module);
    if (tmp_it != func_with_argument_table.end())
    {
        LOG(INFO) << "function: " << tmp_it->first << "\n";
        tmp_it->second(message);
    }

    google::ShutDownCommandLineFlags();
    google::ShutdownGoogleLogging();

    return 0;
}