
    
// 2) start the interval timer
function startTimer(startTime) {
 

  // first immediate update
  update(startTime);
  // then update every second
  setInterval(() => update(startTime), 1000);
}
function update(startTime) {
     
        const $h = document.getElementById('hours');
        const $m = document.getElementById('minutes');
        const $s = document.getElementById('seconds');
        const now = new Date();
        let diff = Math.floor((now - startTime) / 1000); // total seconds elapsed
         
        const hours   = Math.floor(diff / 3600);
        diff %= 3600;
        const minutes = Math.floor(diff / 60);
        const seconds = diff % 60;

        // pad with leading zeros
        $h.textContent = String(hours).padStart(2, '0');
        $m.textContent = String(minutes).padStart(2, '0');
        $s.textContent = String(seconds).padStart(2, '0');
        /*
        if (state==="5 Minutes" && minutes > 5){
          console.log("Time to insert source pollutant");
          window.location.href = "/"
        }
        
        if (state==="30 Minutes"){
                  console.log("hiaaa");

          submit_button = document.getElementById("submitExperimentState");
          if (minutes < 30 && hours < 1){

             //submit_button.disabled = true; 
          }
          else{

            submit_button.disabled = false; 
          }
          if (minutes > 45){
            let originalTitle = document.title;
              let alertTitle = "!ΠΡΕΠΕΙ ΝΑ ΒΓΕΙ Η ΠΗΓΗ ΡΥΠΟΥ!";

              setInterval(() => {
                document.title = (document.title === alertTitle) ? originalTitle : alertTitle;
              }, 1000);
              
              // Stop flashing after some time (e.g., 10 seconds)
              setTimeout(() => {
                clearInterval(flashInterval);
                document.title = originalTitle;
              }, 1000);
              
             
          }
        }
        */
   }

      
    