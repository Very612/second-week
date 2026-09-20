#include <memory>
#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
using namespace std::chrono_literals;
class TorquePub : public rclcpp::Node
{
public:
    TorquePub() : Node("torque_pub"), cnt(0)
    {
        pub = this->create_publisher<std_msgs::msg::Float64>("/command_torque", 10);
        timer = this->create_wall_timer(10ms, std::bind(&TorquePub::pub_torque, this));
    }
private:
    void pub_torque()
    {
        auto msg = std_msgs::msg::Float64();
        msg.data = 2.0 * sin(cnt);
        RCLCPP_INFO(this->get_logger(), "torque: %f", msg.data);
        pub->publish(msg);
        cnt++;
    }

    rclcpp::TimerBase::SharedPtr timer;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub;
    int cnt;
};
int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TorquePub>());
    rclcpp::shutdown();
    return 0;
}
