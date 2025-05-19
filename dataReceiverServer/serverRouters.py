from flask import Flask, request, jsonify, render_template
from serverFunctions import ServerFunctions 
from markupsafe import escape
from datetime import datetime
from zoneinfo import ZoneInfo
from pymongo import errors
import os

class serverRouters:
    def __init__(self):
        self.srvFunc = ServerFunctions()
      
    def get(self):
        # Placeholder for GET request handling
        pass    
    def post(self):
        # Placeholder for POST request handling
        pass    
    #get current date and time in ISO format at the local timezone
    def getCurrentDateTimeISOformat(self):
        timeStamp =datetime.isoformat(datetime.now(ZoneInfo("Europe/Athens")))
        return timeStamp
    
    def getCurrentEnvData(self):
        try:    
            doc = self.srvFunc.UserInput.find(
                    {'userInputType': 'submitEnvRoomData'}
                ).sort('timestamp', -1).limit(1)
            
            lastEnvRoomData = list(doc)
            if lastEnvRoomData:
                    # Normalize the document into a simple object for the template
                    lr = lastEnvRoomData[0]
                    timestamp = datetime.fromisoformat(lr.get('timestamp'))
                    formatted_timestamp = timestamp.strftime("%d %B %Y, %H:%M") 
                    last_reading = {
                        'temperature': lr.get('temperature'),
                        'humidity': lr.get('humidity'),
                        'timestamp': formatted_timestamp
                    }
            else:
                last_reading = None

        except errors.PyMongoError as e:
                print(f"PyMongoError occurred: {e}")
                request.code = 500
                last_reading = None
        return last_reading
    
class indexRouter(serverRouters):  
    def __init__(self):
        super().__init__()
    def get(self):
            # Path to the directory where server.py is located
            base_dir = os.path.dirname(os.path.abspath(__file__))

          
            description_path = os.path.join(base_dir, 'content', 'description.txt')
            with open( description_path, encoding='utf-8') as f:
                page_description = f.read().strip()   
            # Placeholder: the current experiment state
            experiment_state = "In Progress"  # this would normally come from your backend logic

            # Get the last environment data
            last_reading = self.getCurrentEnvData()
            # In real use, fetch from your data source; here we simulate no history
            print(last_reading)
            
            return render_template(
                'base.html',
                page_title="Ο τίτλος μου",
                description=page_description,
                experiment_state=experiment_state,
                last_reading=last_reading
            )


    
    def post(self):
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

class envRoomDataRouter(serverRouters):
    def __init__(self):
        super().__init__()
    def envRoomDataInfoGet(self):
        # Handle GET request for room data
        pass

    def envRoomDataLastTimestampget(self):
        # Handle POST request for room data
        pass        

class submitenvRoomDataRouter(serverRouters):
    def __init__(self):
        super().__init__()
    def get(self):
        last_reading = self.getCurrentEnvData()
        return render_template(
            'submitEnvRoomData.html',
            last_reading = last_reading           
        )

    def post(self):
        try:
            temp_error = None
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

            # Save to MongoDB
            self.srvFunc.UserInput.insert_one({
                'timestamp': self.getCurrentDateTimeISOformat(),
                'userInputType': "submitEnvRoomData",
                'temperature': temperature,
                'humidity': humidity
            })

            return render_template('submitEnvRoomData.html', temperature=temperature, humidity=humidity,temp_error=temp_error)
    
        except ValueError:
            return render_template('submitEnvRoomData.html', temp_error="Invalid input values.")