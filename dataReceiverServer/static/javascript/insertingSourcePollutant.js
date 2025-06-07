const form = document.querySelector("form");
form.addEventListener("submit", (e) => {
   e.preventDefault(); 
  if (!form.checkValidity()) {
    form.reportValidity(); // Show the browser's validation message
    return ;//stop the rest of execution
  }
  
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