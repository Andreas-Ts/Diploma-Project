function genericHandlerResponse(fetchPromise,request_type) {
    fetchPromise.then(response => {
            if (!response.ok) throw new Error("Request failed");
            window.location.href = "/";
        })
        .catch(error => {
            console.error("Error:", error);
            alert("Failed to send "+String(request_type) +" request.");
        });
}