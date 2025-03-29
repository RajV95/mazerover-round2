import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution, Command, FindExecutable, LaunchConfiguration, PythonExpression
from launch.conditions import IfCondition, UnlessCondition
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.actions import (
    DeclareLaunchArgument,
    OpaqueFunction,
    RegisterEventHandler,
    LogInfo,
    IncludeLaunchDescription,
)
from launch.event_handlers import OnProcessExit

def launch_setup(context, *args, **kwargs):

    # General arguments
    robot_type = LaunchConfiguration("robot_type")

    # RViz config file path
    rviz_config_file = PathJoinSubstitution([
        FindPackageShare("haruto_description"), "rviz", "with_camera.rviz"
    ])

    # Xacro command for model generation
    robot_description_content = Command([
        PathJoinSubstitution([FindExecutable(name="xacro")]),
        " ",
        PathJoinSubstitution([
            FindPackageShare("haruto_description"), "urdf", "robot.xacro"
        ]),
        " ",
        "robot_type:=",
        robot_type,
    ])
    robot_description = {"robot_description": robot_description_content}

    # Robot State Publisher node
    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="both",
        parameters=[{"use_sim_time": True}, robot_description]
    )

    # **Gazebo world file (Dynamically located)**
    # world_file = PathJoinSubstitution([
    #     FindPackageShare("haruto_description"), "worlds", "maze.world"
    # ])

    package_share = FindPackageShare(package='haruto_description').find('haruto_description')

    world_file_name = 'maze.world'
    world_path = os.path.join(package_share, 'worlds', world_file_name)

    print("Hi Hello Oyee")
    print(world_path)

    # **Gazebo launch node (No GUI)**
    gazebo_node = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare("gazebo_ros"), "launch", "gazebo.launch.py"
            ])
        ]),
        launch_arguments=[
            #("world", "/home/martial_gautam/Downloads/pre_vashisht/src/haruto_description/worlds/maze.world"),
            # ("world_name", "world_file"),
            # ("world", "../worlds/maze.world"),
            # ("world_name", os.path.abspath(../worlds/maze.world)),
            ("world", world_path),
            ("gui", "true")  # Disable GUI, run in background
        ]
    )

    # Node to spawn the robot model in Gazebo
    spawn_entity_node = Node(
        package="gazebo_ros",
        executable="spawn_entity.py",
        arguments=["-topic", "robot_description", "-entity", "robot"],
        output="screen"
    )

    # Delay start of RViz until after the robot is spawned
    delay_rviz_after_spawn_entity = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=spawn_entity_node,
            on_exit=[
                LogInfo(msg="Spawned robot model in Gazebo. Starting RViz"),
                Node(
                    package="rviz2",
                    executable="rviz2",
                    name="rviz2",
                    output="log",
                    arguments=["-d", rviz_config_file]
                ),
            ],
        )
    )

    nodes_to_start = [
        robot_state_publisher_node,
        gazebo_node,
        spawn_entity_node,
        delay_rviz_after_spawn_entity,
    ]

    return nodes_to_start


def generate_launch_description():
    declared_arguments = [
        DeclareLaunchArgument(
            "robot_type",
            default_value="omni",
            description="Type of robot to visualize (Available options: omni, diff)",
        )
    ]

    return LaunchDescription(declared_arguments + [OpaqueFunction(function=launch_setup)])

