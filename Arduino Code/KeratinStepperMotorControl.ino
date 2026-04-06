/*
  ESP32 + DRV8825 Syringe Pump Controller (SoftAP mode)
  - ESP32 creates its own Wi-Fi network (no router).
  - Connect your laptop/phone to that network, then open http://192.168.4.1
  - Control: Start/Pause, Direction, Speed (mL/hr), Syringe size
  - Smooth pulses via hardware timer @1 MHz.
  - UI won’t overwrite inputs while you type (polling pauses on focus).

  Mechanics:
    - Stepper: 200 steps/rev × 64 microsteps = 12,800 µsteps/rev
    - Worm gear: 2 mm/rev
    - => 6,400 µsteps per mm
    - Base formula for half-period (µs):
        halfPeriodUs = (281250 * mm_per_mL) / (mL/hr)

  Syringe size (mm per mL):
    - Example: 1 mL per 1 mm  -> factor = 1
    - Example: 1 mL per 2 mm -> factor = 2
*/

#include <WiFi.h>
#include <WebServer.h>

// ============ SoftAP CONFIG ============
const char* AP_SSID = "ESP32-Syringe";  
const char* AP_PASS = "";               
const uint8_t AP_CHANNEL = 6;
const bool AP_HIDDEN = false;
const uint8_t AP_MAX_CONN = 4;
// ======================================

// GPIOs
const int DIR_PIN  = 16;
const int STEP_PIN = 17;

// State
volatile uint32_t halfPeriodUs = 2812; // default ≈100 mL/hr with 1 mm/mL
volatile bool     running      = false;
volatile bool     dirCW        = true;
volatile float    syringe_mm_per_mL = 1.0;  // default syringe: 1 mm/mL

WebServer server(80);

// Timer
hw_timer_t* timer = nullptr;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile bool stepState = false;

// ================== ISR ==================
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  if (running) {
    stepState = !stepState;
    if (stepState) {
      GPIO.out_w1ts = (1UL << STEP_PIN);
    } else {
      GPIO.out_w1tc = (1UL << STEP_PIN);
    }
  } else {
    stepState = false;
    GPIO.out_w1tc = (1UL << STEP_PIN);
  }
  timerAlarmWrite(timer, halfPeriodUs, true);
  portEXIT_CRITICAL_ISR(&timerMux);
}

// ================== HTML UI ==================
const char* PAGE = R"HTML(
<!doctype html>
<html>
<head>
<meta charset="utf-8"/>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
<title>ESP32 Syringe Pump</title>
<style>
  body{font-family:system-ui,Segoe UI,Arial,sans-serif;max-width:720px;margin:40px auto;padding:0 16px}
  h1{font-size:1.25rem;margin-bottom:12px}
  .card{border:1px solid #ddd;border-radius:12px;padding:16px;box-shadow:0 2px 6px rgba(0,0,0,.06)}
  .row{display:flex;gap:12px;align-items:center;margin:12px 0;flex-wrap:wrap}
  label{min-width:160px}
  input[type="number"],select{width:160px;padding:6px}
  button{padding:8px 12px;cursor:pointer;border-radius:8px;border:1px solid #ccc;background:#f7f7f7}
  .on{background:#d7ffe0;border-color:#6ccf86}
  .off{background:#ffe0e0;border-color:#ff8c8c}
  .muted{color:#666}
</style>
</head>
<body>
  <h1>ESP32 Syringe Pump</h1>
  <div class="card">
    <div class="row">
      <label>Status:</label>
      <span id="status" class="muted">loading...</span>
    </div>
    <div class="row">
      <label>Run / Pause:</label>
      <button id="btnRun" class="on">Start</button>
      <button id="btnPause" class="off">Pause</button>
    </div>
    <div class="row">
      <label>Direction:</label>
      <select id="dir">
        <option value="1">Clockwise (DIR=HIGH)</option>
        <option value="0">Counter-Clockwise (DIR=LOW)</option>
      </select>
    </div>
    <div class="row">
      <label>Syringe size:</label>
      <select id="syringe">
        <option value="1">1 mm = 1 mL</option> <!-- CHANGE THESE FOR CHANGING SYRINGE OPTIONS -->
        <option value="2">2 mm = 1 mL</option> <!-- CHANGE THESE FOR CHANGING SYRINGE OPTIONS -->
        <option value="0.5">0.5 mm = 1 mL</option> <!-- CHANGE THESE FOR CHANGING SYRINGE OPTIONS -->
      </select>
    </div>
    <div class="row">
      <label>Flow Rate (mL/hr):</label>
      <input id="mlhr" type="number" min="0.1" max="100000" step="0.1"/>
    </div>
    <div class="row">
      <button id="apply">Apply</button>
    </div>
    <p class="muted">Enter flow in mL/hr. Conversion depends on syringe size.</p>
  </div>

<script>
let editing=false, editingDir=false, editingSyr=false;
const statusEl = document.getElementById('status');
const dirEl    = document.getElementById('dir');
const mlhrEl   = document.getElementById('mlhr');
const syringeEl= document.getElementById('syringe');

let pollId = null;
function startPoll(){ stopPoll(); pollId=setInterval(getStatus,1000); }
function stopPoll(){ if(pollId){ clearInterval(pollId); pollId=null; } }

mlhrEl.addEventListener('focus', ()=>{ editing=true; stopPoll(); });
mlhrEl.addEventListener('blur',  ()=>{ editing=false; startPoll(); });
dirEl.addEventListener('focus',  ()=>{ editingDir=true; stopPoll(); });
dirEl.addEventListener('blur',   ()=>{ editingDir=false; startPoll(); });
syringeEl.addEventListener('focus',  ()=>{ editingSyr=true; stopPoll(); });
syringeEl.addEventListener('blur',   ()=>{ editingSyr=false; startPoll(); });

async function getStatus(){
  try{
    const r = await fetch('/status',{cache:'no-store'});
    const j = await r.json();
    statusEl.textContent = (j.running?'Running':'Paused')
        +' | Dir: '+(j.dir?'CW':'CCW')
        +' | Syringe: '+j.mm_per_ml+' mm/mL'
        +' | Flow: '+j.mlhr.toFixed(1)+' mL/hr';
    if(!editingDir) dirEl.value = j.dir ? '1' : '0';
    if(!editingSyr) syringeEl.value = j.mm_per_ml;
    if(!editing)    mlhrEl.value = j.mlhr.toFixed(1);
  }catch(e){
    statusEl.textContent = 'Disconnected';
  }
}

async function apply(){
  const dir  = dirEl.value;
  const mlhr = mlhrEl.value;
  const syr  = syringeEl.value;
  await fetch('/set?dir='+dir+'&mlhr='+mlhr+'&syr='+syr,{cache:'no-store'});
  getStatus();
}

async function run(){ await fetch('/run?state=1',{cache:'no-store'}); getStatus(); }
async function pause(){ await fetch('/run?state=0',{cache:'no-store'}); getStatus(); }
document.getElementById('apply').addEventListener('click', apply);
document.getElementById('btnRun').addEventListener('click', run);
document.getElementById('btnPause').addEventListener('click', pause);

// Apply on Enter
mlhrEl.addEventListener('keydown', (e)=>{ if(e.key==='Enter'){ apply(); mlhrEl.blur(); }});

getStatus();
startPoll();
</script>
</body>
</html>
)HTML";

// ================== HTTP ==================
void handleRoot() { server.send(200, "text/html", PAGE); }

void handleStatus() {
  float mlhr = (halfPeriodUs > 0) ? (281250.0f * syringe_mm_per_mL / halfPeriodUs) : 0;
  String json = "{";
  json += "\"running\":"; json += (running ? "true" : "false"); json += ",";
  json += "\"dir\":";     json += (dirCW   ? "true" : "false"); json += ",";
  json += "\"mlhr\":";    json += mlhr; json += ",";
  json += "\"mm_per_ml\":"; json += syringe_mm_per_mL;
  json += "}";
  server.send(200, "application/json", json);
}

void handleRun() {
  if (server.hasArg("state")) {
    int s = server.arg("state").toInt();
    portENTER_CRITICAL(&timerMux);
    running = (s != 0);
    portEXIT_CRITICAL(&timerMux);
  }
  server.send(200, "text/plain", "OK");
}

void handleSet() {
  if (server.hasArg("dir")) {
    bool d = (server.arg("dir").toInt() != 0);
    dirCW = d;
    digitalWrite(DIR_PIN, dirCW ? HIGH : LOW);
  }
  if (server.hasArg("syr")) {
    syringe_mm_per_mL = server.arg("syr").toFloat();
    if (syringe_mm_per_mL <= 0) syringe_mm_per_mL = 1.0;
  }
  if (server.hasArg("mlhr")) {
    float v = server.arg("mlhr").toFloat();
    if (v > 0) {
      portENTER_CRITICAL(&timerMux);
      halfPeriodUs = (uint32_t)((281250.0f * syringe_mm_per_mL) / v);
      timerAlarmWrite(timer, halfPeriodUs, true);
      portEXIT_CRITICAL(&timerMux);
    }
  }
  server.send(200, "text/plain", "OK");
}

// ================== Setup helpers ==================
void setupTimer() {
  timer = timerBegin(0, 80, true);   // prescaler 80 -> 1 MHz
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, halfPeriodUs, true);
  timerAlarmEnable(timer);
}

void setupPins() {
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, dirCW ? HIGH : LOW);
}

void setupAP() {
  WiFi.mode(WIFI_AP);
  bool ok = WiFi.softAP(AP_SSID, (strlen(AP_PASS)>=8)?AP_PASS:NULL, AP_CHANNEL, AP_HIDDEN, AP_MAX_CONN);
  (void)ok;
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP SSID: "); Serial.println(AP_SSID);
  if (strlen(AP_PASS)>=8) { Serial.print("AP PASS: "); Serial.println(AP_PASS); }
  Serial.print("AP IP: "); Serial.println(ip);
}

void setupServer() {
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/run", handleRun);     
  server.on("/set", handleSet);     
  server.begin();
  Serial.println("HTTP server started");
}

void setup() {
  Serial.begin(115200);
  setupPins();
  setupAP();
  setupServer();
  setupTimer();
}

void loop() {
  server.handleClient();
}
