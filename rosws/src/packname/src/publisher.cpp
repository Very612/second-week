#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std::chrono_literals;
class DataSender : public rclcpp::Node
{
public:
    DataSender() : Node("data_sender")
    {
        srand((unsigned int)time(NULL));
        pub_ = this->create_publisher<std_msgs::msg::Float64>("/raw_data", 10);
        timer_ = this->create_wall_timer(10ms, std::bind(&DataSender::publish_data, this));
        RCLCPP_INFO(this->get_logger(), "✅ 数据发送节点启动");
    }
private:
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    double t = 0.0;
    void publish_data()
    {
        double signal = 5.0 + 2.0 * sin(t);
        double noise = (rand() % 100) / 10.0 - 5.0;
        double raw_data = signal + noise;
        std_msgs::msg::Float64 msg;
        msg.data = raw_data;
        pub_->publish(msg);
        t += 0.01;
    }
};
int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<DataSender>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
