import requests
import os
import json
from pathlib import Path

#some constants to be used
script_dir = Path(__file__).resolve().parent

def create_url(base_url,first_part_endpoint,second_part_endpoint):
    return base_url + first_part_endpoint + second_part_endpoint
def getData(url):
    response = requests.get(url)

    # Check response status
    if response.status_code == 200:
        data = response.json()
        print(f"Received {len(data)} records.")
        return data
    else:
        print(f"Error: {response.status_code}")

def saveDataIntoDataFolder(url,data_file_name):
    data_folder = script_dir / 'data'
    
    data_folder.mkdir(exist_ok=True)
    
    file_path = data_folder / (data_file_name + ".json")
    
    data = getData(url)
    
    if data:
        with open(file_path, 'w') as file:
            json.dump(data, file)
        print(f"Data saved to {file_path}")
    else:
        print("No data to save.")

base_url = "http://192.168.28.129:8080/"
first_part_endpoint = "dataAnalysisEndpoints/"
second_part_endpoint = "getAllUserInputsExperimentState"        

url = create_url(base_url,first_part_endpoint,second_part_endpoint)

saveDataIntoDataFolder(url,"UserInputs")

