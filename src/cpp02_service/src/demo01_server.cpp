#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/srv/add_ints.hpp"

using base_interfaces_demo::srv::AddInts;
using namespace std::chrono_literals;

// 3. 自定义节点类
class AddIntsServer: public rclcpp::Node
{
public:
  AddIntsServer(): Node("addints_server_node_cpp")
  {
    RCLCPP_INFO(this->get_logger(), "服务节点创建.");
    service_ = this->create_service<AddInts>("add_ints",std::bind(&AddIntsServer::add, this, std::placeholders::_1, std::placeholders::_2));
  }
private:
  rclcpp::Service<AddInts>::SharedPtr service_;
  void add(const std::shared_ptr<AddInts::Request> request, std::shared_ptr<AddInts::Response> response)
  {
    RCLCPP_INFO(this->get_logger(), "请求的参数: num1=%d, num2=%d", request->num1, request->num2);
    response->sum = request->num1 + request->num2;
    RCLCPP_INFO(this->get_logger(), "响应的结果: sum=%d", response->sum);
  }
};

int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;

  // init
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<AddIntsServer>());

  // release
  rclcpp::shutdown();

  return 0;
}
