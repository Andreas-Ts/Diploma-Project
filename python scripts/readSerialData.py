import serial
import struct
import os
import csv
import sys
from zoneinfo  import ZoneInfo
from datetime import datetime, timedelta



# Replace 'Your/Timezone' with your actual timezone, e.g., 'America/New_York'
local_timezone = ZoneInfo('Europe/Athens')

# Replace 'COMX' with your port ('/dev/ttyUSB0' on Linux/Mac, 'COM3' on Windows)
serial_port = "COM7"
baud_rate = 115200

# Path to save the CSV file
csv_file_path = os.path.join('data', 'sensor_data.csv')  # This points to root_project_file/data/sensor_data.csv

list_of_csv_variables = ('Date','Time','Id','Sensor',
                         "BME680:IAQ","BME680:IAQ ACCURACY","BME680:IAQ STATIC","BME680:co2Equivalent",
                         "BME680:breathVocEquivalent","BME680:rawTemperature","BME680:pressure","BME680:rawHumidity",
                         "BME680:gasResistance","BME680:stabStatus","BME680:runInStatus","BME680:temperature",
                         "BME680:humidity","BME680:gasPercentage",
                         "CCS811:eCO2","CCS811:TVOC"
                         )  
try:

    # Ensure the 'data' folder exists
    os.makedirs(os.path.dirname(csv_file_path), exist_ok=True)
    

    struct_format = '<IfIfIf'
    expected_size = struct.calcsize(struct_format)  # Compute expected size
    print(f"Expected struct size: {expected_size} bytes")

    ser = serial.Serial(serial_port, baud_rate, timeout=1,write_timeout =1)
    print(f"Connected to {serial_port}")
    

     # Check if the file exists and print the absolute path for clarity
    print(f"Checking if file exists at: {os.path.abspath(csv_file_path)}")

    file_exists = os.path.exists(csv_file_path)
    print(f"File exists: {file_exists}")
    if not file_exists:
        with open(csv_file_path, mode='w', newline='') as csv_file:
            csv_writer = csv.writer(csv_file)
            csv_writer.writerow(list_of_csv_variables)
            print("Header written to new CSV file.")
        

       
    with open(csv_file_path, mode='a', newline='') as csv_file:
        #create the writer based of the dictionary that has all the possible variables
        csv_writer = csv.DictWriter(csv_file,fieldnames=list_of_csv_variables)

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
                #get current date and time
                currentDateAndTime = datetime.now(local_timezone)
                current_date = currentDateAndTime.date()
                current_time = currentDateAndTime.time()
                #create a dictionary for all potential variables
                data_dictionary = {variable: None for variable in list_of_csv_variables}
                
                #Assign date,time,id and Sensor based of the interger value
                data_dictionary["Date"] = current_date
                data_dictionary["Time"] = current_time
                #slice the first part of the buffer in order to take id and sensor.We need it as the sensor variable value will give
                #us the knowledge of what format string we will use as for now,different sensors have different variables
                
                format_struct = "<ii"
                struct_unpacked_data =  struct.unpack(data_bytes[:7],format_struct)
                data_dictionary['Id'] = struct_unpacked_data[0]
               
                if (struct_unpacked_data[1]==1):
                    sensor_used = "BME680"
                    format_struct = "<fBffffffffffff"
                if (struct_unpacked_data[1]==2):
                    sensor_used = "CSS811"
                    format_struct = "<HH"
                data_dictionary['Sensor'] = sensor_used
                struct_unpacked_data =struct.unpack_from(data_bytes,format_struct,sys.getsizeof(data_bytes[:7]))
                
                corresponding_variables_that_will_insert_values = [key for key in data_dictionary if key.startswith(sensor_used)]

                for variable,value in zip(corresponding_variables_that_will_insert_values,struct_unpacked_data):
                    data_dictionary[variable]=value

                # Write data row into CSV file
                csv_writer.writerow(data_dictionary)
                
                csv_file.flush()
                # Also print the data for debugging
                print(data_dictionary)
                
            
except serial.SerialException as e:
    print(f"Error: {e}")
   

finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()