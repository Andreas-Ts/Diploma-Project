from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import os
import os
import csv
from zoneinfo  import ZoneInfo
from datetime import datetime


list_of_csv_variables = ('Timestamp','Id','Sensor',
                         "BME680:iaq","BME680:iaqAccuracy","BME680:staticIaq","BME680:co2Equivalent","BME680:co2EquivalentAccuracy",
                         "BME680:breathVocEquivalent","BME680:breathVocEquivalentAccuracy",
                         "BME680:rawTemperature","BME680:pressure","BME680:rawHumidity",
                         "BME680:gasResistance","BME680:stabStatus","BME680:runInStatus","BME680:temperature",
                         "BME680:humidity","BME680:gasPercentage","BME680:gasPercentageAccuracy",
                         "CCS811:eCO2","CCS811:TVOC","CCS811:RawResistance","CCS811:30minutesPassed"
                         )  

# Path to save the CSV file
csv_file_path = os.path.join('data', 'sensor_data.csv')  # This points to root_project_file/data/sensor_data.csv



class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        print("Received POST request")
        print(f"Request from: {self.client_address}")  # Log client IP
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        print(f"Raw data received: {post_data.decode()}")  # Log raw JSON
        try:
            # Parse JSON data
            data = json.loads(post_data.decode('utf-8'))
            print(f"Received JSON message: {data}")
            saveData(data)
        except json.JSONDecodeError:
            print("Received invalid JSON")
            self.send_response(400)  # Bad Request
            self.end_headers()
            self.wfile.write(b"Invalid JSON")
            return
        except Exception as e:
            print(f"Error: {e}")
            self.send_response(500)  # Internal Server Error
            return
        
        self.send_response(200)
        self.end_headers()
        self.wfile.write(b"Message received")

    def do_GET(self):
        # Define the path to the CSV file
        

        if os.path.exists(csv_file_path):
            self.send_response(200)
            self.send_header('Content-type', 'text/csv')
            self.end_headers()
            with open(csv_file_path, 'rb') as file:
                self.wfile.write(file.read())
        else:
            self.send_response(404)  # File not found
            self.end_headers()
            self.wfile.write(b"File not found")


def saveData(message):
    local_timezone = ZoneInfo('Europe/Athens')

    with open(csv_file_path, mode='a', newline='') as csv_file:
          # Write data row into CSV file
        
        csv_writer = csv.DictWriter(csv_file,fieldnames=list_of_csv_variables,extrasaction='ignore')
        
        currentDateAndTime = datetime.now(local_timezone)

        message['DateTime'] = datetime.isoformat(currentDateAndTime)

        print(message)
        csv_writer.writerow(message)

        csv_file.flush()



# Function to run the server
def run_server():
    server_address = ("0.0.0.0", 8080)
    httpd = HTTPServer(server_address, SimpleHTTPRequestHandler)
    print("Server running on http://localhost:8080")
    httpd.serve_forever()






# Run the server if this file is executed directly
if __name__ == "__main__":
        # Ensure the 'data' folder exists
    os.makedirs(os.path.dirname(csv_file_path), exist_ok=True)
    file_exists = os.path.exists(csv_file_path)
    print(f"File exists: {file_exists}")
    if not file_exists:
        with open(csv_file_path, mode='w', newline='') as csv_file:
            csv_writer = csv.writer(csv_file)
            csv_writer.writerow(list_of_csv_variables)
            print("Header written to new CSV file.")
   
    run_server() #start the server
