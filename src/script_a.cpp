#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <sys/mman.h>
#include <fcntl.h>
#include <chrono>
#include <cmath>

#define WHEEL_DIST 0.443
#define WHEEL_DIA  0.181

struct SharedData {
    double rpm_left;
    double rpm_right;
    double linear_vel;
    double angular_vel;
    long timestamp;
};

class DiffDriveNode : public rclcpp::Node
{
public:
    DiffDriveNode() : Node("diff_drive_node")
    {
        // setup shared memory
        shm_fd = shm_open("/robot_data", O_CREAT | O_RDWR, 0666);
        ftruncate(shm_fd, sizeof(SharedData));
        shared = (SharedData*)mmap(0, sizeof(SharedData),
                  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        sub = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel", 10,
            std::bind(&DiffDriveNode::callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "Script A started");
    }

private:
    int shm_fd;
    SharedData* shared;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub;

    void callback(const geometry_msgs::msg::Twist::SharedPtr msg)
    {
        double V = msg->linear.x;
        double W = msg->angular.z;

        // differential drive kinematics
        double v_left  = V - (W * WHEEL_DIST / 2.0);
        double v_right = V + (W * WHEEL_DIST / 2.0);

        // convert to RPM
        double rpm_left  = (v_left  / (M_PI * WHEEL_DIA)) * 60.0;
        double rpm_right = (v_right / (M_PI * WHEEL_DIA)) * 60.0;

        // get timestamp
        auto now = std::chrono::system_clock::now();
        long ts = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()).count();

        // write to shared memory
        shared->rpm_left    = rpm_left;
        shared->rpm_right   = rpm_right;
        shared->linear_vel  = V;
        shared->angular_vel = W;
        shared->timestamp   = ts;

        RCLCPP_INFO(this->get_logger(),
            "L: %.2f | R: %.2f | V: %.2f | W: %.2f | T: %ld",
            rpm_left, rpm_right, V, W, ts);
    }
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DiffDriveNode>());
    rclcpp::shutdown();
    return 0;
}