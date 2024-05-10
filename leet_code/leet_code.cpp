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

/**
输入：plants = [2,2,3,3], capacityA = 5, capacityB = 5
输出：1
解释：
- 最初，Alice 和 Bob 的水罐中各有 5 单元水。
- Alice 给植物 0 浇水，Bob 给植物 3 浇水。
- Alice 和 Bob 现在分别剩下 3 单元和 2 单元水。
- Alice 有足够的水给植物 1 ，所以她直接浇水。Bob 的水不够给植物 2 ，所以他先重新装满水，再浇水。
所以，两人浇灌所有植物过程中重新灌满水罐的次数 = 0 + 0 + 1 + 0 = 1 。
*/
int code_2105()
{
    int capacityA{2};
    int capacityB{2};
    std::vector<int> plants{2, 1, 1};

    int res{0};
    int n = plants.size();
    int posa{0};
    int posb{n - 1};
    int vala{capacityA};
    int valb{capacityB};

    while (posa < posb)
    {
        if (vala < plants[posa])
        {
            ++res;
            vala = capacityA - plants[posa];
        }
        else 
        {
            vala -= plants[posa];
        }
        ++posa;
        if (valb < plants[posb])
        {
            ++res;
            valb = capacityB - plants[posb];
        }
        else 
        {
            valb -= plants[posb];
        }
        --posb;
    }

    if (posa == posb)
    {
        if (vala >= valb && vala < plants[posa])
        {
            ++res;
        }
        if (vala < valb && valb < plants[posb])
        {
            ++res;
        }
    }

    LOG(INFO) << "counter: " << res << "\n";

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
        {"code_2105", code_2105}
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