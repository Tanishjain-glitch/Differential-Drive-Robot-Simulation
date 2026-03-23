#Differential Drive Robot RPM Pipeline

## Overview
This project subscribes to `cmd_vel` messages from a ROS2 bag file, computes
left and right wheel RPMs for a differential drive robot, exchanges data between
3 scripts, and plots the RPM values in real time.

## Data Flow
```
ROS2 Bag File
     ↓  (publishes /cmd_vel)
Script A (C++)  →  computes RPM  →  writes to Shared Memory
                                          ↓
Script B (C++)  →  reads Shared Memory  →  serves REST API (port 8080)
                                          ↓
Script C (Python)  →  fetches REST API  →  prints + plots RPM live
```

## Robot Parameters
- Wheel-to-wheel distance: 443 mm
- Wheel diameter: 181 mm

## Scripts

### Script A (script_a.cpp)
- ROS2 C++ node
- Subscribes to `/cmd_vel` (geometry_msgs/Twist)
- Computes left/right wheel RPM using differential drive kinematics
- Writes RPM, velocities and epoch timestamp to POSIX shared memory (`/robot_data`)

### Script B (script_b.cpp)
- Reads shared memory from Script A at 10 Hz
- Prints all values to console
- Serves HTTP REST API on port 8080
- Endpoint: `GET /get_data_from_B` — returns JSON payload

### Script C (script_c.py)
- Fetches data from Script B via REST API every 100ms
- Prints fetched values to console
- Live plots left and right wheel RPM using matplotlib

## Dependencies

### ROS2
```bash
sudo apt install ros-humble-rclcpp
sudo apt install ros-humble-geometry-msgs
```

### C++ Libraries
```bash
# nlohmann json
sudo apt install nlohmann-json3-dev

# cpp-httplib (single header — already in include/ folder)
```

### Python
```bash
pip install flask requests matplotlib
```

## Build

```bash
cd ~/rse004_ws
colcon build --packages-select rse004_robot
source install/setup.bash
```

## Launch

Open 4 terminals and run in this order:

### Terminal 1 — Script A
```bash
source ~/rse004_ws/install/setup.bash
ros2 run rse004_robot script_a
```

### Terminal 2 — Bag File
```bash
ros2 bag play ~/rse004_ws/src/rse004_robot/bag/rse_assignment_unbox_robotics.db3
```

### Terminal 3 — Script B
```bash
~/rse004_ws/install/rse004_robot/lib/rse004_robot/script_b
```

### Terminal 4 — Script C
```bash
python3 ~/rse004_ws/src/rse004_robot/src/script_c.py
```

## API Reference

**Endpoint:** `GET http://localhost:8080/get_data_from_B`

**Response:**
```json
{
  "rpm_left": 50.42,
  "rpm_right": 55.10,
  "linear_vel": 0.5,
  "angular_vel": 0.1,
  "timestamp_A": 1774176224111,
  "timestamp_B": 1774176230591
}
```

## Resilience
- If Script A crashes → Script B keeps serving last valid shared memory values
- If Script B crashes → Script C catches HTTP error and retries next loop
- If Script C crashes → Scripts A and B are completely unaffected

## Folder Structure
```
rse004_robot/
├── src/
│   ├── script_a.cpp
│   ├── script_b.cpp
│   └── script_c.py
├── include/
│   └── httplib.h
├── bag/
│   └── rse_assignment_unbox_robotics.db3
├── CMakeLists.txt
├── package.xml
└── README.md
```
