import serial
import pandas

import time

# n means that the flight will be played n times slower. So 1 means real time, 2 means half speed, etc.
SPEED_FACTOR = 1
# n means that only 1 out of every n packets will be sent. Lower is more granular (min 1, must be integer).
GRANULARITY_FACTOR = 2

# -----------------<Change first param to the correct port, MacOS (ls /dev/tty.*)>-----------------
ser = serial.Serial("/dev/tty.usbmodem132228101", 9600, timeout=10, write_timeout=10)

csv = pandas.read_csv('flight_computer.csv')
total_rows = csv.shape[0]
total_time = total_rows * 10/1000 * SPEED_FACTOR * 2  # 2 because python is lying lmao

written = ["highg_ax", "highg_ay", "highg_az", "barometer_altitude", "temperature", "pressure", "ax", "ay", "az", "gx", "gy", "gz", "mx", "my", "mz", "mx", "my", "mz"]

string = ""

last10 = []

for i, row in csv.iterrows():
    if i % GRANULARITY_FACTOR != 0:
        continue

    start = time.time()
    last10.append(start)
    if len(last10) > 10:
        last10.pop(0)
    line = f"{','.join(str(item) for item in row[written])}\n"

    ser.write(line.encode("utf8"))

    data = ser.read_all()
    decoded = data.decode("utf8")
    if decoded != "":
        string = decoded
    curr = time.time()

    sleep_time = min(max(0.0, 10 / 1000 * GRANULARITY_FACTOR * SPEED_FACTOR - (curr - start)), 10 / 1000 * GRANULARITY_FACTOR * SPEED_FACTOR)

    if i % 7 == 0:
        print(f"\r{total_time - i/total_rows * total_time:.2f} sec remaining, True packet/sec: {sum(last10):.2f}, Sleep Time: {sleep_time*1000:.2f} ms, Recent String: {string!r}", end="", flush=True)

    time.sleep(sleep_time)
