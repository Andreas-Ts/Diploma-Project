

from pymongo import MongoClient,errors
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
csv_file_path = os.path.join('data', 'sensor_data.csv')  # This points to root_project_file/data/sensor_data.csv


class ServerFunctions:

    

    def __init__(self):
        #connect to the database collections
        self.client = MongoClient()  # connect to default localhost:27017
        self.db = self.client['sourceLocalizationDb']  # access or create the database if it doesn't exist
        # access or create the collections if they don't exist
        self.timeSeries = self.db['timeSeries']  #for storing the time series data of the sensors
        self.UserInput = self.db['UserInput'] # for storing the front end data that shows the timestamps of the experiment 
        self.zoneInfo = ZoneInfo('Europe/Athens')  # Set the timezone to Europe/Athens
        #create the cvs file
        # Path to save the CSV file
        os.makedirs(os.path.dirname(csv_file_path), exist_ok=True)
        file_exists = os.path.exists(csv_file_path)
        print(f"File exists: {file_exists}")
        if not file_exists:
            with open(csv_file_path, mode='w', newline='') as csv_file:
                csv_writer = csv.writer(csv_file)
                csv_writer.writerow(list_of_csv_variables)
                print("Header written to new CSV file.")
   
    #get the last user input from a particular category from the database
    def getLastUserInput(self,userInputCategory):
            return self.UserInput.find({'userInputType': userInputCategory}
                ).sort('timestamp', -1).limit(1)

    def getCurrentDateTime(self):
        # Get the current date and time in the local timezone
        currentDateAndTime = datetime.now(self.zoneInfo)
        return currentDateAndTime 
    #get current date and time in ISO format at the local timezone
    def getCurrentDateTimeISOformat(self):
        timeStamp =datetime.isoformat(self.getCurrentDateTime)
        return timeStamp

    #check if the last time user has put an input was less than 1 hour ago.
    # If it more than 1 hour, return False in order to start a new experiment




    #save the data to the csv file and the database
    def saveData(self, message):
        local_timezone = ZoneInfo('Europe/Athens')

        # Write data row into CSV file

        with open(csv_file_path, mode='a', newline='') as csv_file:
            csv_writer = csv.DictWriter(csv_file, fieldnames=list_of_csv_variables, extrasaction='ignore')

            currentDateAndTime = datetime.now(local_timezone)

            message['DateTime'] = datetime.isoformat(currentDateAndTime)

            print(message)
            csv_writer.writerow(message)

            csv_file.flush()

        #Insert the data into the database,deleting the irrelevant fields
        #keep the Timestamp,Id and Sensor fields
        keep_fields = ['Timestamp', 'Id', 'Sensor']
        # Create a new dictionary with only the fields we want to keep
        message = {key: value 
                   for key, value in message.items() 
                   if  key.startswith(message["Sensor"]) or key in keep_fields}
        insertion_result = self.timeSeries.insert_one(message)
        print(f"Data inserted into MongoDB with id: {insertion_result.inserted_id}")

        