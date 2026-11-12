# ROS2_Communications

ROS 2 通信机制学习示例（C++ 实现），用同一套工作空间演示 ROS 2 的三种核心通信方式：

| 通信方式 | 包 | 特点 |
| --- | --- | --- |
| 话题通信 Topic | `cpp01_topic` | 单向、异步，发布/订阅，一对多 |
| 服务通信 Service | `cpp02_service` | 请求/响应，同步等待，一对一 |
| 动作通信 Action | `cpp03_action` | 请求/响应 + 连续反馈 + 可取消，适合长任务 |

自定义接口统一放在 `base_interfaces_demo` 包中（`msg` / `srv` / `action`）。

---

## 目录结构

```
ROS2_Communications/
└── src/
    ├── base_interfaces_demo/        # 自定义接口包
    │   ├── msg/Student.msg
    │   ├── srv/AddInts.srv
    │   └── action/Progress.action
    ├── cpp01_topic/                 # 话题通信：字符串 / 自定义消息
    │   └── src/demo01_talker_str.cpp
    │       src/demo02_listener_str.cpp
    │       src/demo03_talker_stu.cpp
    │       src/demo04_listener_stu.cpp
    ├── cpp02_service/               # 服务通信：整数求和
    │   └── src/demo01_server.cpp
    │       src/demo02_client.cpp
    └── cpp03_action/                # 动作通信：累加求和 + 进度反馈
        └── src/demo01_action_server.cpp
            src/demo02_action_client.cpp
```

## 环境要求

- ROS 2（Foxy / Humble / Jazzy 均可，仅使用 `rclcpp`、`rclcpp_action`、`std_msgs`）
- `colcon`、CMake ≥ 3.8、支持 C++17 的编译器

## 编译

```bash
# 克隆后当前目录即为工作空间根目录
cd ROS2_Communications

# 先编译接口包，再编译依赖它的功能包
colcon build --packages-select base_interfaces_demo
source install/setup.bash
colcon build --packages-select cpp01_topic cpp02_service cpp03_action
source install/setup.bash
```

> 也可以直接 `colcon build` 一次编译全部：colcon 会按依赖顺序自动处理 `base_interfaces_demo`。
> Windows 下把 `source install/setup.bash` 换成 `call install/setup.bat`。

---

## 一、话题通信（cpp01_topic）

发布者与订阅者通过话题异步收发消息，双方互不等待。示例包含字符串（内置 `std_msgs/msg/String`）与自定义消息（`Student`）两组。

### 1. 字符串话题 `chatter`

| 节点 | 类型 | 说明 |
| --- | --- | --- |
| `talker_node_cpp` | 发布者 | 1 s 定时器发布 `Hello, ROS2 C++, <序号>` |
| `listener_node_cpp` | 订阅者 | 打印收到的字符串 |

```bash
ros2 run cpp01_topic demo01_talker_str     # 终端 1
ros2 run cpp01_topic demo02_listener_str   # 终端 2
```

### 2. 自定义消息话题 `chatter_stu`

消息定义 `base_interfaces_demo/msg/Student`：

```
string  name
int32   age
float64 height
```

| 节点 | 类型 | 说明 |
| --- | --- | --- |
| `talkerstu_node_cpp` | 发布者 | 500 ms 定时器发布学生信息，`age` 从 10 开始自增 |
| `listenerstu_node_cpp` | 订阅者 | 打印 name / age / height |

```bash
ros2 run cpp01_topic demo03_talker_stu     # 终端 1
ros2 run cpp01_topic demo04_listener_stu   # 终端 2
```

### 常用调试命令

```bash
ros2 topic list
ros2 topic echo /chatter
ros2 topic echo /chatter_stu
ros2 topic info /chatter -v      # 查看发布者/订阅者数量与 QoS
ros2 interface show base_interfaces_demo/msg/Student
```

---

## 二、服务通信（cpp02_service）

客户端发送请求，服务端处理后返回响应。相比话题，它是「一问一答」的同步模式，服务端同一时刻只处理一个请求。

服务定义 `base_interfaces_demo/srv/AddInts`：

```
int32 num1
int32 num2
---
int32 sum
```

| 节点 | 类型 | 说明 |
| --- | --- | --- |
| `addints_server_node_cpp` | 服务端 | 提供 `add_ints` 服务，返回两数之和 |
| `addints_client_node_cpp` | 客户端 | 命令行传入两个整数，等待服务上线后发起请求并等待响应 |

```bash
ros2 run cpp02_service demo01_server          # 终端 1
ros2 run cpp02_service demo02_client 3 5      # 终端 2 → sum = 8
```

客户端实现要点：
- `wait_for_service(3s)` 循环等待服务端上线，未上线时持续打印「服务连接中…」
- `async_send_request()` 发起异步请求，`spin_until_future_complete()` 阻塞等待响应

```bash
ros2 service list
ros2 service call /add_ints base_interfaces_demo/srv/AddInts "{num1: 10, num2: 20}"
```

---

## 三、动作通信（cpp03_action）

动作 = 服务 + 话题。客户端提交目标（goal），服务端在执行过程中持续发布反馈（feedback），最终返回结果（result），并支持中途取消（cancel）。适用于耗时较长的任务。

接口定义 `base_interfaces_demo/action/Progress`：

```
int32 num          # Goal   目标值
---
int32 sum          # Result 最终结果
---
float64 progress   # Feedback 执行进度（0.0 ~ 1.0）
```

| 节点 | 类型 | 说明 |
| --- | --- | --- |
| `progress_action_server_node_cpp` | 动作服务端 | 提供 `get_sum` 动作，计算 0+1+…+num，每秒发布一次进度 |
| `progress_action_client_node_cpp` | 动作客户端 | 命令行传入 num，打印进度与最终结果 |

```bash
ros2 run cpp03_action demo01_action_server     # 终端 1
ros2 run cpp03_action demo02_action_client 10  # 终端 2
```

服务端行为：
- `handle_goal`：`num < 1` 时拒绝目标，否则接受并执行
- `execute`：累加循环中发布 `progress = i / num` 反馈，每秒一次
- 每轮循环检查 `is_canceling()`，若客户端取消则回传已算出的 `sum` 并置为 canceled
- `handle_accepted` 中把 `execute` 放到独立线程，避免阻塞 executor

客户端行为：`wait_for_action_server(5s)` 后发送目标，并注册 goal / feedback / result 三个回调，分别处理「目标是否被接受」「当前进度」「最终结果（SUCCEEDED / ABORTED / CANCELED）」。

```bash
ros2 action list
ros2 action info /get_sum
ros2 interface show base_interfaces_demo/action/Progress
```

---

## 三种通信方式对比

| 维度 | 话题 Topic | 服务 Service | 动作 Action |
| --- | --- | --- | --- |
| 通信模型 | 发布/订阅 | 请求/响应 | 目标/反馈/结果 |
| 同步性 | 异步 | 同步（阻塞等响应） | 异步 + 持续反馈 |
| 反馈 | 无 | 无 | 有（0~N 次） |
| 可取消 | 无 | 无 | 支持 |
| 典型场景 | 传感器数据、状态广播 | 一次性查询/计算 | 导航、抓取等长任务 |

## License

暂无（学习示例）。如需开源可补充 Apache-2.0 或 MIT，并同步更新各 `package.xml` 中的 `<license>` 字段。
