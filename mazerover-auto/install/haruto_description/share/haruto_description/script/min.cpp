#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "std_msgs/msg/float32.hpp"

class DistancePublisher : public rclcpp::Node {
public:
    DistancePublisher() : Node("distance_publisher") {
        // Subscribe to the /scan topic
        subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan", 10, std::bind(&DistancePublisher::scan_callback, this, std::placeholders::_1));
        
        // Publisher for the /distance topic
        publisher_ = this->create_publisher<std_msgs::msg::Float32>("/distance", 10);
    }

private:
    void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
        // Find the minimum distance (closest object)
        float min_distance = *std::min_element(msg->ranges.begin(), msg->ranges.end());

        // Create a Float32 message
        auto distance_msg = std_msgs::msg::Float32();
        distance_msg.data = min_distance;

        // Publish the distance
        publisher_->publish(distance_msg);
        RCLCPP_INFO(this->get_logger(), "Published Distance: %.2f meters", min_distance);
    }

    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr publisher_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<DistancePublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
