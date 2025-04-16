import serial
import struct
import os
import csv
import sys
import time
from zoneinfo  import ZoneInfo
from datetime import datetime, timedelta

#number of devices the network has
NUMBER_OF_DEVICES = 3

#different types of sensors the network has
SENSOR_TYPES = 2

#the size of the biggest message we are going to send
MAX__MESSAGE_SIZE = 64

# Replace 'Your/Timezone' with your actual timezone, e.g., 'America/New_York'
local_timezone = ZoneInfo('Europe/Athens')

# Replace 'COMX' with your port ('/dev/ttyUSB0' on Linux/Mac, 'COM3' on Windows)
serial_port = "COM7"
baud_rate = 115200

# Path to save the CSV file
csv_file_path = os.path.join('data', 'sensor_data.csv')  # This points to root_project_file/data/sensor_data.csv

list_of_csv_variables = ('DateTime','Id','Sensor',
                         "BME680:IAQ","BME680:IAQ ACCURACY","BME680:IAQ STATIC","BME680:co2Equivalent",
                         "BME680:breathVocEquivalent","BME680:rawTemperature","BME680:pressure","BME680:rawHumidity",
                         "BME680:gasResistance","BME680:stabStatus","BME680:runInStatus","BME680:temperature",
                         "BME680:humidity","BME680:gasPercentage",
                         "CCS811:eCO2","CCS811:TVOC"
                         )  
try:

    # Ensure the 'data' folder exists
    os.makedirs(os.path.dirname(csv_file_path), exist_ok=True)
    
  
    
    ser = serial.Serial(serial_port, baud_rate, timeout=2,write_timeout =3)
    print(f"Connected to {serial_port}")
    

     # Check if the file exists and print the absolute path for clarity
    print(f"Checking if file exists at: {os.path.abspath(csv_file_path)}")

    file_exists = os.path.exists(csv_file_path)
    print(f"File exists: {file_exists}")
    ser.reset_input_buffer()
    if not file_exists:
        with open(csv_file_path, mode='w', newline='') as csv_file:
            csv_writer = csv.writer(csv_file)
            csv_writer.writerow(list_of_csv_variables)
            print("Header written to new CSV file.")
        

       
    with open(csv_file_path, mode='a', newline='') as csv_file:
        #create the writer based of the dictionary that has all the possible variables
        csv_writer = csv.DictWriter(csv_file,fieldnames=list_of_csv_variables)
        #everytime we use continue we stop the loop, and rerun,we suposse it will be hard to fit all the structures by accident
        read_something = False
        while True:
           
            #first we read all the message
            data_bytes = ser.read(MAX__MESSAGE_SIZE)

            length_of_read_message = len(data_bytes)
            #if it isn't a full message
            if length_of_read_message < MAX__MESSAGE_SIZE:
                ser.reset_input_buffer()

                continue
            #grab the id and sensor type first
            format_struct = "<ii"
            struct_unpacked_data =  struct.unpack(format_struct,data_bytes[:8]) 
            id = struct_unpacked_data[0]  
            sensor_type = struct_unpacked_data[1]  
            #check if id is legal

            if (abs(id) >= NUMBER_OF_DEVICES):
                #clear the buffer
                ser.reset_input_buffer()
                continue

            #check if sensor type is legal
            if (abs(sensor_type) > SENSOR_TYPES):
                ser.reset_input_buffer()

                continue
           
            #enum types in c are interger so we will match each interger value with its sensor
            sensor_used = "NO-KNOWN-SENSOR"
            
            #we use the limit_based_of_sensor as a way to get the actual useful information for sensors type less than the max size message
            #We start for 8 bytes
            if (sensor_type==1):
                sensor_used = "BME680"
                format_struct = "<fBffffffffffff"
                limit_based_of_sensor = MAX__MESSAGE_SIZE
            elif (sensor_type==2):
                sensor_used = "CCS811:"
                format_struct = "<II"
                limit_based_of_sensor = 16 #WE only send 2 bytes
            else :
                print(f"Not correct sensor type the number is {sensor_type}")
                ser.reset_input_buffer()
                continue      

            struct_unpacked_data =struct.unpack_from(format_struct,data_bytes[8:limit_based_of_sensor])

             #create a dictionary for all potential variables as we have got the id,sensor type and the payload succesfully
            data_dictionary = {variable: None for variable in list_of_csv_variables}

            #get current date and time
            currentDateAndTime = datetime.now(local_timezone)
            current_date = currentDateAndTime.date()
            current_time = currentDateAndTime.time()
            #Assign date,time,id and Sensor based of the interger value
            data_dictionary["DateTime"] = datetime.isoformat(currentDateAndTime)
            
            data_dictionary["Id"] = id
            data_dictionary["Sensor"] = sensor_used
            
            corresponding_variables_that_will_insert_values = [key for key in data_dictionary if key.startswith(sensor_used)]

            for variable,value in zip(corresponding_variables_that_will_insert_values,struct_unpacked_data):
                data_dictionary[variable]=value

            # Write data row into CSV file
            csv_writer.writerow(data_dictionary)
            
            csv_file.flush()
            # Also print the data for debugging
            #print(data_dictionary)
            #write a response back into the esp32 to confirm the transition
          
    

except serial.SerialException as e:
    print(f"Error: {e}")
   

finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()