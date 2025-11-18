// EchoBot Project: ESP8266 + HC-SR04 + Buzzer + 2 LEDs + Laser

// The project uses an HC-SR04 ultrasonic sensor to measure distances.
// When an obstacle is detected, the system:
// - turns on 2 LEDs
// - activates 1 Buzzer
// - turns on 1 Laser

// HC-SR04 SENSOR PINS
const int TRIG_PIN = D6;  // HC-SR04 Pin (GPIO12)
const int ECHO_PIN = D5;  // HC-SR04 Pin (GPIO14)

// LED PINS
const int LED1_PIN = D1;  // LED 1 connected to pin D1 (GPIO5)
const int LED2_PIN = D2;  // LED 2 connected to pin D2 (GPIO4)

// BUZZER PIN
const int BUZZER_PIN = D7;  // Active Buzzer connected to pin D7 (GPIO13)

// LASER PIN
const int LASER_PIN = D3;  // Laser connected to pin D3 (GPIO0)

// MOVEMENT DETECTION DISTANCE
float detection_threshold_cm = 35.0;  // If distance <= 35cm, activate LEDs, buzzer, and laser

// AI SUGGESTION - SMOOTHING
// Used to avoid erroneous readings. Take 5 quick measurements and average them.
const int NUM_SAMPLES = 5;  // Number of measurements
const int SETTLE_TIME_MS = 50;  // Wait time (ms) between each sample

// SPEED OF SOUND CONSTANT
const float SOUND_SPEED = 0.0343f;

// BUZZER CONFIGURATION
int buzzerFrequency = 2000;  // Sound frequency in Hz
int buzzerBeepDuration = 100; // Duration of each beep (ms)
int buzzerPauseDuration = 100;  // Pause between beeps (ms)
unsigned long lastBuzzerToggleTime = 0;  // Last buzzer state change time
bool isBuzzerActive = false;  // Current buzzer state (true = playing, false = paused)

// SETUP FUNCTION
// Executed at the beginning, it configures all pins and their states.
void setup() {
  // PIN MODE CONFIGURATION
  pinMode(TRIG_PIN, OUTPUT);  // TRIG is output because I send the ultrasonic pulse
  pinMode(ECHO_PIN, INPUT); // ECHO is input because it receives the sound echo
  pinMode(LED1_PIN, OUTPUT);  // LED1 is output, activated when sound is interrupted
  pinMode(LED2_PIN, OUTPUT);  // LED2 is output, activated when sound is interrupted
  pinMode(BUZZER_PIN, OUTPUT);  // Buzzer is output, emits sounds when movement is detected
  pinMode(LASER_PIN, OUTPUT); // Laser is output, emits laser when movement is detected

  // INITIAL STATE OF PINS (ALL OFF)
  digitalWrite(TRIG_PIN, LOW);    
  digitalWrite(LED1_PIN, LOW);   
  digitalWrite(LED2_PIN, LOW);    
  digitalWrite(BUZZER_PIN, LOW);  
  digitalWrite(LASER_PIN, LOW);  

  // START SERIAL MONITOR COMMUNICATION
  Serial.begin(9600);  // Starts serial communication at 9600 baud - SERIAL MONITOR MUST BE 9600, OTHERWISE MESSAGES WILL NOT APPEAR CORRECTLY
  delay(200);         
  Serial.println("EchoBot ON");
}

// FUNCTION TO READ DISTANCE IN CENTIMETERS
// This function sends an ultrasonic pulse and measures the time until the echo is received
float readDistanceCM() {
  // Ensure TRIGGER pin is LOW
  digitalWrite(TRIG_PIN, LOW);   
  delayMicroseconds(2);         
  
  // Set TRIGGER pin HIGH, keep active for 10us, then turn off
  digitalWrite(TRIG_PIN, HIGH);  
  delayMicroseconds(10);         
  digitalWrite(TRIG_PIN, LOW); 

  // CALCULATE ECHO WAIT TIME
  // Maximum distance of 2 meters, calculates the time.
  // If the sound returns faster, it means an obstacle was found and activates the EchoBot.
  const unsigned long MAX_DISTANCE_CM = 200;
  unsigned long maxEchoTime = (unsigned long)((MAX_DISTANCE_CM * 2.0f) / SOUND_SPEED);
  
  // MEASURE THE TIME IT TAKES FOR THE SOUND TO RETURN
  // pulseIn() measures how long the ECHO pin stays HIGH (round trip time of sound)
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, maxEchoTime);

  // VERIFICATION AND DISTANCE CALCULATION
  if (duration == 0) return NAN;  // If no echo, return NAN (Not A Number)
  
  // Formula: distance = (time × speed_of_sound) / 2
  // We divide by 2 because the sound travels back and forth
  return (duration * SOUND_SPEED) / 2.0f;
}

// FUNCTION TO READ DISTANCE WITH SMOOTHING (AVERAGE OF SEVERAL READINGS) - AI SUGGESTED SMOOTHING
// This function takes several readings and calculates the average to get a more stable value
float readAveragedDistanceCM() {
  int validReadings = 0; // Counter for valid readings
  float sumOfReadings = 0.0f; // Sum of readings
  
  // COLLECT 5 SAMPLES - 'NUM_SAMPLES' variable was defined at the beginning
  for (int i = 0; i < NUM_SAMPLES; i++) {
    float distance = readDistanceCM();  // Take a reading
    
    // If the reading is valid (not NAN), add to the sum
    if (!isnan(distance)) { 
      sumOfReadings += distance; 
      validReadings++; 
    }
    
    delay(SETTLE_TIME_MS);  // Wait between readings - variable defined at the beginning.
  }
  
  // CALCULATION AND RETURN OF THE AVERAGE
  // If there is at least one valid reading, return the average
  // Otherwise, return NAN
  return (validReadings > 0) ? (sumOfReadings / validReadings) : NAN;

        // EQUIVALENT TO 
        //   if (validReadings > 0) {
        //     return sumOfReadings / validReadings;
        // } else {
        //     return NAN;
        // }
}

// FUNCTION TO CONTROL THE LEDs
// Turns both LEDs on or off simultaneously
void setLEDs(bool on) {
  digitalWrite(LED1_PIN, on ? HIGH : LOW);  // If on=true, turn on (HIGH), otherwise turn off (LOW)
  digitalWrite(LED2_PIN, on ? HIGH : LOW);  // Same behavior for LED2

  // The expressions are equivalent to if statements. If ON is HIGH, it proceeds, otherwise it returns LOW.
}

// FUNCTION TO CONTROL THE LASER
// Turns the laser module on or off
void setLaser(bool on) {
  digitalWrite(LASER_PIN, on ? HIGH : LOW); // If on=true, turn on, otherwise turn off

  // Equivalent to an if statement.
}

// FUNCTION TO CONTROL THE BUZZER BEEP PATTERN
// Creates an intermittent beep pattern when shouldBeep is true
void updateBuzzer(bool shouldBeep) {
  // IF IT SHOULD NOT BEEP, TURN EVERYTHING OFF
  if (!shouldBeep) {
    noTone(BUZZER_PIN); // Stop any sound on the buzzer
    isBuzzerActive = false;  // Mark state as off
    return;
  }

  // CREATION OF THE INTERMITTENT BEEP PATTERN
  unsigned long currentTime = millis(); // Get current time in milliseconds
  
  if (isBuzzerActive) {
    // BUZZER IS PLAYING - CHECK IF IT SHOULD STOP
    if (currentTime - lastBuzzerToggleTime >= buzzerBeepDuration) {
      noTone(BUZZER_PIN); // Stop the sound
      isBuzzerActive = false;  // Mark as paused
      lastBuzzerToggleTime = currentTime;  // Record the time of state change
    }
  } else {
    // BUZZER IS PAUSED - CHECK IF IT SHOULD PLAY AGAIN
    if (currentTime - lastBuzzerToggleTime >= buzzerPauseDuration) {
      tone(BUZZER_PIN, buzzerFrequency); // Start sound at defined frequency
      isBuzzerActive = true;  // Mark as playing
      lastBuzzerToggleTime = currentTime;  // Record the time of state change
    }
  }
}

// LOOP FUNCTION - EXECUTED CONTINUOUSLY
void loop() {
  // DISTANCE READING
  float distance = readAveragedDistanceCM();  

  // CHECK IF READING IS VALID
  if (isnan(distance)) {
    // NO VALID READING - TURN EVERYTHING OFF
    setLEDs(false);  // Turn off LEDs
    updateBuzzer(false);  // Turn off buzzer
    setLaser(false);  // Turn off laser
    Serial.println("No valid reading, please check the sensor.");
  } else {
    // VALID READING - CHECK IF OBJECT IS NEAR
    bool isNear = (distance <= detection_threshold_cm);  // true if distance <= 35cm
    
    // Activate or deactivate components based on proximity
    setLEDs(isNear);  // Turn on LEDs 
    updateBuzzer(isNear);  // Activate beeps 
    setLaser(isNear);  // Turn on laser 

    // SEND INFORMATION
    Serial.print("Distance: ");
    Serial.print(distance, 1);  // Show distance with 1 decimal place
    Serial.print(" cm  -> LEDs ");
    Serial.print(isNear ? "ON" : "OFF");
    Serial.print(" Buzzer ");
    Serial.print(isNear ? "PLAYING" : "OFF");
    Serial.print(" Laser ");
    Serial.println(isNear ? "ON" : "OFF");
  }

  // PAUSE BEFORE NEXT ITERATION
  delay(20);  // Wait 20ms before taking a new reading
}