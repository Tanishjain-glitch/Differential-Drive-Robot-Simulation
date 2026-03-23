import requests
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from flask import Flask, jsonify
import threading
import time

app = Flask(__name__)

# store data
data_store = {
    "rpm_left": 0.0,
    "rpm_right": 0.0,
    "linear_vel": 0.0,
    "angular_vel": 0.0,
    "timestamp_A": 0,
    "timestamp_B": 0
}

rpm_left_list  = []
rpm_right_list = []

# flask endpoint
@app.route('/get_data_from_B', methods=['GET'])
def get_data():
    return jsonify(data_store)

def fetch_loop():
    while True:
        try:
            res = requests.get("http://localhost:8080/get_data_from_B", timeout=2)
            data = res.json()

            # update store
            data_store.update(data)
            rpm_left_list.append(data["rpm_left"])
            rpm_right_list.append(data["rpm_right"])

            # print values
            print(f"L_RPM: {data['rpm_left']:.2f} | "
                  f"R_RPM: {data['rpm_right']:.2f} | "
                  f"V: {data['linear_vel']:.2f} | "
                  f"W: {data['angular_vel']:.2f} | "
                  f"T_A: {data['timestamp_A']} | "
                  f"T_B: {data['timestamp_B']}")

        except Exception as e:
            print(f"Waiting for Script B... {e}")

        time.sleep(0.1)  # 10 Hz

def animate(i):
    plt.cla()
    plt.plot(rpm_left_list,  label="Left RPM",  color="blue")
    plt.plot(rpm_right_list, label="Right RPM", color="red")
    plt.xlabel("Time")
    plt.ylabel("RPM")
    plt.title("Wheel RPM Live Plot")
    plt.legend()

if __name__ == "__main__":
    # flask in background
    flask_t = threading.Thread(
        target=lambda: app.run(port=5000, debug=False))
    flask_t.daemon = True
    flask_t.start()

    # fetch loop in background
    fetch_t = threading.Thread(target=fetch_loop)
    fetch_t.daemon = True
    fetch_t.start()

    # live plot on main thread
    fig = plt.figure()
    ani = animation.FuncAnimation(fig, animate, interval=100)
    plt.show()