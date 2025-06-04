

from pymongo import MongoClient,errors
import os
import csv
from zoneinfo  import ZoneInfo
from datetime import datetime
from typing import Literal,Optional

list_of_csv_variables = ('Timestamp','Id','Sensor',
                         "BME680:iaq","BME680:iaqAccuracy","BME680:staticIaq","BME680:co2Equivalent","BME680:co2EquivalentAccuracy",
                         "BME680:breathVocEquivalent","BME680:breathVocEquivalentAccuracy",
                         "BME680:rawTemperature","BME680:pressure","BME680:rawHumidity",
                         "BME680:gasResistance","BME680:stabStatus","BME680:runInStatus","BME680:temperature",
                         "BME680:humidity","BME680:gasPercentage","BME680:gasPercentageAccuracy",
                         "CCS811:eCO2","CCS811:TVOC","CCS811:RawResistance","CCS811:30minutesPassed"
                         )  


class ServerFunctions:

    list_of_User_Input_Categories= ('EnvRoomData',"ExperimentState")

    list_of_User_Input_Type_In_Category_Experiment_State = ('Any','StartingExperiment','InsertingSourcePollutant','RemovingSourcePollutant')

    csv_file_path = os.path.join('data', 'sensor_data.csv')  # This points to root_project_file/data/sensor_data.csv

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
        os.makedirs(os.path.dirname(self.csv_file_path), exist_ok=True)
        file_exists = os.path.exists(self.csv_file_path)
        print(f"File exists: {file_exists}")
        if not file_exists:
            with open(self.csv_file_path, mode='w', newline='') as csv_file:
                csv_writer = csv.writer(csv_file)
                csv_writer.writerow(self.list_of_csv_variables)
                print("Header written to new CSV file.")
    
     


    #get the last user input from a particular category from the database
    def getLastUserInput(self,userInputCategory: Literal['EnvRoomData',"ExperimentState"] ,
                         userInputType: Optional[Literal['Any','StartingExperiment','InsertingSourcePollutant','RemovingSourcePollutant']] = 'Any' ):
        if userInputCategory not in self.list_of_User_Input_Categories:
            raise ValueError(f"Invalid user input category: {userInputCategory}. Must be one of {self.list_of_User_Input_Categories}.")
        
        if userInputCategory == 'ExperimentState' and userInputType not in self.list_of_User_Input_Type_In_Category_Experiment_State:    
            raise ValueError(f"Invalid user input type for category ExperimentState." 
                             f"Must be one of {self.list_of_User_Input_Type_In_Category_Experiment_State}.")
        if userInputCategory == 'EnvRoomData':
            
            lastUserInputElement = self.UserInput.find_one({'userInputCategory': 'EnvRoomData'},sort=[("timestamp",-1)])
               
        
        elif userInputCategory == 'ExperimentState' and userInputType == 'Any':
            lastUserInputElement= self.UserInput.find_one({'userInputCategory': "ExperimentState"},sort=[("timestamp",-1)])
                                        
           
        elif userInputCategory == 'ExperimentState' and userInputType != 'Any':
            lastUserInputElement= self.UserInput.find_one({'userInputCategory': "ExperimentState",
                                        'experimentState': userInputType},sort=[("timestamp",-1)])  
        lastUserInputElement["timestamp"] =lastUserInputElement["timestamp"].isoformat()   
        return lastUserInputElement
        
    def getCurrentDateTime(self):
        # Get the current date and time in the local timezone
        currentDateAndTime = datetime.now(self.zoneInfo)
       # print (f"Current date and time: {currentDateAndTime}")
        return currentDateAndTime 
    #get current date and time in ISO format at the local timezone
    def getCurrentDateTimeISOformat(self):
        timeStamp =datetime.isoformat(self.getCurrentDateTime())
        return timeStamp

    def getReadableDateTimeFromISOformat(self,isoDateTime):
        # Convert ISO format to datetime object
        dt = datetime.fromisoformat(isoDateTime)
        # Format the datetime object to a readable string
        formatted_date = dt.strftime("%d %B %Y, %H:%M")
        return formatted_date 
           
