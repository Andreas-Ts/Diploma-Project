
from flask import Flask, request, jsonify, render_template,Response
import serverRouters
import logging
import os


# Create subdirectory path (e.g., "logs/" relative to current file)
log_dir = os.path.join(os.path.dirname(__file__), 'logs')
os.makedirs(log_dir, exist_ok=True)  # Ensure the directory exists

log_file_path = os.path.join(log_dir, 'server_requests.log')
logging.basicConfig(
        filename=log_file_path,
        level=logging.INFO,
        format='%(asctime)s - %(levelname)s - %(message)s'
    )

app = Flask(__name__)

# Function to run the server
def server_routers():
    # Initialize the server routers
    # Set up logging
  
    @app.before_request
    def log_request_info():
        logging.info(
            f"{request.remote_addr} - {request.method} {request.path} - "
            f"Headers: {dict(request.headers)} "
        )
    @app.route("/", methods=["GET","POST"])
    def index():
        router = serverRouters.serverRouters()
        if request.method == "GET":
           return router.getIndex()   
        elif request.method == "POST":
           return  router.postIndex()
    @app.route("/description")
    def description():
        return render_template("description.html")    
    @app.route("/lastUserInputExperimentState/<ExperimentState>", methods=["GET","DELETE"])
    def lastUserInputExperiment(ExperimentState):
        router = serverRouters.getlastUserInputExperimentState()
        if request.method == "GET":
            print(f"Received GET request for last user input of type: {ExperimentState}")
            return router.get(ExperimentState)
        if request.method == "DELETE": 
            return router.delete(ExperimentState)


    @app.route("/postUserInput/<ExperimentState>", methods=["GET","POST"])  
    def postUserInput(ExperimentState):  
        router = serverRouters.postUserInput()
        if request.method == "GET":
            return router.get()
        elif request.method == "POST":
            return router.post(ExperimentState)

    @app.route("/submitSourcePollutantDetails",methods=["GET","POST"]) 
    def submitSourcePollutantDetails():
        router = serverRouters.submitSourcePollutantDetails()
        if request.method == "GET":
            return router.get()
        elif request.method == "POST":
            return router.post()




    @app.route("/submitEnvRoomData", methods=["GET", "POST"])
    def submitEnvRoomData():
        router = serverRouters.submitenvRoomDataRouter()
        if request.method == "GET":
            return router.get()
        if request.method == "POST":
            return router.post()

    @app.route("/timeSeriesEndpoints/<endpoint>", methods=["GET", "POST"])
    def timeSeriesEndpoints(endpoint):
        router = serverRouters.timeSeriesEndpoints()
        if request.method == "GET":
            return router.get(endpoint)
        if request.method == "POST":
            return router.post(endpoint)


    
 
# Run the server if this file is executed directly
if __name__ == "__main__":
    # Initialize the server functions
    
    server_routers()
    
    app.run(host="0.0.0.0", port=8080, debug=True)
