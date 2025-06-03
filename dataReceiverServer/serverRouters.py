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
                    env_last_reading=self.beautifyTimestamp(env_last_reading)   
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

    def beautifyTimestamp(self,env_last_reading):
        if env_last_reading is not None:
            env_last_reading["timestamp"] = self.srvFunc.getReadableDateTimeFromISOformat(env_last_reading['timestamp'])
        return env_last_reading


    

   
class getlastUserInputExperimentState(serverRouters):
    def __init__(self):
        super().__init__()
    def get(self,ExperimentState):
        returnState = self.srvFunc.getLastUserInput("ExperimentState",ExperimentState)
        
        print(f"Last user input for ExperimentState '{ExperimentState}': {returnState}") 
        if returnState is not None:
            returnState.pop('_id', None)
        return  jsonify(returnState) if returnState else jsonify({"noPreviousInput": True}), 200

    def delete(self,ExperimentState):
        try:
         inputToBeDeleted = self.srvFunc.getLastUserInput("ExperimentState",ExperimentState)
        
         print(f"Last user input to be deleted for experimentState:'{ExperimentState}': {inputToBeDeleted}") 
         self.srvFunc.UserInput.delete_one({"_id": inputToBeDeleted["_id"]})   
         return render_template('base.html',temp_success = "Διαγράφτηκε με επιτυχία η τελευταία εισαγωγή του χρήστη.")
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            Response.code = 500
            return render_template('base.html', temp_error=str(e))  

    
       
class postUserInput(serverRouters):
    def __init__(self):
        super().__init__()
   
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

  

  

class submitSourcePollutantDetails(serverRouters):
    def __init__(self):
        super().__init__()
    def get(self):
        try:
            pipeline = [
                { "$sort" : {"timestamp" : -1} },
                {"$group":{
                    "_id": None,
                    "pollutant-type-distinct-list": {"$addToSet":"details.pollutant-type"},
                    "item-used-distinct-list": {"$addToSet": "item-used"},
                    "quantity-used-distinct-list" : {"$addToSet": "quantity-used"},
                    "pollutant-type-latest_value":{"$first":"details.pollutant-type"},
                    "item-used-latest_value": {"$first": "item-used"},
                    "quantity-used-latest_value" : {"$first": "quantity-used"},
                    "front-wall-distance-latest_value": {"$first":"front-wall"},
                    "side-right-wall-distance-latest_value": {"$first":"side-right-wall"}
                    
                }}
            ]
            if self.srvFunc.getLastUserInput("ExperimentState","InsertingSourcePollutant") is None:
                return render_template('insertingSourcePollutant.html')
            else:   
                details = list(self.srvFunc.timeSeries.aggregate(pipeline))
                return render_template('insertingSourcePollutant.html',details = details)
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            Response.code = 500
            return render_template('base.html',temp_error="Υπήρξε πρόβλημα με την βάση δεδομένων"+str(e))    
    
       
    def post(self):
        try:
            insertion = { 'timestamp': self.srvFunc.getCurrentDateTimeISOformat(),
                    'userInputCategory': "ExperimentState",
                    'experimentState': "InsertingSourcePollutant",
                    "details": request.form }
            self.srvFunc.UserInput.insert_one(insertion)
            return self.getIndex()
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            Response.code = 500
            return self.getIndex()        

class submitenvRoomDataRouter(serverRouters):
    def __init__(self):
        super().__init__()
    def get(self):
        env_last_reading = self.getLastUserInputHandler("EnvRoomData")
        
        return render_template(
            'submitEnvRoomData.html',
            env_last_reading = self.beautifyTimestamp(env_last_reading)          
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
            timestamp = self.srvFunc.getCurrentDateTimeISOformat()
           

            # Save to MongoDB
            self.srvFunc.UserInput.insert_one({
                'timestamp': timestamp,
                'userInputCategory': "EnvRoomData",
                'temperature': temperature,
                'humidity': humidity
            })
            #create a last reading object 
            env_last_reading = {
                'timestamp': self.srvFunc.getReadableDateTimeFromISOformat(timestamp),
                'userInputCategory': "EnvRoomData",
                'temperature': temperature,
                'humidity': humidity,
            }
            Response.code = 200
            return render_template('base.html',env_last_reading= env_last_reading,temp_success="Η θερμοκρασία αποθηκεύτηκε σωστά" )
    
        except ValueError:
            Response.code = 400
            return render_template('base.html',env_last_reading= env_last_reading,temp_error = "error"+ str(e) + "with the value")
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            Response.code = 500
            return render_template('base.html',env_last_reading= env_last_reading,temp_error =  "error"+ str(e) + "with the database")
        
class getEnvRoomDataRaw(serverRouters):
    def __init__(self):
        super().__init__()
    def get(self):
        try:
            last_env_room_reading = self.getLastUserInputHandler("EnvRoomData")
            if last_env_room_reading is None:
                env_data_response = None
            else:
                env_data_response = {"temperature":last_env_room_reading["temperature"] , "humidity":last_env_room_reading["humidity"]}    
            return env_data_response
        except ValueError as e:
            print(f"ValueError occurred: {e}")
            Response.code = 400
            env_data_response = {"error":e}
            return jsonify(env_data_response)
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            Response.code = 500
            env_data_response = {"error":e}
            return jsonify(env_data_response)
        