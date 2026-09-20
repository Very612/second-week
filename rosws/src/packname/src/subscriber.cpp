#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <vector>
class DataFilter : public rclcpp::Node
{
public:
    DataFilter() : Node("data_filter")
    {
        sub = this->create_subscription<std_msgs::msg::Float64>(
            "/raw_data", 10, std::bind(&DataFilter::cb, this, std::placeholders::_1));
        pub = this->create_publisher<std_msgs::msg::Float64>("/filtered_data", 10);
        RCLCPP_INFO(this->get_logger(), "滤波节点启动");
    }
private:
    void cb(const std_msgs::msg::Float64::SharedPtr msg)
    {
        double raw = msg->data;
        buf.push_back(raw);
        int win = 5;
        if (buf.size() > win) buf.erase(buf.begin());
        double sum = 0;
        for (int i = 0; i < buf.size(); i++) sum += buf[i];
        double out = sum / buf.size();
        RCLCPP_INFO(this->get_logger(), "raw: %.3f, filtered: %.3f", raw, out);
        auto res = std_msgs::msg::Float64();
        res.data = out;
        pub->publish(res);
    }
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr sub;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub;
    std::vector<double> buf;
};
int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DataFilter>());
    rclcpp::shutdown();
    return 0;
}
