
    
// 2) start the interval timer
function startTimer(startTime) {
    fetch('/lastUserInput')
        .then(res => {
          if (!res.ok) throw new Error('Network response was not ok');
          return res.json();
        })
        .then(data => {
          const startTime = new Date(data.datetime);
          if (isNaN(startTime)) {
            throw new Error('Invalid datetime from server');
          }
          startTimer(startTime);
        })
        .catch(err => {
          console.error('Failed to load start time:', err);
          // optionally show an error message in the UI
        });

  const $h = document.getElementById('hours');
  const $m = document.getElementById('minutes');
  const $s = document.getElementById('seconds');
}
        function update() {
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

        // first immediate update
        update();
        // then update every second
        setInterval(update, 1000);
      }
    