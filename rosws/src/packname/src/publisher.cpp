#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std::chrono_literals;
class DataGen : public rclcpp::Node
{
public:
    DataGen() : Node("data_gen"), t(0)
    {
        srand(time(0));
        pub = this->create_publisher<std_msgs::msg::Float64>("/raw_data", 10);
        timer = this->create_wall_timer(10ms, std::bind(&DataGen::send, this));
        RCLCPP_INFO(this->get_logger(), "发数据的节点启动了");
    }
private:
    void send()
    {
        // 5 + 2*sin(t) 再加点噪声
        double sig = 5 + 2 * sin(t);
        double noise = (rand() % 100) / 10.0 - 5;
        auto msg = std_msgs::msg::Float64();
        msg.data = sig + noise;
        pub->publish(msg);
        t += 0.01;
    }
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub;
    rclcpp::TimerBase::SharedPtr timer;
    double t;
};
int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DataGen>());
    rclcpp::shutdown();
    return 0;
}

