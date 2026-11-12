#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

// self-defined class
class Talker: public rclcpp::Node
{
public:
  Talker(): Node("talker_node_cpp")
  {
    RCLCPP_INFO(this->get_logger(), "发布节点创建.");
    // 创建发布者
    publisher_ = this->create_publisher<std_msgs::msg::String>("chatter", 10);
    // 创建定时器
    timer_ = this->create_wall_timer(1000ms, std::bind(&Talker::timer_callback, this));
  }
private:
  void timer_callback()
  {
    auto msg = std_msgs::msg::String();
    msg.data = "Hello, ROS2 C++, " + std::to_string(count++);
    RCLCPP_INFO(this->get_logger(), "发布的消息: '%s'", msg.data.c_str());
    publisher_->publish(msg);
  }
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  size_t count = 0;
};

int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;

  // init
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<Talker>());

  // release
  rclcpp::shutdown();

  return 0;
}
