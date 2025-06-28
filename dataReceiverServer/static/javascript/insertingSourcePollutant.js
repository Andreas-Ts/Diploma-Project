const form = document.querySelector("form");
console.log(form);
form.addEventListener("submit", (e) => {
   e.preventDefault();
  if (checkWallDistanceValidity()){
         return ;
  }
  if (!form.checkValidity()) {
    form.reportValidity(); // Show the browser's validation message
    return ;//stop the rest of execution
  }
    trimInputs(form);
    const formData = new FormData(form);
    
    let confirm_text = createConfirmText(formData);

    
    console.log(confirm_text);
    if (confirm(confirm_text) != true){
            return ;
    }
    
    const fetchPromise = fetch("/submitSourcePollutantDetails",{
      method :"POST",
      body : formData
    })
    genericHandlerResponse(fetchPromise,"POST");
});


function createConfirmText(formData){
   let confirm_text = "Αυτές είναι οι τιμές που θα αποθηκευτούν:";
  for (let [name, value] of formData.entries()) {
      switch(name){
        case "pollutant-type":
           name = "Τύπος ρύπου";
           break;
        case "item-used":
           name = "Αντικείμενο προέλευσης";
           break;
        case "quantity-used":
           name = "Ποσότητα που χρησιμοποιείται";
           break;
        case "room":
           name = "Δωμάτιο";
           break;  
        case "front-wall":
           name = "Απόσταση από μπροστινό τοίχο σε μέτρα";
           break;
        case "side-right-wall":
           name = "Απόσταση από δεξίο τοίχο σε μέτρα";
           break;
        case "are-windows-opened":
           name = "Είναι ανοιχτά τα παράθυρα";   
           value = "Ναι";
           break;
        case "are-doors-opened":
           name = "Είναι ανοιχτές οι πόρτες";   
           value = "Ναι";
           break;
        case "are-people-inside":
           name = "Είναι άνθρωποι μέσα";   
           value = "Ναι";
           break;      
      }
     console.log(name);

      confirm_text = confirm_text + name   + ":" + value + "\n";
     
    }
     return confirm_text;
}

function trimInputs(form){

    for (const element of form.elements) {
      if (element.type === "text") {
        element.value = element.value.trim();
      }
    }
   
}

function checkWallDistanceValidity(){
    const front = parseFloat(document.getElementById('front-wall').value) || 0;
    const back = parseFloat(document.getElementById('back-wall').value) || 0;
    const right = parseFloat(document.getElementById('side-right-wall').value) || 0;
    const left = parseFloat(document.getElementById('side-left-wall').value) || 0;

    // At least one of front or back must be filled with a value > 0
    if (front <= 0 && back <= 0) {
        alert("Συμπληρώστε είτε την απόσταση από τον μπροστινό τοίχο είτε από τον πίσω τοίχο.");
        return false;
    }

    // At least one of side-right or side-left must be filled with a value > 0
    if (right <= 0 && left <= 0) {
        alert("Συμπληρώστε είτε την απόσταση από τον δεξιό τοίχο είτε από τον αριστερό τοίχο.");
        return false;
    }

    return true; // Allow form submission
}