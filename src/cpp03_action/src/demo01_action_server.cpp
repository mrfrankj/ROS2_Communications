#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "base_interfaces_demo/action/progress.hpp"

using base_interfaces_demo::action::Progress;
using namespace std::chrono_literals;

// 3. 自定义节点类
class ProgressActionServer: public rclcpp::Node
{
public:
  ProgressActionServer(): Node("progress_action_server_node_cpp")
  {
    RCLCPP_INFO(this->get_logger(), "action服务节点创建.");
    /*
    
    */
   server_ = rclcpp_action::create_server<Progress>(
      this,
      "get_sum",
      std::bind(&ProgressActionServer::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
      std::bind(&ProgressActionServer::handle_cancel, this, std::placeholders::_1),
      std::bind(&ProgressActionServer::handle_accepted, this, std::placeholders::_1)
    );

  }

  /* 处理提交的目标值 */
  rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID & uuid, std::shared_ptr<const Progress::Goal> goal)
  {
    RCLCPP_INFO(this->get_logger(), "接收到新的目标: num=%d", goal->num);
    if(goal->num < 1)
    {
      RCLCPP_WARN(this->get_logger(), "目标值非法, 拒绝执行.");
      return rclcpp_action::GoalResponse::REJECT;
    }

    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  /* 处理取消目标请求 */
  rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<rclcpp_action::ServerGoalHandle<Progress>> goal_handle)
  {
    RCLCPP_INFO(this->get_logger(), "接收到取消目标请求.");


    return rclcpp_action::CancelResponse::ACCEPT;
  }

  /* 生成连续反馈与最终响应(回调函数) */
  void execute(const std::shared_ptr<rclcpp_action::ServerGoalHandle<Progress>> goal_handle)
  {
    int num = goal_handle->get_goal()->num;
    int sum = 0;
    auto feedback = std::make_shared<Progress::Feedback>();
    auto result = std::make_shared<Progress::Result>();
    rclcpp::Rate loop_rate(1s);
    for(int i=0; i<=num; i++)
    {
      sum += i;

      feedback->progress = (i)/static_cast<float>(num);
      goal_handle->publish_feedback(feedback);

      /* 取消处理 */
      if (goal_handle->is_canceling())
      {
        result->sum = sum;
        goal_handle->canceled(result);
        RCLCPP_INFO(this->get_logger(), "目标被取消, 已计算的结果: sum=%d", result->sum);
        return;
      }

      std::this_thread::sleep_for(1s);

      RCLCPP_INFO(this->get_logger(), "连续反馈中: 进度: %.2f", feedback->progress);
    }
    result->sum = sum;
    goal_handle->succeed(result);
    RCLCPP_INFO(this->get_logger(), "目标执行完成, 结果: sum=%d", result->sum);

  }

  /* 处理目标被接受 */
  void handle_accepted(const std::shared_ptr<rclcpp_action::ServerGoalHandle<Progress>> goal_handle)
  {
    RCLCPP_INFO(this->get_logger(), "目标被接受, 开始执行.");

    std::thread{std::bind(&ProgressActionServer::execute, this, std::placeholders::_1), goal_handle}.detach();

  }


private:
  rclcpp_action::Server<Progress>::SharedPtr server_;
};

int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;

  // init
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<ProgressActionServer>());

  // release
  rclcpp::shutdown();

  return 0;
}
