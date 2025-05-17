
from flask import Flask, request, jsonify, render_template
import serverRouters
app = Flask(__name__)
# Load description text from file once


# Function to run the server
def server_routers():
    # Initialize the server routers
    indexRouter = serverRouters.indexRouter()
    @app.route("/", methods=["GET","POST"])
    def index():
        if request.method == "GET":
           return indexRouter.get()

        
        elif request.method == "POST":
          return  indexRouter.post()
    @app.route("/submit", methods=["GET"])
    def submit():
        # Placeholder for GET request handling
        return jsonify({"message": "GET request received"}), 200

# Run the server if this file is executed directly
if __name__ == "__main__":
    # Initialize the server functions
    
    server_routers()
    app.run(host="0.0.0.0", port=8080, debug=True)
