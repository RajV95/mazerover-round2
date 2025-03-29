#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include <random>

using std::placeholders::_1;

class MazeSolver : public rclcpp::Node {
public:
    MazeSolver() : Node("maze_solver"), initialized_(false), zigzag_dir_(1) {
        cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        scan_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan", 10, std::bind(&MazeSolver::laser_callback, this, _1));
        state_ = "INIT_ROTATION";
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
    std::string state_;
    bool initialized_;
    int zigzag_dir_;

    void laser_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
        float front = *std::min_element(msg->ranges.begin(), msg->ranges.begin() + 10);
        float left = *std::min_element(msg->ranges.begin() + 80, msg->ranges.begin() + 100);
        float right = *std::min_element(msg->ranges.begin() + 260, msg->ranges.begin() + 280);
        
        RCLCPP_INFO(this->get_logger(), "Front: %.2f, Left: %.2f, Right: %.2f", front, left, right);

        geometry_msgs::msg::Twist cmd;

        if (state_ == "INIT_ROTATION") {
            cmd.linear.x = 0.0;
            cmd.angular.z = 0.61; // Approx. 35 degrees turn
            state_ = "FORWARD";
        } else if (front < 0.5) { 
            cmd.linear.x = 0.0;

            float random_angle = (std::rand() % 245 + 15) * (M_PI / 180.0); // Random angle between 30 to 120 degrees in radians
            cmd.angular.z = random_angle; 
            state_ = "TURNING_LEFT";
        } else {
            float max_dist = std::max({front, left, right});
            if (max_dist == left) {
                zigzag_dir_ = 1;
            } else if (max_dist == right) {
                zigzag_dir_ = -1;
            }

            cmd.linear.x = 1.1;
            cmd.angular.z = 0.3 * zigzag_dir_; // Zigzag motion
            state_ = "FORWARD";
        }
        
        cmd_pub_->publish(cmd);
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MazeSolver>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
