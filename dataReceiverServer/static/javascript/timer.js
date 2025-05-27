
    
// 2) start the interval timer
function startTimer(startTime) {
    
  const $h = document.getElementById('hours');
  const $m = document.getElementById('minutes');
  const $s = document.getElementById('seconds');

  // first immediate update
  update(startTime);
  // then update every second
  setInterval(update, 1000);
}
function update(startTime) {
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
   }

      
    