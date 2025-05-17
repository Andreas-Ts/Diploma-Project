from flask import Flask, request, jsonify, render_template
from serverFunctions import ServerFunctions 
import os

class serverRouters:
    def __init__(self):
        self.srvFunc = ServerFunctions()
    def get(self):
        # Placeholder for GET request handling
        return    
    def post(self):
        # Placeholder for POST request handling
        return    
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

            # Placeholder URL to fetch last temperature & humidity
            last_reading = {
                'temperature': None,
                'humidity': None
            }
            # In real use, fetch from your data source; here we simulate no history
            print(page_description)
            return render_template(
                'base.html',
                page_title="Ο τίτλος μου",
                description=page_description,
                experiment_state=experiment_state,
                last_reading=last_reading,
                submit_url="/submit"
            )


    
    def IndexPost(self):
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
    def envRoomDataInfoGet():
        # Handle GET request for room data
        pass

    def envRoomDataLastTimestampget():
        # Handle POST request for room data
        pass        