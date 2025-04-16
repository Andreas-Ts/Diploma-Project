from http.server import BaseHTTPRequestHandler, HTTPServer
import os

# Path to save the CSV file
CSV_PATH = os.path.join('data', 'sensor_data.csv')  # This points to root_project_file/data/sensor_data.csv

class CSVRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        try:
            # Read the latest CSV file from disk
            with open(CSV_PATH, "rb") as csv_file:  # Read in binary mode
                csv_data = csv_file.read()
            
            # Send headers to force download
            self.send_response(200)
            self.send_header("Content-Type", "text/csv; charset=utf-8")
            self.send_header("Content-Disposition", f'attachment; filename="{os.path.basename(CSV_PATH)}"')
            self.send_header("Content-Length", str(len(csv_data)))
            self.end_headers()

            # Send the CSV file content
            self.wfile.write(csv_data)
        
        except FileNotFoundError:
            self.send_error(404, "File not found")
        except Exception as e:
            self.send_error(500, f"Server error: {str(e)}")


if __name__ == "__main__":
    server_address = ("", 8000)
    httpd = HTTPServer(server_address, CSVRequestHandler)
    print("Serving live-updating CSV on port 8000...")
    httpd.serve_forever()            