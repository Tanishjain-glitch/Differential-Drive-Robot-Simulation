# Main Launch Document
## How to Install, Build and Run

---

## System Requirements
- Ubuntu 22.04
- ROS2 Humble
- Python 3.10+
- GCC/G++ compiler

---

## Step 1 — Install Dependencies

### ROS2 Humble (if not installed)
```bash
sudo apt update
sudo apt install ros-humble-desktop
echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

### ROS2 Packages
```bash
sudo apt install ros-humble-rclcpp
sudo apt install ros-humble-geometry-msgs
sudo apt install ros-humble-rosbag2
```

### C++ Libraries
```bash
# nlohmann json
sudo apt install nlohmann-json3-dev

# cpp-httplib — already included in include/ folder
# no installation needed
```

### Python Libraries
```bash
pip install flask requests matplotlib
```

---

## Step 2 — Build the Package

```bash
# Source ROS2
source /opt/ros/humble/setup.bash

# Go to workspace
cd ~/rse004_ws

# Build
colcon build --packages-select rse004_robot

# Source workspace
source install/setup.bash
```

---

## Step 3 — Launch (4 Terminals)

### ⚠️ Important — Always follow this order:
```
Terminal 1 → Terminal 3 → Terminal 2 → Terminal 4
```

---

### Terminal 1 — Script A (ROS2 Node)
```bash
source ~/rse004_ws/install/setup.bash
ros2 run rse004_robot script_a
```

Expected output:
```
[INFO] Script A started
[INFO] L: 50.42 | R: 55.10 | V: 0.50 | W: 0.10 | T: 1774176224111
```

---

### Terminal 2 — ROS2 Bag File
```bash
ros2 bag play ~/rse004_ws/src/rse004_robot/bag/rse_assignment_unbox_robotics.db3
```

Expected output:
```
[INFO] Opened database for READ_ONLY
[INFO] Set rate to 1
```

---

### Terminal 3 — Script B (Shared Memory + REST API)
```bash
~/rse004_ws/install/rse004_robot/lib/rse004_robot/script_b
```

Expected output:
```
Script B started
Server running on port 8080
L_RPM: 50.42 R_RPM: 55.10 V: 0.5 W: 0.1 T_A: 1774.. T_B: 1774..
```

---

### Terminal 4 — Script C (Python + Live Plot)
```bash
python3 ~/rse004_ws/src/rse004_robot/src/script_c.py
```

Expected output:
```
L_RPM: 50.42 | R_RPM: 55.10 | V: 0.50 | W: 0.10 | T_A: 1774.. | T_B: 1774..
```
A live matplotlib window will open showing RPM plot.

---

## Step 4 — Verify Everything is Working

### Check Script A → Script B (Shared Memory)
Script B terminal should show same RPM values as Script A ✅

### Check Script B → Script C (REST API)
Open browser and go to:
```
http://localhost:8080/get_data_from_B
```
You should see JSON response with all values ✅

### Check Live Plot
matplotlib window should show:
- Blue line = Left RPM
- Red line = Right RPM
- Values updating in real time ✅

---

## Resilience Testing

### Kill Script A — Script B should keep running:
```bash
# In Terminal 1 press Ctrl+C
# Script B continues with last known values ✅
```

### Kill Script B — Script C should keep running:
```bash
# In Terminal 3 press Ctrl+C
# Script C prints "Waiting for Script B..." and retries ✅
```

### Restart any script — pipeline recovers automatically ✅

---

## Folder Structure
```
rse004_robot/
├── src/
│   ├── script_a.cpp      # ROS2 subscriber + shared memory writer
│   ├── script_b.cpp      # Shared memory reader + REST API server
│   └── script_c.py       # REST API client + live plotter
├── include/
│   └── httplib.h         # HTTP library (single header)
├── bag/
│   └── rse_assignment_unbox_robotics.db3
├── CMakeLists.txt
├── package.xml
├── README.md
└── LAUNCH.md             # this file
```

---

## Troubleshooting

| Problem | Solution |
|---|---|
| `script_a` not found | Run `source install/setup.bash` |
| Port 8080 already in use | `sudo kill $(lsof -t -i:8080)` |
| Script B shows all zeros | Start Script A first, then Script B |
| Plot not showing | Install matplotlib: `pip install matplotlib` |
| Bag file not playing | Check file path is correct |
