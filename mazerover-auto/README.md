## **Clone the `mazerover-auto` workspace from GitHub:**
   ```bash
   git clone https://github.com/mission-mangal/mazerover-auto
   ```
## **Build and Run the Package**

1.  Navigate to the workspace:
    ```bash
    cd ~/mazerover-auto
    ```
2.  Build the package:
    ```bash
    colcon build
    ```
3.  Source the workspace:
    ```bash
    source install/local_setup.bash
    ```
4.  Launch the robot simulation:
    ```bash
    ros2 launch haruto_description visualize_robot_simulation.launch.py
    ```
# **ROS 2 Distance Publisher Node Documentation**  

## **Overview**  
The **Distance Publisher** is a ROS 2 node that subscribes to a **LiDAR sensor's scan data** from the `/scan` topic and determines the **closest detected object**. The minimum distance is then published to the `/distance` topic as a `Float32` message.  

This node is useful for **obstacle detection**, **navigation**, and **safety monitoring** in robotic applications.

---

## **Node Information**  
- **Node Name**: `distance_publisher`  
- **Subscribed Topic**: `/scan` (`sensor_msgs::msg::LaserScan`)  
- **Published Topic**: `/distance` (`std_msgs::msg::Float32`)  
- **Programming Language**: C++  
- **Dependencies**:  
  - `rclcpp` (ROS 2 core library)  
  - `sensor_msgs` (for LaserScan messages)  
  - `std_msgs` (for Float32 messages)  

---


Modify the `CMakeLists.txt` in your package to include this new node:  
```cmake
add_executable(talker src/publisher_member_function.cpp)
ament_target_dependencies(talker rclcpp std_msgs sensor_msgs)
install(TARGETS talker DESTINATION lib/${PROJECT_NAME})
```
---
## To run min.cpp node run this command on your terminal after sourcing the workspace:
```bash
ros2 run haruto_description talker
```

## **Code Explanation of min.cpp in script directory**  

### **1. Node Initialization**  
The node is named `"distance_publisher"` and initializes a **subscriber** and a **publisher**:  
```cpp
DistancePublisher() : Node("distance_publisher") {
    subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan", 10, std::bind(&DistancePublisher::scan_callback, this, std::placeholders::_1));
    publisher_ = this->create_publisher<std_msgs::msg::Float32>("/distance", 10);
}
```
- Subscribes to the `/scan` topic to receive **LiDAR** data.
- Creates a publisher for `/distance` to **publish** the closest object’s distance.

### **2. Processing LaserScan Data**  
```cpp
void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
    float min_distance = *std::min_element(msg->ranges.begin(), msg->ranges.end());
    auto distance_msg = std_msgs::msg::Float32();
    distance_msg.data = min_distance;
    publisher_->publish(distance_msg);
    RCLCPP_INFO(this->get_logger(), "Published Distance: %.2f meters", min_distance);
}
```
- Extracts **distance measurements** from the LaserScan message.
- Finds the **minimum** distance (closest object).
- Publishes this **distance** as a `Float32` message to `/distance`.

### **3. Main Function Execution**  
```cpp
int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<DistancePublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
```
- Initializes ROS 2.
- Runs the **DistancePublisher** node in a loop to continuously process LiDAR data.

---

## **Topics Summary**  
| Topic Name | Message Type | Description |
|------------|--------------|-------------|
| `/scan` | `sensor_msgs::msg::LaserScan` | Incoming LiDAR scan data |
| `/distance` | `std_msgs::msg::Float32` | Minimum detected distance from the scan |

---

## **Example Output**  
When running the node, it will continuously publish the closest object's distance and log the output:  
```sh
[INFO] [distance_publisher]: Published Distance: 1.23 meters
[INFO] [distance_publisher]: Published Distance: 0.89 meters
[INFO] [distance_publisher]: Published Distance: 1.67 meters
```

---

## **Modifications & Extensions**  
### **1. Filtering Invalid LiDAR Readings**  
LiDAR sensors may return **infinite values** or `NaN` for missing data. Modify the callback function to handle such cases:  
```cpp
float min_distance = std::numeric_limits<float>::infinity();
for (float range : msg->ranges) {
    if (std::isfinite(range) && range > 0) {
        min_distance = std::min(min_distance, range);
    }
}
```
### **2. Publishing Additional Data**  
You could also **publish the angle** of the closest object by modifying the message to include more information.

---

## **Conclusion**  
This ROS 2 node processes **LiDAR scan data**, finds the closest object, and publishes the result to `/distance`. It can be integrated into **robot navigation**, **collision avoidance**, or **mapping** applications. 🚀

