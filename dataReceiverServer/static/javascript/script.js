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
        console.log="hi"
        return ;
    }
    const startTime = new Date(response.timestamp);     // parsed into Date object the ISO Object
      if (isNaN(startTime)) {
            throw new Error('Invalid datetime from server');
          }
    const now = new Date();                 // current time
    const diffInMs = now - startTime;        // difference in milliseconds

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
            startTimer(startTime);
            return ;
    }
    else if (minutesPassed >=5 && response.experimentState !== "InsertingSourcePollutant") {
        submitExperimentStateClickListener("http://localhost:8080/postUserInput/InsertingSourcePollutant","GET","InsertingSourcePollutant");
        experimentStateText.textContent = "Εισήγαγε την πηγή ρύπου";
        return ;
    }

 }

 function submitExperimentStateClickListener(url,method,experimentStateToBeSubmitted) {
    
    const experimentStateButton = document.getElementById("submitExperimentState");
    if (method === "POST") {
            experimentStateButton.textContent = "Υποβολή καινούργιου κύκλου πειράματος.";
            experimentState = "StartingExperiment";
            const timestamp = new Date().toISOString();
            experimentStateButton.addEventListener("click", () => {
                fetch(url, {
                    method: "POST",
                    headers: {
                    "Content-Type": "application/json"
                    },
                    body: JSON.stringify({ timestamp,experimentStateToBeSubmitted })
                })
                .then(response => {
                    if (!response.ok) throw new Error("Request failed");
                    window.location.reload(); // Reload the page on success
                })
                .catch(error => {
                    console.error("Error:", error);
                    alert("Failed to send POST request.");

                });
            })
    }
    if (method === "GET") {
        experimentStateButton.textContent = "Εισήγαγε την πηγή ρύπου";
        experimentStateButton.onclick = () => {
            location.href.url = "http://localhost:8080/postUserInput/InsertingSourcePollutant";  
    }

 }
}