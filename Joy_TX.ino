///This is the Transmitter side code (transmits signals made by the movement of joystick) /// 

#include <SPI.h>
#include <RF24.h>

// NRF24L01 Pin Definitions
#define CE_PIN 7
#define CSN_PIN 8
/// connect sck -13 mosi-11 miso-12 
// buttonstate
int buttonstate;
// Joystick Pin Definitions
#define JOYSTICK_X A0    // X-axis of joystick
#define JOYSTICK_Y A1    // Y-axis of joystick
#define JOYSTICK_SW 2    // Joystick button/switch

// Additional button for special functions (optional)
#define EXTRA_BUTTON 3   

// Create an RF24 object
RF24 radio(CE_PIN, CSN_PIN);

// Address through which two modules communicate
const byte address[6] = "00001";

// Data structure to hold joystick and button values
struct RobotControlData {
  int x;          // X-axis value
  int y;          // Y-axis value
  bool button;    // Joystick button state
  bool extraBtn;  // Extra function button
};

RobotControlData controlData;

void setup() {
  Serial.begin(9600);
  
  // Initialize joystick and button pins
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  pinMode(JOYSTICK_SW, INPUT_PULLUP);  // Internal pull-up for button
  
  // Optional extra button
  pinMode(EXTRA_BUTTON, INPUT_PULLUP);
  
  // Initialize the NRF24L01
  if (!radio.begin()) {
    Serial.println("Radio hardware not responding!");
    while (1) {} // Hold in infinite loop
  }
  
  // Set the transmitter configuration
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX for maximum range
  radio.setDataRate(RF24_250KBPS); // Slower data rate for better range
  radio.setChannel(76);  // Set to channel 76 to avoid Wi-Fi interference
  radio.stopListening();  // Set as transmitter
  
  Serial.println("Robot Car Transmitter initialized");
}

void loop() {
  // Read joystick values
  controlData.x = analogRead(JOYSTICK_X);
  controlData.y = analogRead(JOYSTICK_Y);
  
  // Read button states (inverted due to pull-up)
  controlData.button = !digitalRead(JOYSTICK_SW);
  controlData.extraBtn = !digitalRead(EXTRA_BUTTON);
  
  // Send the control data
  bool report = radio.write(&controlData, sizeof(controlData));
  
  // Debug output
  Serial.print("X: ");
  Serial.print(controlData.x);
  Serial.print(" Y: ");
  Serial.print(controlData.y);
  Serial.print(" Joystick Button: ");
  Serial.print(controlData.button);
  Serial.print(" Extra Button: ");
  Serial.print(controlData.extraBtn);
  Serial.print(" Transmission: ");
  Serial.println(report ? "Success" : "Failed");
  
  // Small delay to prevent overwhelming transmission
  delay(50);
}
