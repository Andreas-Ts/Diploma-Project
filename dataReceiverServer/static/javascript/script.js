 document.addEventListener('DOMContentLoaded', () => {

    fetch('/lastUserInputExperimentState/Any')
        .then(res => {
          if (!res.ok) throw new Error('Network response was not ok');
          return res.json();
        })
        .then(response => {
          data.experimentState
          const startTime = new Date(data.timestamp);
          if (isNaN(startTime)) {
            throw new Error('Invalid datetime from server');
          }
          startTimer(startTime);
        })
        .catch(err => {
          console.error('Failed to load start time:', err);
          // optionally show an error message in the UI
        });
 

 });

 function checkExperimentState(response){
    
    const experimentState = document.getElementById("experiment_state");
    const pastTime = new Date(response.timestamp);     // parsed into Date object the ISO Object
    const now = new Date();                 // current time
    const diffInMs = now - pastTime;        // difference in milliseconds

    const minutesPassed = diffInMs / (1000 * 60);    // convert to minutes
    const hoursPassed = diffInMs / (1000 * 60 * 60); // convert to hours
    if  (hoursPassed > 2 ) {
        experimentState.textContent = "Πρέπει να ξεκινήσει καινούργιος κύκλος πειραμάτων"
        return 
    }
    else 

 }