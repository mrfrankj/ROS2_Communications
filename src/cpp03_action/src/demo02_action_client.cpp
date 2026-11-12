#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "base_interfaces_demo/action/progress.hpp"

using base_interfaces_demo::action::Progress;
using namespace std::chrono_literals;

// 3. 自定义节点类
class ProgressActionClient: public rclcpp::Node
{
public:
  ProgressActionClient(): Node("progress_action_client_node_cpp")
  {
    RCLCPP_INFO(this->get_logger(), "action客户端节点创建.");
    client_ = rclcpp_action::create_client<Progress>(this, "get_sum");

  }
/* 发生请求 */
  void send_goal(int num)
  {
    /* 连接服务端 */
    if(!client_->wait_for_action_server(5s))
    {
      RCLCPP_ERROR(this->get_logger(), "action服务连接失败!");
      return;
    }
    /* 发送具体请求 */
    auto goal_msg = Progress::Goal();
    goal_msg.num = num;
    RCLCPP_INFO(this->get_logger(), "发送目标: num=%d", goal_msg.num);
    rclcpp_action::Client<Progress>::SendGoalOptions options;
    options.goal_response_callback = std::bind(&ProgressActionClient::goal_response_callback, this, std::placeholders::_1);
    options.feedback_callback = std::bind(&ProgressActionClient::feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
    options.result_callback = std::bind(&ProgressActionClient::result_callback, this, std::placeholders::_1);
    auto future = client_->async_send_goal(goal_msg, options);
  }

    void goal_response_callback(rclcpp_action::ClientGoalHandle<Progress>::SharedPtr goal_handle)
    {
        if(!goal_handle)
        {
            RCLCPP_ERROR(this->get_logger(), "目标请求被服务端拒绝.");
        }
        else
        {
            RCLCPP_INFO(this->get_logger(), "目标处理中！");
        }

    }
    void feedback_callback(rclcpp_action::ClientGoalHandle<Progress>::SharedPtr goal_handle, const std::shared_ptr<const Progress::Feedback> feedback)
    {
      double progress = (double)feedback->progress;
      RCLCPP_INFO(this->get_logger(), "当前进度：%.2f", progress);
    }
    void result_callback(const rclcpp_action::ClientGoalHandle<Progress>::WrappedResult & result)
    {
        if(result.code == rclcpp_action::ResultCode::SUCCEEDED)
        {
            RCLCPP_INFO(this->get_logger(), "最终结果: sum=%d", result.result->sum);
        }
        else if(result.code == rclcpp_action::ResultCode::ABORTED)
        {
            RCLCPP_INFO(this->get_logger(), "目标被中止.");
        }
        else if(result.code == rclcpp_action::ResultCode::CANCELED)
        {
            RCLCPP_INFO(this->get_logger(), "目标被取消.");
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(), "未知异常.");
        }
    }

private:
    rclcpp_action::Client<Progress>::SharedPtr client_;
};

int main(int argc, char ** argv)
{
  if(argc != 2)
  {
      RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "参数个数不正确! 需要一个整数参数.");
      return 1;
  }

  // init
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ProgressActionClient>();
  node->send_goal(std::stoi(argv[1]));
  rclcpp::spin(node);

  // release
  rclcpp::shutdown();

  return 0;
}
