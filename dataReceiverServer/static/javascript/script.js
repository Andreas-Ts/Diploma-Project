// In future you can poll an API endpoint to update the status dynamically.
// For now this is just a placeholder.

document.getElementsByClassName("submit-Env-Room-Data")[0].addEventListener("submit", function(event) {
  event.preventDefault(); // Stop normal form submission
  
    // Submit form via JS
    this.submit();

    // Redirect (slight delay to ensure submission gets processed)
    setTimeout(() => {
      window.location.href = '/';
    }, 500); // Adjust time as needed
})