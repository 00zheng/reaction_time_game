int startButton = 10;
int stopButton  = 9;

int redLED    = 13;
int yellowLED = 12;
int greenLED  = 11;

enum State { IDLE, RED, YELLOW, GREEN, DONE };
State state = IDLE;

unsigned long stateStart = 0;     // when the current state began
unsigned long randDelay = 0;      // random delay for red & yellow
unsigned long greenTime = 0;      // timestamp when green turned on

void setup() {
  pinMode(startButton, INPUT_PULLUP);
  pinMode(stopButton, INPUT_PULLUP);

  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  Serial.begin(9600);
  randomSeed(analogRead(A0));     // helps randomness (optional but good)
}

void setLights(bool r, bool y, bool g) {
  digitalWrite(redLED, r);
  digitalWrite(yellowLED, y);
  digitalWrite(greenLED, g);
}

void loop() {
  unsigned long now = millis();

  // --- START button: only works in IDLE ---
  if (state == IDLE && digitalRead(startButton) == LOW) {
    randDelay = random(800, 3000);
    state = RED;
    stateStart = now;
    setLights(true, false, false);
    delay(200); // simple debounce is fine here
  }

  // --- State machine ---
  if (state == RED) {
    if (now - stateStart >= randDelay) {
      state = YELLOW;
      stateStart = now;
      setLights(false, true, false);
    }
  }
  else if (state == YELLOW) {
    if (now - stateStart >= randDelay) {
      state = GREEN;
      stateStart = now;
      greenTime = now;                 // ✅ timer starts exactly here
      setLights(false, false, true);
    }
  }
  else if (state == GREEN) {
    // ✅ Stop button works WHILE green is on because we are NOT blocked
    if (digitalRead(stopButton) == LOW) {
      unsigned long reactionTime = now - greenTime;
      Serial.print("Reaction time: ");
      Serial.print(reactionTime);
      Serial.println(" ms");

      state = DONE;
      stateStart = now;
      setLights(false, false, false);
      delay(200); // debounce
    }

    // Optional: auto turn off green after 3 seconds even if no stop press
    if (now - stateStart >= 3000) {
      state = DONE;
      stateStart = now;
      setLights(false, false, false);
    }
  }
  else if (state == DONE) {
    // return to IDLE after a short pause
    if (now - stateStart >= 300) {
      state = IDLE;
    }
  }
}
