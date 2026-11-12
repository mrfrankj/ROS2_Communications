#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/srv/add_ints.hpp"
#include <cstdint>

using base_interfaces_demo::srv::AddInts;
using namespace std::chrono_literals;

// 3. 自定义节点类
class AddIntsClient: public rclcpp::Node
{
public:
    AddIntsClient(): Node("addints_client_node_cpp")
    {
        RCLCPP_INFO(this->get_logger(), "客户端节点创建.");
        // 创建客户端
        client_ = this->create_client<base_interfaces_demo::srv::AddInts>("add_ints");

    }
    // 连接服务器
    bool connect_server(){
        while(!client_->wait_for_service(3s))
        {
            if(!rclcpp::ok())
            {
                RCLCPP_ERROR(this->get_logger(), "客户端节点被中断!");
                return false;
            }
            RCLCPP_INFO(this->get_logger(), "服务连接中...");
        }
        return true;
    }
    // 发送请求
    rclcpp::Client<AddInts>::FutureAndRequestId send_request(int32_t num1, int32_t num2)
    {
        auto request = std::make_shared<AddInts::Request>();
        request->num1 = num1;
        request->num2 = num2;
        
        return client_->async_send_request(request);
    }

private:
  rclcpp::Client<base_interfaces_demo::srv::AddInts>::SharedPtr client_;
};

int main(int argc, char ** argv)
{
    if(argc != 3)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "参数个数不正确! 需要两个整数参数.");
        return 1;
    }


    // init
    rclcpp::init(argc, argv);

    // create client node
    auto client = std::make_shared<AddIntsClient>();
    bool flag = client->connect_server();
    if(!flag)
    {
        RCLCPP_ERROR(client->get_logger(), "连接服务器失败!");
        return 1;
    }

    auto future = client->send_request(atoi(argv[1]), atoi(argv[2]));
    // 等待响应
    if(rclcpp::spin_until_future_complete(client, future) == rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_INFO(client->get_logger(), "请求发送成功! sum = %d", future.get()->sum);
    }
    else
    {
        RCLCPP_ERROR(client->get_logger(), "请求发送失败!");
        return 1;
    }
    
    // release
    rclcpp::shutdown();

    return 0;
}



