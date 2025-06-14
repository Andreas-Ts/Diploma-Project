from flask import Flask, redirect, request,Response, jsonify, render_template, send_file, url_for
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
            env_last_reading = self.getLastUserInputHandler("EnvRoomData",is_front_end_account_for_local_timezone = 'No')
            # In real use, fetch from your data source; here we simulate no history
            if env_last_reading is not None:

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

    
class utilityUrl(serverRouters):
    def __init__(self):
        super().__init__()
    def get(self,utilityUrl):
        if utilityUrl == "getLogs":
            return self.getLogs()
        elif utilityUrl == "description":
            return render_template("description.html")
        elif utilityUrl == "checkSensorStability":
            return self.checkStability()
        else:
            return render_template('base.html', temp_error="Invalid utility URL.")
    def post(self,utilityUrl):
        pass
    def getLogs(self):
        log_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "logs"))
        log_file_path = os.path.join(log_dir, "server_requests.log")
        if not os.path.exists(log_file_path):
            return render_template('base.html', temp_error="Log file does not exist.")
        
        return send_file(log_file_path, as_attachment=True)
    def checkStability(self):
        try:
            total_result ={"id:0":self.srvFunc.checkSensorStability(0),
                        "id:1":self.srvFunc.checkSensorStability(1),
                        "id:2":self.srvFunc.checkSensorStability(2),
            }
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            return  jsonify({"error": "No data available"}), 500
        if total_result is None:
            print("No data available")
            return jsonify({"error": "No data available"}), 500
        else:
            return jsonify(total_result),200


class getlastUserInputExperimentState(serverRouters):
    def __init__(self):
        super().__init__()
    def get(self,ExperimentState):
        returnState = self.getLastUserInputHandler("ExperimentState",ExperimentState,is_front_end_account_for_local_timezone = "No")
        
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
            pipeline = [{
                #Step 0: take all the documents with the category inside experimentState 
                "$match" :{
                    "userInputCategory":"ExperimentState",
                    "experimentState":"InsertingSourcePollutant"
                }
            },
                # Step 1: Flatten 'details' fields into root level
                {
                    "$project": {
                        "pollutant-type": "$details.pollutant-type",
                        "item-used": "$details.item-used",
                        "quantity-used": "$details.quantity-used",
                        "room": "$details.room",
                        "front-wall": "$details.front-wall",
                        "side-right-wall": "$details.side-right-wall",
                        "are-windows-opened": "$details.are-windows-opened",
                        "timestamp": 1  # preserve timestamp for sorting
                    }
                },
                   {  
                        "$sort": {
                            "timestamp": -1
                        }
                    },
                
                # Step 3: Group to get distinct + latest values
                {
                    "$group": {
                        "_id": None,
                        "pollutant-type-distinct-list": { "$addToSet": "$pollutant-type" },
                        "item-used-distinct-list": { "$addToSet": "$item-used" },
                        "quantity-used-distinct-list": { "$addToSet": "$quantity-used" },
                        "room-distinct-list": { "$addToSet": "$room" },

                        "pollutant-type-latest_value": { "$first": "$pollutant-type" },
                        "item-used-latest_value": { "$first": "$item-used" },
                        "quantity-used-latest_value": { "$first": "$quantity-used" },
                        "room-latest_value": { "$first": "$room" },
                        "front-wall-latest_value": { "$first": "$front-wall" },
                        "side-right-wall-latest_value": { "$first": "$side-right-wall" },
                        "are-windows-opened-latest_value": { "$first": "$are-windows-opened" }
                    }
                }
            ]
          
            if self.getLastUserInputHandler("ExperimentState","InsertingSourcePollutant") is None:
                print("noeeeeee")
                return render_template('insertingSourcePollutant.html')
            else:   
                details = list(self.srvFunc.UserInput.aggregate(pipeline))
                print(details)
                return render_template('insertingSourcePollutant.html',details = details[0])
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
        env_last_reading = self.getLastUserInputHandler("EnvRoomData",is_front_end_account_for_local_timezone = 'No')
        
        return render_template(
            'submitEnvRoomData.html',
            env_last_reading = self.beautifyTimestamp(env_last_reading)          
        )

    def post(self):
        env_last_reading = None
        try:
            temperature = float(request.form['temperature'])
            humidity = float(request.form['humidity'])

            # Basic input validation
            if not (0 <= humidity <= 100):
                return render_template('index.html', temp_error="Humidity must be between 0% and 100%.")

            if temperature < -40 or temperature > 65:
                return render_template('index.html', temp_error="Temperature cannot be handled by the sensors.")

            # Sanitize inputs
            temperature = escape(temperature)
            humidity = escape(humidity)
            timestamp = self.srvFunc.getCurrentDateTimeUTC()
           

            # Save to MongoDB
            self.srvFunc.UserInput.insert_one({
                'timestamp': timestamp,
                'userInputCategory': "EnvRoomData",
                'temperature': temperature,
                'humidity': humidity
            })
            #create a last reading object 
            env_last_reading = {
                'timestamp': self.srvFunc.getReadableDateTimeFromISOformat(self.srvFunc.makeDateCorrectForJavascript(timestamp)),
                'userInputCategory': "EnvRoomData",
                'temperature': temperature,
                'humidity': humidity,
            }
            
            return redirect(url_for('index',env_last_reading= env_last_reading,temp_success="Η θερμοκρασία αποθηκεύτηκε σωστά" ))
    
        except ValueError as e:
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
        start_timestamp_epoch = request.args.get("start")
        end_timestamp_epoch =   request.args.get("end")
        if start_timestamp_epoch is not None and end_timestamp_epoch is not None:
            start_timestamp_datetime = self.srvFunc.getDateTimeFromEpoch(start_timestamp_epoch)
            end_timestamp_datetime = self.srvFunc.getDateTimeFromEpoch(end_timestamp_epoch)

        if endpoint == "checkConnection":
            return jsonify({"response": "ok"}), 200  

        if endpoint == "getEnvRoomData":
           
            if start_timestamp_epoch is None or end_timestamp_epoch is None:
                return self.getEnvRoomDataRaw()
            else:
                return self.getEnvRoomDataMultipeEntries(start_timestamp_datetime,end_timestamp_datetime)
        elif endpoint == "getTimeSeriesData":
            if start_timestamp_epoch is None or end_timestamp_epoch is None:
                return jsonify({"error": "Start and end timestamps are required"}), 400
            else:
                return self.getTimeSeriesData(start_timestamp_datetime,end_timestamp_datetime)
        
    def post(self,endpoint):

        if endpoint == "postTimeSeriesData":
            return self.postTimeSeriesData()
        if endpoint == "postError":
            return self.postError()
        
    def getTimeSeriesData(self,start_timestamp_datetime,end_timestamp_datetime):
        
        try:
            
            id = request.args.get("Id")
            if id is not None:
                id = int(id)
            pipeline = self.srvFunc.pipelineForTakingDataBasedOfStartAndEndTime(start_timestamp_datetime,end_timestamp_datetime,id = id)                
            response_docs = list(self.srvFunc.timeSeries.aggregate(pipeline))
            return jsonify(json.loads(json_util.dumps(response_docs))), 200 
        
        except ValueError:
            return jsonify({"error": "Invalid url query arguments"}), 400       
        except errors.PyMongoError as e:
            print(f"PyMongoError occurred: {e}")
            return jsonify({"error": str(e)}), 500  
            
    def getEnvRoomDataRaw(self):
        try:
            
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

    def  getEnvRoomDataMultipeEntries(self,start_timestamp_datetime,end_timestamp_datetime):

       
        pipeline = self.srvFunc.pipelineForTakingDataBasedOfStartAndEndTime(start_timestamp_datetime,end_timestamp_datetime,
        amIcheckingUserInputDataOfEnvRoomData = True)
        #add an extra match state
        
        response_docs = list(self.srvFunc.UserInput.aggregate(pipeline))
        return jsonify(json.loads(json_util.dumps(response_docs))), 200 
        


    def postTimeSeriesData(self):
        try: 
            data = request.get_json()
            if not data:
              return jsonify({"error": "Missing or invalid JSON data"}), 400
            data['timestamp'] = self.srvFunc.getCurrentDateTime()

            #Insert the data into the database,deleting the irrelevant fields
            #keep the Timestamp,Id and Sensor fields for both sensors
            keep_fields = ['timestamp', 'Id', 'Sensor']
             #trim from bme680 some fields to save storage space
            trim_fields = ["BME680:co2EquivalentAccuracy","BME680:breathVocEquivalentAccuracy",
                           "BME680:rawTemperature","BME680:pressure","BME680:rawHumidity",
                           "BME680:stabStatus","BME680:runInStatus","BME680:gasPercentageAccuracy"
                           ] 


            # Create a new dictionary with only the fields we want to keep
            data = {key: value 
                    for key, value in data.items() 
                    if  (key.startswith(data["Sensor"]) and key not in trim_fields) or (key in keep_fields)}


            self.srvFunc.timeSeries.insert_one(data)
            return jsonify({"status": "OK"}), 200
        except errors.PyMongoError as e:
            print(f"An error occurred while inserting data into MongoDB: {e}")
            return jsonify({"error": "Failed to insert data into MongoDB"}), 500    
        
        except ValueError as e:
            print(f"ValueError occurred: {e}")
            Response.code = 400
            return jsonify({"error": str(e)}), 400
        
        except Exception as e:
            print(f"Unexpected error: {e}")
            return jsonify({"error": "Internal server error"}), 500
        
    def postError(self):
        error =request.get_json()
        print("Error from "+error['id']+":" + error["error_message"])
        return jsonify({"response" :"ok"}),200
class dataAnalysisEndpoints(serverRouters):
    def __init__(self):
        super().__init__()  

    def get(self,endpoint):
        if endpoint == "getAllUserInputsExperimentState":
           return self.getAllUserInputsExperimentState()
    def getAllUserInputsExperimentState(self):  
        try:
            docs = list(self.srvFunc.UserInput.find({"userInputCategory":"ExperimentState"}))
        except Exception as e:
            print(f"Unexpected error: {e}")
            return jsonify({"error": "Internal server error"}), 500  
        return jsonify(json.loads(json_util.dumps(docs))), 200 
    