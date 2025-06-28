import requests
import os
import json

#some constants to be used

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
    data = getData(url)
    # Writing JSON to a file
    with open('data/'+data_file_name+".json", 'w') as file:
        json.dump(data, file)

base_url = "http://192.168.28.129:8080/"
first_part_endpoint = "dataAnalysisEndpoints/"
second_part_endpoint = "/getAllUserInputsExperimentState"        

url = create_url(base_url,first_part_endpoint,second_part_endpoint)

saveDataIntoDataFolder(url,"UserInputs")