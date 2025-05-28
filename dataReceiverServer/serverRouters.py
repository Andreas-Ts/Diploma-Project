from flask import Flask, request,Response, jsonify, render_template
from serverFunctions import ServerFunctions 
from markupsafe import escape
from datetime import datetime
from zoneinfo import ZoneInfo
from pymongo import errors
import os
from typing import Optional

class serverRouters:
    def __init__(self):
        self.srvFunc = ServerFunctions()
      
    def get(self):
        # Placeholder for GET request handling
        pass    
    def post(self):
        # Placeholder for POST request handling
        pass    
    def getIndex(self):
          
           

            # Get the last environment data
            env_last_reading = self.getLastUserInputHandler("EnvRoomData")
            # In real use, fetch from your data source; here we simulate no history
            print(env_last_reading)
            if env_last_reading is not None:
                print("reading found for EnvRoomData.")

                return render_template(
                    'base.html',                  
                    env_last_reading=env_last_reading 
                )
            else:
                return render_template(
                    'base.html'
                )
     
    def postIndex(self):
        print("Received POST request")
        print(f"Request from: {request.remote_addr}")  # Log client IP

        try:
            data = request.get_json(force=True)
            print(f"Received JSON message: {data}")
            self.srvFunc.saveData(data)
        except Exception as e:
            print(f"Error: {e}")
            return  "Invalid JSON", 400

        return "Message received", 200
    def getReadableDateTimeFromISOformat(self,isoDateTime):
        # Convert ISO format to datetime object
        dt = datetime.fromisoformat(isoDateTime)
        # Format the datetime object to a readable string
        formatted_date = dt.strftime("%d %B %Y, %H:%M")
        return formatted_date
   

    def getLastUserInputHandler(self,userInputCategory,userInputType:Optional[str]=None):
        try:
            return self.srvFunc.getLastUserInput(userInputCategory, userInputType)
        except ValueError as e:
            print(f"ValueError occurred: {e}")
            Response.code = 400
            return render_template('base.html', temp_error=str(e))
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            Response.code = 500
            return render_template('base.html', temp_error=str(e))    
        
class indexRouter(serverRouters):  
    def __init__(self):
        super().__init__()
    

   
class getlastUserInputExperimentState(serverRouters):
    def __init__(self):
        super().__init__()
    def get(self,ExperimentState):
        returnState = self.srvFunc.getLastUserInput("ExperimentState",ExperimentState)
        returnState.pop('_id', None)
        print(f"Last user input for ExperimentState '{ExperimentState}': {returnState}") 
        return  jsonify(returnState) if returnState else jsonify({"noPreviousInput": True}), 200

    
       
class postUserInput(serverRouters):
    def __init__(self):
        super().__init__()
    def get(self):
        return render_template('insertPollutantSource.html')
    def post(self,ExperimentState):
        try:
            if ExperimentState not in self.srvFunc.list_of_User_Input_Type_In_Category_Experiment_State or ExperimentState is 'Any':
                return render_template('base.html', temp_error="Invalid Experiment State for an input.") 
            timestamp = self.srvFunc.getCurrentDateTimeISOformat() 
            if ExperimentState == "InsertingSourcePollutant":
                last_insertion = self.insertingSourceHandler(timestamp)  
            else:
                last_insertion = {
                    'timestamp': timestamp,
                    'userInputCategory': "ExperimentState",
                    'experimentState': ExperimentState
                }
            self.srvFunc.UserInput.insert_one(last_insertion)
            return  jsonify({"status": "OK"})
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            Response.code = 500
            return render_template('base.html', temp_error=str(e))    

    def insertingSourceHandler(self,timestamp):
        pass 

class submitenvRoomDataRouter(serverRouters):
    def __init__(self):
        super().__init__()
    def get(self):
        last_reading = self.getLastUserInputHandler("EnvRoomData")
        last_reading.timestamp = self.getReadableDateTimeFromISOformat(last_reading['timestamp'])
        return render_template(
            'submitEnvRoomData.html',
            last_reading = last_reading           
        )

    def post(self):
        try:
            temperature = int(request.form['temperature'])
            humidity = int(request.form['humidity'])

            # Basic input validation
            if not (0 <= humidity <= 100):
                return render_template('index.html', temp_error="Humidity must be between 0% and 100%.")

            if temperature < -40 or temperature > 65:
                return render_template('index.html', temp_error="Temperature cannot be handled by the sensors.")

            # Sanitize inputs
            temperature = escape(temperature)
            humidity = escape(humidity)
            timestamp = self.getCurrentDateTimeISOformat()
           

            # Save to MongoDB
            self.srvFunc.UserInput.insert_one({
                'timestamp': timestamp,
                'userInputCategory': "EnvRoomData",
                'temperature': temperature,
                'humidity': humidity
            })
            #create a last reading object 
            last_reading = {
                'timestamp': self.getReadableDateTimeFromISOformat(timestamp),
                'userInputCategory': "EnvRoomData",
                'temperature': temperature,
                'humidity': humidity,
            }
            Response.code = 200
            return render_template('submitEnvRoomData.html',last_reading =last_reading)
    
        except ValueError:
            Response.code = 400
            return render_template('submitEnvRoomData.html', temp_error="Invalid input values.")
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            Response.code = 500
            return render_template('submitEnvRoomData.html', temp_error="Database error.")