#include <algorithm>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
using std::placeholders::_1;
using namespace std::chrono_literals;
class PidAdaptive : public rclcpp::Node
{
public:
    PidAdaptive() : Node("pid_adaptive"), tgt(0), act(0), integ(0), pe(0)
    {
        kp_min = 10.0;
        kp_max = 50.0;
        ki_min = 1.0;
        ki_max = 5.0;
        kd_min = 0.0;
        kd_max = 0.5;
        integ_limit = 20.0;
        out_limit = 10.0;
        dt = 0.002;
        tgt_sub = this->create_subscription<std_msgs::msg::Float64>(
            "/target_vel", 10, std::bind(&PidAdaptive::tgt_cb, this, std::placeholders::_1));
        act_sub = this->create_subscription<std_msgs::msg::Float64>(
            "/motor_velocity", 10, std::bind(&PidAdaptive::act_cb, this, std::placeholders::_1));
        pub = this->create_publisher<std_msgs::msg::Float64>("/command_torque", 10);
        timer = this->create_wall_timer(2ms, std::bind(&PidAdaptive::calc, this));
    }
private:
    void tgt_cb(const std_msgs::msg::Float64::SharedPtr msg) { tgt = msg->data; }
    void act_cb(const std_msgs::msg::Float64::SharedPtr msg) { act = msg->data; }
    void calc()
    {
        double e = tgt - act;
        double dv = (e - pe) / dt;
        pe = e;
        integ += e * dt;
        integ = std::clamp(integ, -integ_limit, integ_limit);
        // 误差大就Kp大，误差小就Ki大
        double er = std::clamp(std::abs(e) / 10.0, 0.0, 1.0);
        double kp = kp_min + (kp_max - kp_min) * er;
        double ki = ki_min + (ki_max - ki_min) * (1.0 - er);
        double kd = kd_min + (kd_max - kd_min) * std::clamp(std::abs(dv) / 100.0, 0.0, 1.0);
        double out = kp * e + ki * integ + kd * dv;
        out = std::clamp(out, -out_limit, out_limit);
        auto msg = std_msgs::msg::Float64();
        msg.data = out;
        pub->publish(msg);
        RCLCPP_INFO(this->get_logger(), "t:%.2f a:%.2f e:%.2f kp:%.2f ki:%.2f out:%.2f",
                    tgt, act, e, kp, ki, out);
    }

    double kp_min, kp_max, ki_min, ki_max, kd_min, kd_max;
    double integ_limit, out_limit, dt;
    double tgt, act, integ, pe;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr tgt_sub, act_sub;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub;
    rclcpp::TimerBase::SharedPtr timer;
};
int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PidAdaptive>());
    rclcpp::shutdown();
    return 0;
}
