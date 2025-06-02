const form = document.querySelector("form");
form.addEventListener("submit", (e) => {
  if (!form.checkValidity()) {
    e.preventDefault(); // Prevent form submission
    form.reportValidity(); // Show the browser's validation message
    return ;//stop the rest of execution
  }
  
    const formData = new FormData(form);
    let confirm_text = "Αυτές είναι οι τιμές που θα αποθηκευτούν:";

    for (const [name, value] of formData.entries()) {
      confirm_text =+ name + ":" + value + "\n";
    }
    if (confirm(confirm_text) != true){
            return ;
    }
    
    const fetchPromise = fetch("/submitSourcePollutantDetails",{
      method :"POST",
      body : formData
    })
    genericHandlerResponse(fetchPromise,"POST");
});