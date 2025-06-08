from flask import Flask, request,Response, jsonify, render_template
from serverFunctions import ServerFunctions 
from markupsafe import escape
from datetime import datetime
from zoneinfo import ZoneInfo

from pymongo import errors
import os
from bson import json_util
import json
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
            env_last_reading = self.getLastUserInputHandler("EnvRoomData",is_front_end_account_for_local_timezone = 'Yes')
            # In real use, fetch from your data source; here we simulate no history
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
        pass
   
   

    def getLastUserInputHandler(self,userInputCategory,userInputType:Optional[str]=None,
    is_front_end_account_for_local_timezone= 'Yes' ):
        try:
            last_user_input = self.srvFunc.getLastUserInput(userInputCategory, userInputType,is_front_end_account_for_local_timezone)
            self.srvFunc.logging.info(last_user_input)
            return last_user_input
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
        returnState = self.getLastUserInputHandler("ExperimentState",ExperimentState,is_front_end_account_for_local_timezone = "Yes")
        
        #print(f"Last user input for ExperimentState '{ExperimentState}': {returnState}") 
        if returnState is not None:
            returnState.pop('_id', None)
        #check if sensor have stabilized:
        
        return  jsonify(returnState) if returnState else jsonify({"noPreviousInput": True}), 200

    def delete(self,ExperimentState):
        try:
         inputToBeDeleted = self.getLastUserInputHandler("ExperimentState",ExperimentState)
        
        # print(f"Last user input to be deleted for experimentState:'{ExperimentState}': {inputToBeDeleted}") 
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
            if ExperimentState not in self.srvFunc.list_of_User_Input_Type_In_Category_Experiment_State or ExperimentState == 'Any':
                return render_template('base.html', temp_error="Invalid Experiment State for an input.") 
            timestamp = self.srvFunc.getCurrentDateTimeISOformat() 
            if ExperimentState != "InsertingSourcePollutant":
        
                last_insertion = {
                    'timestamp': datetime.fromisoformat(timestamp),
                    'userInputCategory': "ExperimentState",
                    'experimentState': ExperimentState
                }
                self.srvFunc.UserInput.insert_one(last_insertion)

                return  jsonify({"status": "OK"}),200

            else:   
                return  jsonify({"status": "Doesn't insert source pollutant that way"}),400

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
                    "room-list" : {"$addToSet": "room-list"},

                    "pollutant-type-latest_value":{"$first":"details.pollutant-type"},
                    "item-used-latest_value": {"$first": "item-used"},
                    "quantity-used-latest_value" : {"$first": "quantity-used"},
                    "room-latest_value" :{"$first" :"room"},
                    "front-wall-distance-latest_value": {"$first":"front-wall"},
                    "side-right-wall-distance-latest_value": {"$first":"side-right-wall"}
                    
                }}
            ]
            if self.getLastUserInputHandler("ExperimentState","InsertingSourcePollutant") is None:
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
            insertion = { 'timestamp': self.srvFunc.getCurrentDateTime(),
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
        env_last_reading = self.getLastUserInputHandler("EnvRoomData",is_front_end_account_for_local_timezone = 'Yes')
        
        return render_template(
            'submitEnvRoomData.html',
            env_last_reading = self.beautifyTimestamp(env_last_reading)          
        )

    def post(self):
        env_last_reading = None
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
            timestamp = self.srvFunc.getCurrentDate()
           

            # Save to MongoDB
            self.srvFunc.UserInput.insert_one({
                'timestamp': timestamp,
                'userInputCategory': "EnvRoomData",
                'temperature': temperature,
                'humidity': humidity
            })
            #create a last reading object 
            env_last_reading = {
                'timestamp': self.srvFunc.getReadableDateTimeFromISOformat(datetime.isoformat(timestamp)),
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
        
class timeSeriesEndpoints(serverRouters):
    def __init__(self):
        super().__init__()
    def get(self,endpoint):
        if endpoint == "getEnvRoomData":
            return self.getEnvRoomDataRaw()
        elif endpoint == "getTimeSeriesData":
            return self.getTimeSeriesData()
        
    def post(self,endpoint):

        if endpoint == "postTimeSeriesData":
            return self.postTimeSeriesData()
        
    def getTimeSeriesData(self):
        start_timestamp_epoch = request.args.get("start")
        end_timestamp_epoch =   request.args.get("end")
        is_the_front_end_timezone_naive= request.args.get("timezone_naive")
        if start_timestamp_epoch is None or end_timestamp_epoch is None:
            return jsonify({"error": "Start and end timestamps are required"}), 400
        try:
            print(start_timestamp_epoch, end_timestamp_epoch)
            start_timestamp = datetime.fromtimestamp((int(start_timestamp_epoch)/1000),  self.srvFunc.zoneInfo)
            end_timestamp= datetime.fromtimestamp((int(end_timestamp_epoch)/1000), self.srvFunc.zoneInfo)
       
            pipeline = [
                {"$match": {
                    "Id" : int(request.args.get("Id")),
                    "timestamp": {
                        "$gte": start_timestamp,
                        "$lte": end_timestamp
                    }
                }},
                {"$sort": {"timestamp": 1}},  # Sort by timestamp in ascending order
                
            ]
            if (is_the_front_end_timezone_naive == True):
                pipeline.append(  {"$addFields": {
                                    "timestamp": {
                                        "$dateToString": {
                                            
                                            "date": "$timestamp",
                                            "timezone" : "Europe/Athens"

                                        }
                                    }
                                }})   
            else:
                pipeline.append({"$addFields": {
                                    "timestamp": {
                                        "$dateToString": {
                                          
                                            "date": "$timestamp",

                                        }
                                    }
                                }})    
              
            response_docs = list(self.srvFunc.timeSeries.aggregate(pipeline))

            print(response_docs)
            return jsonify(json.loads(json_util.dumps(response_docs))), 200 
        except ValueError:
            return jsonify({"error": "Invalid timestamp format"}), 400       
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            return jsonify({"error": str(e)}), 500  
            
    def getEnvRoomDataRaw(self):
        try:
            insert_local_timezone = request.args.get("timezone_aware")
            
            last_env_room_reading = self.getLastUserInputHandler("EnvRoomData")
            if last_env_room_reading is None:
                env_data_response = None
            else:
                env_data_response = {"temperature":last_env_room_reading["temperature"] , "humidity":last_env_room_reading["humidity"]}    
            return jsonify(env_data_response)
        except ValueError as e:
            print(f"ValueError occurred: {e}")
            env_data_response = {"error":str(e)}
            return jsonify(env_data_response),400
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            env_data_response = {"error":str(e)}
            return jsonify(env_data_response),500  

    def postTimeSeriesData(self):
        try: 
            data = request.get_json()

            data['timestamp'] = self.srvFunc.getCurrentDateTime()

            #Insert the data into the database,deleting the irrelevant fields
            #keep the Timestamp,Id and Sensor fields
            keep_fields = ['timestamp', 'Id', 'Sensor']
            # Create a new dictionary with only the fields we want to keep
            data = {key: value 
                    for key, value in data.items() 
                    if  key.startswith(data["Sensor"]) or key in keep_fields}
           
            self.srvFunc.timeSeries.insert_one(data)
            return jsonify({"status": "OK"}), 200
        except errors.PyMongoError as e:
            print(f"An error occurred while inserting data into MongoDB: {e}")
            return jsonify({"error": "Failed to insert data into MongoDB"}), 500    
        
        except ValueError as e:
            print(f"ValueError occurred: {e}")
            Response.code = 400
            return jsonify({"error": str(e)}), 400
        
            