#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <vector>
class DataReceiver : public rclcpp::Node
{
public:
    DataReceiver() : Node("data_receiver")
    {
        sub_ = this->create_subscription<std_msgs::msg::Float64>(
            "/raw_data",
            10,
            std::bind(
                &DataReceiver::on_data_callback,
                this,
                std::placeholders::_1
            )
        );
        pub_ = this->create_publisher<std_msgs::msg::Float64>(
            "/filtered_data",
            10
        );
        RCLCPP_INFO(
            this->get_logger(),
            "✅ 数据接收节点启动，等待数据..."
        );
    }
private:
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_;
    std::vector<double> buffer;
    void on_data_callback(
        const std_msgs::msg::Float64::SharedPtr msg)
    {
        double raw = msg->data;
        RCLCPP_INFO(
            this->get_logger(),
            "收到原始数据: %.3lf",
            raw
        );
        buffer.push_back(raw);
        const int win_size = 5;
        if (buffer.size() > win_size)
        {
            buffer.erase(buffer.begin());
        }
        double sum = 0.0;
        for (auto v : buffer)
        {
            sum += v;
        }
        double mean_filtered = sum / buffer.size();
        RCLCPP_INFO(
            this->get_logger(),
            "均值滤波输出: %.3lf",
            mean_filtered
        );
        std_msgs::msg::Float64 filtered_msg;
        filtered_msg.data = mean_filtered;
        pub_->publish(filtered_msg);
    }
};
int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<DataReceiver>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}