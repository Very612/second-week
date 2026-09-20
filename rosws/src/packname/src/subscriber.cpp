#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <vector>
#include <algorithm>

class DataFilter : public rclcpp::Node
{
public:
    DataFilter() : Node("data_filter"), last_lp(0)
    {
        sub = this->create_subscription<std_msgs::msg::Float64>(
            "/raw_data", 10, std::bind(&DataFilter::cb, this, std::placeholders::_1));
        pub_m = this->create_publisher<std_msgs::msg::Float64>("/filtered_mean", 10);
        pub_md = this->create_publisher<std_msgs::msg::Float64>("/filtered_median", 10);
        pub_lp = this->create_publisher<std_msgs::msg::Float64>("/filtered_lowpass", 10);
        win = 5;
        alpha = 0.2;
    }
private:
    void cb(const std_msgs::msg::Float64::SharedPtr msg)
    {
        double raw = msg->data;
        buf.push_back(raw);
        if (buf.size() > win) buf.erase(buf.begin());

        // 均值
        double m = raw;
        if (!buf.empty()) {
            double s = 0;
            for (double v : buf) s += v;
            m = s / buf.size();
        }

        // 中值
        double md = raw;
        if (!buf.empty()) {
            std::vector<double> tmp = buf;
            std::sort(tmp.begin(), tmp.end());
            md = tmp[tmp.size() / 2];
        }

        // 低通
        double lp = alpha * raw + (1 - alpha) * last_lp;
        last_lp = lp;

        auto mm = std_msgs::msg::Float64(); mm.data = m; pub_m->publish(mm);
        auto mmd = std_msgs::msg::Float64(); mmd.data = md; pub_md->publish(mmd);
        auto mlp = std_msgs::msg::Float64(); mlp.data = lp; pub_lp->publish(mlp);
    }

    int win;
    double alpha, last_lp;
    std::vector<double> buf;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr sub;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_m, pub_md, pub_lp;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DataFilter>());
    rclcpp::shutdown();
    return 0;
}
