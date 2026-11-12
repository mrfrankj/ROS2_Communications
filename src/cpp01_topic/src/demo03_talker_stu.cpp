// 1. 包含头文件
#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/msg/student.hpp"

using base_interfaces_demo::msg::Student;
using namespace std::chrono_literals;

// 2. 自定义节点类
class TalkerStu: public rclcpp::Node
{
public:
  TalkerStu(): Node("talkerstu_node_cpp"), age(10)
  {
    RCLCPP_INFO(this->get_logger(), "发布节点创建.");
    // 创建发布者
    publisher_ = this->create_publisher<Student>("chatter_stu",10);
    // 创建定时器
    timer_ = this->create_wall_timer(500ms, std::bind(&TalkerStu::on_timer,this));
  }
private:
    void on_timer()
    {
        // 3-3. 创建学生消息对象并发布
        auto msg = Student();
        msg.name = "张三";
        msg.age = age++;
        msg.height = 1.75;
        RCLCPP_INFO(this->get_logger(), "发布的消息: name=%s, age=%d, height=%.2f", msg.name.c_str(), msg.age, msg.height);
        publisher_->publish(msg);
    }
    rclcpp::Publisher<Student>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    int age;
};


int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;

  // init
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<TalkerStu>());

  // release
  rclcpp::shutdown();

  return 0;
}

