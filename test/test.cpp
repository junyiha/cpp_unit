#include <iostream>
#include <sstream>
#include <string>

extern "C"
{
    #include "curl/curl.h"
}

// 回调函数，用于处理收到的数据
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *stream) {
    size_t total_size = size * nmemb;
    stream->append((char *)contents, total_size);
    return total_size;
}

int AppendMedia()
{
    CURL* curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.101:8888/");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, R"({
                        "sn": "ls20://0201EE5E4D31",
                        "type": "req",
                        "name": "songs_queue_append",
                            "params": {
                                "tid": "234",
                                "vol": 50,
                                "urls": [
                                    {
                                        "name": "helmet.mp3",
                                        "uri": "http://192.168.0.64:13000/helmet_detect_media.mp3"
                                    }
                                ]
                            }
                            })");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "failed to perform request: " << curl_easy_strerror(res) << "\n";
        }
        std::cout << response << "\n";

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}

int GetMediaList()
{
    CURL* curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.101:8888/");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, R"({
                        "sn": "ls20://0201EE5E4D31",
                        "type": "req",
                        "name": "songs_queue_list"
                        })");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "failed to perform request: " << curl_easy_strerror(res) << "\n";
        }
        std::cout << response << "\n";

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}

int ClearMedia()
{
    CURL* curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.101:8888/");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, R"({
                        "sn": "ls20://0201EE5E4D31",
                        "type": "req",
                        "name": "songs_queue_clear"
                        })");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "failed to perform request: " << curl_easy_strerror(res) << "\n";
        }
        std::cout << response << "\n";

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}

class Tool 
{
public:
    int AppendMedia()
    {
        CURL* curl;
        CURLcode res;
        std::string response;

        curl_global_init(CURL_GLOBAL_DEFAULT);

        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.101:8888");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, R"({
                            "sn": "ls20://0201EE5E4D31",
                            "type": "req",
                            "name": "songs_queue_append",
                                "params": {
                                    "tid": "234",
                                    "vol": 50,
                                    "urls": [
                                        {
                                            "name": "helmet.mp3",
                                            "uri": "http://192.168.0.64:13000/helmet_detect_media.mp3"
                                        }
                                    ]
                                }
                                })");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackFunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                std::cerr << "failed to perform request: " << curl_easy_strerror(res) << "\n";
            }
            std::cout << response << "\n";

            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();

        return 0;
    }

    int GetMediaList()
    {
        CURL* curl;
        CURLcode res;
        std::string response;

        curl_global_init(CURL_GLOBAL_DEFAULT);

        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.101:8888");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, R"({
                            "sn": "ls20://0201EE5E4D31",
                            "type": "req",
                            "name": "songs_queue_list"
                            })");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackFunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                std::cerr << "failed to perform request: " << curl_easy_strerror(res) << "\n";
            }
            std::cout << response << "\n";

            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();

        return 0;
    }

    int ClearMedia()
    {
        CURL* curl;
        CURLcode res;
        std::string response;

        curl_global_init(CURL_GLOBAL_DEFAULT);

        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.101:8888");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, R"({
                            "sn": "ls20://0201EE5E4D31",
                            "type": "req",
                            "name": "songs_queue_clear"
                            })");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackFunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                std::cerr << "failed to perform request: " << curl_easy_strerror(res) << "\n";
            }
            std::cout << response << "\n";

            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();

        return 0;
    }

private:
    // 回调函数，用于处理收到的数据
    static size_t WriteCallbackFunc(void *contents, size_t size, size_t nmemb, std::string *stream) 
    {
        size_t total_size = size * nmemb;
        stream->append((char *)contents, total_size);
        return total_size;
    }
};


int call_command_tool()
{
    std::stringstream cmd_stream;
    std::string body_str{R"({"sn": "ls20://0201EE5E4D31","type": "req","name": "songs_queue_list"})"};

    cmd_stream << "curl -X POST http://192.168.0.101:8888 --header 'Content-Type: application/json' --data-raw '" << body_str << "'";
    std::cout << "cmd_stream: " << cmd_stream.str();

    std::system(cmd_stream.str().c_str());

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        std::cerr << "invalid argument\n";
        return -1;
    }
    Tool tool;

    std::string cmd = argv[1];

    if (cmd == "append")
    {
        tool.AppendMedia();
    }
    else if (cmd == "list")
    {
        tool.GetMediaList();
    }
    else if (cmd == "clear")
    {
        tool.ClearMedia();
    }
    else if (cmd == "call-cmd")
    {
        call_command_tool();
    }
    else 
    {
        std::cerr << "invalid command: " << cmd << "\n";
        return -1;
    }

    return 0;
}