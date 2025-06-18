function downloadLogFile() {
  fetch('/getLogs')  // or the correct URL you defined
    .then(response => {
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      return response.blob();  // Get response as a Blob (file-like object)
    })
    .then(blob => {
      // Create a temporary URL for the blob
      const url = window.URL.createObjectURL(blob);

      // Create a temporary <a> element to trigger download
      const a = document.createElement('a');
      a.href = url;
      
      // Set the filename (you can hardcode or try to read from headers)
      a.download = 'server_requests.log';
      
      // Append <a> to the DOM, trigger click, then remove it
      document.body.appendChild(a);
      a.click();
      a.remove();

      // Revoke the object URL to free memory
      window.URL.revokeObjectURL(url);
    })
    .catch(error => {
      console.error('Download error:', error);
      alert('Failed to download logs.');
    });
}
 
 document.addEventListener('DOMContentLoaded', () => {

    fetch('/lastUserInputExperimentState/Any')
        .then(res => {
          if (!res.ok) throw new Error('Network response was not ok');
          return res.json();
        })
        .then(response => {
          console.log(response);
          checkExperimentState(response);
        
        })
        .catch(err => {
          console.error('error', err);
          // optionally show an error message in the UI
        });
 

 });

 function checkExperimentState(response){
    const experimentStateText = document.getElementById("experiment_state");


    //just in case of no previous experiment state
    if (areAllSensorsStable(experimentStateText, document.getElementById("submitExperimentState")) == false){
        return ;
    }
    if (response.noPreviousInput == true) {
        submitExperimentStateClickListener("http://localhost:8080/postUserInput/StartingExperiment","POST","StartingExperiment")
        experimentStateText.textContent = "Πρέπει να ξεκινήσει καινούργιος κύκλος πειραμάτων";
        
        return ;
    }
    
    console.log(response.timestamp);
    const startTime = new Date(response.timestamp);     // parsed into Date object the ISO Object
    console.log(startTime);
    if (isNaN(startTime)) {
            throw new Error('Invalid datetime from server');
          }
    const now = new Date();                 // current time
    const diffInMs = now - startTime;        // difference in milliseconds
    console.log(now);
    console.log(startTime);
    console.log(diffInMs);
    const minutesPassed = diffInMs / (1000 * 60);    // convert to minutes
    const hoursPassed = diffInMs / (1000 * 60 * 60); // convert to hours
    console.log(`Time since last experiment: ${minutesPassed} minutes, ${hoursPassed} hours`);
    if  (hoursPassed > 2 ) {
        submitExperimentStateClickListener("http://localhost:8080/postUserInput/StartingExperiment","POST","StartingExperiment");
        experimentStateText.textContent = "Πρέπει να ξεκινήσει καινούργιος κύκλος πειραμάτων";
        return ;
    }
    else if (minutesPassed < 1 && (response.experimentState === "StartingExperiment" || 
             response.experimentState === "RemovingSourcePollutant")) {
            //document.getElementById("submitExperimentState").disabled = true;
            experimentStateText.textContent = "Άσε τον χώρο να αεριστεί για 5 λεπτά.";
            document.getElementById("timer-container").hidden = false;
            document.getElementById("timer-container").querySelector("h1").textContent= "Χρόνος που πέρασε από την εισαγωγή δήλωσης του πειράματος";
            startTimer(startTime,"5 Minutes");
            return ;
    }
    else if (response.experimentState !== "InsertingSourcePollutant" && minutesPassed > 1 ) {
        submitExperimentStateClickListener("http://localhost:8080/postUserInput/InsertingSourcePollutant","GET","InsertingSourcePollutant");
        experimentStateText.textContent = "Εισήγαγε την πηγή ρύπου";
        return ;
    }
    else if (response.experimentState === "InsertingSourcePollutant"){
         
         experimentStateText.textContent = "Άσε τον ρύπο να εξελιχτεί με βάση τις συνθήκες που δώσατε για τουλάχιστον 30 λεπτά."+
         "Στα 45 λεπτά, θα εμφανίστει και ακουστεί ειδοποίηση για να βγάλετε την πηγή.\n"+
         "Αν έγινε κάποιο λάθος στην εισαγωγή πηγής, πάτα το κουμπί \"Διαγραφή της προηγούμενης εισαγωγής ρύπου\".";
          createDeleteButton();  
          document.getElementById("timer-container").hidden = false;
          document.getElementById("timer-container").querySelector("h1").textContent= "Χρόνος που πέρασε από την εισαγωγή της πηγής ρύπου";
          submitExperimentStateClickListener("postUserInput/RemovingSourcePollutant","POST","RemovingSourcePollutant")

        startTimer(startTime,"30 Minutes");


    }

 }
function areAllSensorsStable(experimentStateText,submitExperimentStateElement){
      fetch('/checkSensorStability')
      .then(res =>{
        if (!res.ok) throw new Error('Network response was not ok');
       return res.json();
      })
      .then(response => {
        console.log(response);
        if ("error" in response){
            experimentStateText.textContent = "Προέκυψε πρόβλημα στον σέρβερ" + response.error;
            submitExperimentStateElement.disabled = true;
            submitExperimentStateElement.textContent = "";
            return false;
        }
        finalText = checkEveryPosibilityForSensorStability(response);
        if (finalText == ""){
            return true;
        }
        else{
          experimentStateText.textContent = finalText;
          submitExperimentStateElement.disabled = true;
          submitExperimentStateElement.textContent = "Οι αισθητήρες δεν είναι ακόμα σταθεροί.Δες περιγραφή για την σταθεροποίηση τους";
          return false;
        }
       
      }).catch(err => {
          console.error('error', err);
          // optionally show an error message in the UI
        });
 
      }



 function submitExperimentStateClickListener(url,method,experimentStateToBeSubmitted) {
    
    const experimentStateButton = document.getElementById("submitExperimentState");
    if (method === "POST") {
          if (experimentStateToBeSubmitted === "StartingExperiment") {
            experimentStateButton.textContent = "Υποβολή καινούργιου κύκλου πειράματος.";
                    
          }
          else if (experimentStateToBeSubmitted === "RemovingSourcePollutant"){
            experimentStateButton.textContent = "Απομάκρυνση πηγής ρύπου και τερματισμός κύκλου πειράματος.";
          }
          else {
            return ;
          }
          experimentStateButton.addEventListener("click", () => {
            const timestamp = new Date().toISOString();

            const fetchPromise = fetch(url, {
                    method: "POST",
                    headers: {
                    "Content-Type": "application/json"
                    },
                    body: JSON.stringify({ timestamp,experimentStateToBeSubmitted })
                })
                 genericHandlerResponse(fetchPromise,"POST");

            })

    }
    if (method === "GET") {
        experimentStateButton.textContent = "Εισήγαγε την πηγή ρύπου";
        experimentStateButton.onclick = () => {
            location.href= "/submitSourcePollutantDetails";  
    }

 }
}

function createDeleteButton(){
      console.log("hii from delete");
        deleteButton = document.createElement("button");
         deleteButton.textContent = "Διαγραφή της προηγούμενης εισαγωγής πηγής ρύπου";
         deleteButton.className = "delete-button";
         deleteButton.id = "delete-last-source-insertion";
        
        deleteButton.addEventListener("click",(event)=>{
             confirm_text= "Είσαι σίγουρος ότι θέλεις να ακυρώσεις την προηγούμενη εισαγωγή πηγής ρύπου;"+
             "Τα δεδομένα που μετρήθηκαν κατά την διάρκεια δεν θα υπολογιστούν σαν ενεργές μετρήσεις."+
             "Μπορείς να εισηγάγεις κατευθείαν καινούργια πηγή, αλλά ανοιξέ τα παράθυρα για σιγουριά.";
             if (confirm(confirm_text) != true){
                return ;
             }
             const fetchPromise = fetch("/lastUserInputExperimentState/InsertingSourcePollutant",{method : 'DELETE'});
             
              genericHandlerResponse(fetchPromise,"DELETE");

        });
        document.getElementById("submitExperimentState").insertAdjacentElement('afterend',deleteButton);
}

function checkEveryPosibilityForSensorStability(response) {

    let finalText = "";
    
    finalText += checkEveryPosibilityForSensorStabilityOnePerTime(response, 0,"CCS811 με την λευκή βάση");
    finalText += checkEveryPosibilityForSensorStabilityOnePerTime(response, 1,"ΒΜΕ680 με την διαφανή βάση");
    finalText += checkEveryPosibilityForSensorStabilityOnePerTime(response, 2,"BME680 με την κίτρινη βάση");
   
    
    return finalText;
}


function checkEveryPosibilityForSensorStabilityOnePerTime(response, sensorId,sensorDescription) {
    let finalText = "";
    let id_field = "id:" + sensorId;
    console.log(id_field);
    /*
    if (!(id_field in response) || response[id_field] == null){
          finalText+="Δεν υπάρχει ο αισθητήρας "+ sensorDescription +" στην βάση δεδομένων για τα τελευταια 30 λεπτά.";
          return finalText;
        }

    else if (response[id_field].has_full_30_min_data == false){
        finalText += "Ο αισθητήρας " +sensorDescription+" δεν έχει σταθεροποιηθεί ακόμα. Πρέπει να έχεις τουλαχιστον 30 λεπτά δεδομένων για να θεωρηθεί σταθερός.";
    }
    else if (response[id_field].all_accuracy_2_or_3 == false){
        finalText += "Ο αισθητήρας " +sensorDescription+ " δεν έχει σταθεροποιηθεί ακόμα. Πρέπει να έχεις τουλαχιστον 30 λεπτά δεδομένων με ακρίβεια 2 ή 3 για να θεωρηθεί σταθερός.";

    }
        */
    return finalText;
}