
from flask import Flask, request, jsonify
from serverFunctions import ServerFunctions

app = Flask(__name__)
handler = ServerFunctions()


# Function to run the server
def run_server():
    @app.route("/", methods=["GET,POST"])
    def index():
        if request.method == "GET":
            pass #leave empty for now

        
        elif request.method == "POST":
            print("Received POST request")
            print(f"Request from: {request.remote_addr}")  # Log client IP

            try:
                data = request.get_json(force=True)
                print(f"Received JSON message: {data}")
                handler.saveData(data)
            except Exception as e:
                print(f"Error: {e}")
                return  "Invalid JSON", 400

            return "Message received", 200
    

# Run the server if this file is executed directly
if __name__ == "__main__":
    ServerFunctions = ServerFunctions()  # Initialize the server functions
    
    app.run(host="0.0.0.0", port=8080, debug=True)
