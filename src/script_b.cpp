#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <string>
#include "httplib.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

struct SharedData {
    double rpm_left;
    double rpm_right;
    double linear_vel;
    double angular_vel;
    long timestamp;
};

class ScriptB
{
public:
    ScriptB()
    {
        // open shared memory created by Script A
        shm_fd = shm_open("/robot_data", O_RDONLY, 0666);
        shared = (SharedData*)mmap(0, sizeof(SharedData),
                 PROT_READ, MAP_SHARED, shm_fd, 0);

        ts_b = 0;
        std::cout << "Script B started" << std::endl;
    }

    void run()
    {
        // start http server in background
        std::thread t([this]() { startServer(); });
        t.detach();

        // main loop at 10 Hz
        while (true)
        {
            fetchAndPrint();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

private:
    int shm_fd;
    SharedData* shared;
    SharedData latest;
    long ts_b;

    void fetchAndPrint()
    {
        // copy from shared memory
        latest = *shared;

        // script B timestamp
        ts_b = std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch()).count();

        std::cout << "L_RPM: "  << latest.rpm_left
                  << " R_RPM: " << latest.rpm_right
                  << " V: "     << latest.linear_vel
                  << " W: "     << latest.angular_vel
                  << " T_A: "   << latest.timestamp
                  << " T_B: "   << ts_b
                  << std::endl;
    }

    void startServer()
    {
        httplib::Server svr;

        svr.Get("/get_data_from_B", [this](const httplib::Request&,
                                            httplib::Response& res)
        {
            json data;
            data["rpm_left"]    = latest.rpm_left;
            data["rpm_right"]   = latest.rpm_right;
            data["linear_vel"]  = latest.linear_vel;
            data["angular_vel"] = latest.angular_vel;
            data["timestamp_A"] = latest.timestamp;
            data["timestamp_B"] = ts_b;

            res.set_content(data.dump(), "application/json");
        });

        std::cout << "Server running on port 8080" << std::endl;
        svr.listen("0.0.0.0", 8080);
    }
};

int main()
{
    ScriptB node;
    node.run();
    return 0;
}