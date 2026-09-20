#include <algorithm>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
using std::placeholders::_1;
using namespace std::chrono_literals;
class MotorSim : public rclcpp::Node
{
public:
    MotorSim() : Node("motor_sim"), tau(0), w(0), th(0)
    {
        J = 0.01;
        B = 0.1;
        dt = 0.001;
        tau_limit = 10.0;
        w_limit = 100.0;
        sub = this->create_subscription<std_msgs::msg::Float64>("/command_torque", 10, std::bind(&MotorSim::tau_cb, this, _1));
        w_pub = this->create_publisher<std_msgs::msg::Float64>("/motor_velocity", 10);
        th_pub = this->create_publisher<std_msgs::msg::Float64>("/motor_position", 10);
        timer = this->create_wall_timer(1ms, std::bind(&MotorSim::step, this));
    }
private:
    void tau_cb(const std_msgs::msg::Float64::SharedPtr msg)
    {
        tau = std::clamp(msg->data, -tau_limit, tau_limit);
    }
    void step()
    {
        double alpha = (tau - B * w) / J;
        w += alpha * dt;
        th += w * dt;
        w = std::clamp(w, -w_limit, w_limit);
        auto w_msg = std_msgs::msg::Float64();
        w_msg.data = w;
        w_pub->publish(w_msg);
        auto th_msg = std_msgs::msg::Float64();
        th_msg.data = th;
        th_pub->publish(th_msg);
        RCLCPP_INFO(this->get_logger(), "tau:%.3f w:%.3f th:%.3f", tau, w, th);
    }
    double tau, w, th;
    double J, B, dt;
    double tau_limit, w_limit;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr sub;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr w_pub, th_pub;
    rclcpp::TimerBase::SharedPtr timer;
};
int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotorSim>());
    rclcpp::shutdown();
    return 0;
}
