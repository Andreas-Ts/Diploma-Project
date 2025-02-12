import serial
import struct
import os
import csv
import datetime as datetime
import zoneinfo as zoneinfo

# Replace 'Your/Timezone' with your actual timezone, e.g., 'America/New_York'
local_timezone = zoneinfo('Europe/Athens')

# Replace 'COMX' with your port ('/dev/ttyUSB0' on Linux/Mac, 'COM3' on Windows)
serial_port = "COM6"
baud_rate = 115200

# Path to save the CSV file
csv_file_path = os.path.join('data', 'sensor_data.csv')  # This points to root_project_file/data/sensor_data.csv
try:

    # Ensure the 'data' folder exists
    os.makedirs(os.path.dirname(csv_file_path), exist_ok=True)
    

    struct_format = '<IfIfIf'
    expected_size = struct.calcsize(struct_format)  # Compute expected size
    print(f"Expected struct size: {expected_size} bytes")

    ser = serial.Serial(serial_port, baud_rate, timeout=1)
    print(f"Connected to {serial_port}")
    

     # Check if the file exists and print the absolute path for clarity
    print(f"Checking if file exists at: {os.path.abspath(csv_file_path)}")

    file_exists = os.path.exists(csv_file_path)
    print(f"File exists: {file_exists}")
    if not file_exists:
        with open(csv_file_path, mode='w', newline='') as csv_file:
            csv_writer = csv.writer(csv_file)
            csv_writer.writerow(['Date','Time','Id', 'Temperature', 'Humidity', 'Pressure', 'Gas Resistance', 'Altitude'])
            print("Header written to new CSV file.")
        

       
    with open(csv_file_path, mode='a', newline='') as csv_file:

        csv_writer = csv.writer(csv_file)

        while True:

            # Step 1: Read struct size (2 bytes)
            size_bytes = ser.read(2) 
            print("hi")
            print(size_bytes) 
            print(len(size_bytes))
            if len(size_bytes) < 2:
                continue  # Skip if not enough data
            
            struct_size = struct.unpack('<H', size_bytes)[0]  # Convert bytes to int (little-endian)
            print(f"Struct Size: {struct_size} bytes")
            data_bytes = ser.read(struct_size)
            print(f"Received {len(data_bytes)} bytes: {data_bytes.hex()}")  # Print in HEX
            # Step 2: Read the struct data
            print(data_bytes)
            print(struct_size)

            if len(data_bytes) == struct_size:
                currentDateAndTime = datetime.now()
                current_date = current_time.date()
                current_time = current_time.time()
                id, temperature, pressure, humidity, gas_resistance, altitude = struct.unpack('<IfIfIf', data_bytes)
                # Write data row into CSV file
                csv_writer.writerow([current_date,
                                     current_time,
                                     id, 
                                     temperature, 
                                     humidity, 
                                     pressure, 
                                     gas_resistance,
                                     altitude])
                csv_file.flush()
                # Also print the data for debugging
                print(f"Date : {current_date}, "
                      f"Time : {current_time}, "
                      f"Id: {id}, "
                      f"Temperature: {temperature:.2f} °C, "
                      f"Humidity: {humidity:.2f} %, "
                      f"Gas resistance: {gas_resistance} Ohms, "
                      f"Altitude: {altitude:.2f} m.")

except serial.SerialException as e:
    print(f"Error: {e}")

finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()