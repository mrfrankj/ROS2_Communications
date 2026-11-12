#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

// self-defined class
class Listener: public rclcpp::Node
{
public:
  Listener(): Node("listener_node_cpp")
  {
    RCLCPP_INFO(this->get_logger(), "订阅节点创建.");
    // 创建订阅者
    subscription_ = this->create_subscription<std_msgs::msg::String>(
      "chatter", 10, std::bind(&Listener::topic_callback, this, std::placeholders::_1));
  }
private:
  void topic_callback(const std_msgs::msg::String::SharedPtr msg)
  {
    RCLCPP_INFO(this->get_logger(), "订阅的消息: '%s'", msg->data.c_str());
  }
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;

  // init
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<Listener>());

  // release
  rclcpp::shutdown();

  return 0;
}
