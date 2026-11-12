// 1. 包含头文件
#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/msg/student.hpp"

using base_interfaces_demo::msg::Student;
using namespace std::chrono_literals;

// 2. 自定义节点类
class ListenerStu: public rclcpp::Node
{
public:
  ListenerStu(): Node("listenerstu_node_cpp")
  {
    RCLCPP_INFO(this->get_logger(), "监听节点创建.");
    // 创建订阅者
    subscription_ = this->create_subscription<Student>(
      "chatter_stu", 10, std::bind(&ListenerStu::do_cb, this, std::placeholders::_1));
  }
private:
void do_cb(const Student::SharedPtr msg)
{
    RCLCPP_INFO(this->get_logger(), "订阅的消息: name=%s, age=%d, height=%.2f", msg->name.c_str(), msg->age, msg->height);
}
rclcpp::Subscription<Student>::SharedPtr subscription_;
};


int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;

  // init
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<ListenerStu>());

  // release
  rclcpp::shutdown();

  return 0;
}

