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
          console.error('Failed to load start time:', err);
          // optionally show an error message in the UI
        });
 

 });

 function checkExperimentState(response){
    const experimentStateText = document.getElementById("experiment_state");


    //just in case of no previous experiment state
    if (response.noPreviousInput == true) {
        submitExperimentStateClickListener("http://localhost:8080/postUserInput/StartingExperiment","POST")
        experimentStateText.textContent = "Πρέπει να ξεκινήσει καινούργιος κύκλος πειραμάτων";
        
        return ;
    }
    
    const startTime = new Date(response.timestamp);     // parsed into Date object the ISO Object
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
    else if (minutesPassed < 5 && (response.experimentState === "StartingExperiment" || 
             response.experimentState === "RemovingSourcePollutant")) {
            document.getElementById("submitExperimentState").disabled = true;
            experimentStateText.textContent = "Άσε τον χώρο να αεριστεί για 5 λεπτά.";
            document.getElementById("timer-container").hidden = false;
            document.getElementById("timer-container").querySelector("h1").textContent= "Χρόνος που πέρασε από την εισαγωγή δήλωσης του πειράματος";
            startTimer(startTime,"5 Minutes");
            return ;
    }
    else if (response.experimentState !== "InsertingSourcePollutant" && minutesPassed >=5  ) {
        submitExperimentStateClickListener("http://localhost:8080/postUserInput/InsertingSourcePollutant","GET","InsertingSourcePollutant");
        experimentStateText.textContent = "Εισήγαγε την πηγή ρύπου";
        return ;
    }
    else if (response.experimentState === "InsertingSourcePollutant"){
         
         experimentStateText.textContent = "Άσε τον ρύπο να εξελιχτεί με βάση τις συνθήκες που δώσατε για τουλάχιστον 20 λεπτά."+
         "Στα 45 λεπτά, θα εμφανίστει και ακουστεί ειδοποίηση για να βγάλετε την πηγή.\n"+
         "Αν έγινε κάποιο λάθος στην εισαγωγή πηγής, πάτα το κουμπί \"Διαγραφή της προηγούμενης εισαγωγής ρύπου\".";
          createDeleteButton();  
          document.getElementById("timer-container").hidden = false;
          document.getElementById("timer-container").querySelector("h1").textContent= "Χρόνος που πέρασε από την εισαγωγή της πηγής ρύπου";
          submitExperimentStateClickListener("postUserInput/RemovingSourcePollutant","POST","RemovingSourcePollutant")

        startTimer(startTime,"20 Minutes");


    }

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