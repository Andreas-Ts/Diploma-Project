
from flask import Flask, request, jsonify, render_template
import serverRouters
app = Flask(__name__)
# Load description text from file once


# Function to run the server
def server_routers():
    # Initialize the server routers
    
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
    @app.route("/lastUserInputExperimentState/<ExperimentState>", methods=["GET", "POST"])
    def lastUserInputExperiment(ExperimentState):
        router = serverRouters.lastUserInputExperimentState()
        if request.method == "GET":
            return router.get(ExperimentState)
        if request.method == "POST":
            return router.post(ExperimentState)
        
    
      
    @app.route("/submitEnvRoomData", methods=["GET", "POST"])
    def submitEnvRoomData():
        router = serverRouters.submitenvRoomDataRouter()
        if request.method == "GET":
            return router.get()
        if request.method == "POST":
            return router.post()
 
# Run the server if this file is executed directly
if __name__ == "__main__":
    # Initialize the server functions
    
    server_routers()
    app.run(host="0.0.0.0", port=8080, debug=True)
