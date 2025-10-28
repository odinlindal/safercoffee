const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>SmarterCoffee</title>
  <style>
    body {
      margin: 0;
      padding: 0;
      font-family: 'Poppins', sans-serif;
      background: linear-gradient(135deg, #f6ede4 0%, #e0d3c2 100%);
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      color: #3b2f2f;
    }
    .card {
      background: rgba(255, 255, 255, 0.85);
      backdrop-filter: blur(10px);
      border-radius: 24px;
      box-shadow: 0 8px 30px rgba(0, 0, 0, 0.1);
      width: 360px;
      padding: 40px 30px;
      text-align: center;
      transition: transform 0.3s ease;
    }
    .card:hover { transform: translateY(-4px); }
    h1 { font-size: 2em; margin: 0; color: #4b3a33; }
    .mode { margin-top: 10px; font-size: 1.1em; color: #7a6a5f; }
    .timer { margin: 30px 0; font-size: 3.5em; font-weight: 600; color: #4b3a33; }
    .switch { position: relative; display: inline-block; width: 70px; height: 38px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider {
      position: absolute;
      cursor: pointer;
      top: 0; left: 0;
      right: 0; bottom: 0;
      background-color: #cbb7a7;
      transition: .4s;
      border-radius: 34px;
    }
    .slider:before {
      position: absolute;
      content: "";
      height: 30px;
      width: 30px;
      left: 4px;
      bottom: 4px;
      background-color: white;
      transition: .4s;
      border-radius: 50%;
    }
    input:checked + .slider { background-color: #7b5b42; }
    input:checked + .slider:before { transform: translateX(32px); }
    button {
      margin-top: 24px;
      background: #7b5b42;
      color: #fff;
      border: none;
      border-radius: 12px;
      padding: 12px 28px;
      font-size: 1em;
      cursor: pointer;
      transition: 0.3s;
    }
    button:hover { background: #5f4532; }
    footer { margin-top: 24px; font-size: 0.85em; color: #a18f81; }
  </style>
  <script>
    async function fetchStatus() {
      const res = await fetch('/status');
      const data = await res.json();
      document.getElementById('timer').textContent = data.remaining;
      document.getElementById('mode').textContent = data.mode + " min";
      document.getElementById('toggle').checked = data.is_on;
    }
    async function toggleMachine() {
      await fetch('/toggle');
      fetchStatus();
    }
    async function nextMode() {
      await fetch('/nextmode');
      fetchStatus();
    }
    setInterval(fetchStatus, 1000);
    window.onload = fetchStatus;
  </script>
</head>
<body>
  <div class="card">
    <h1>☕ SmarterCoffee</h1>
    <div class="mode">Mode: <span id="mode"></span></div>
    <div class="timer" id="timer">--:--</div>
    <label class="switch">
      <input type="checkbox" id="toggle" onchange="toggleMachine()">
      <span class="slider"></span>
    </label>
    <div><button onclick="nextMode()">Next Mode</button></div>
    <footer>Connected via Wi-Fi</footer>
  </div>
</body>
</html>
)rawliteral";
