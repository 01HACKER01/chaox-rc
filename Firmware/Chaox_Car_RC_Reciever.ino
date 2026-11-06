#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <avr/wdt.h>
//Pin definations
#define CE_PIN 9
#define CSN_PIN 10
#define ENA 5        // PWM Speed Pin
#define IN1 2        // Direction Pin
#define IN2 3        // Direction Pin
#define SERVO_PIN 8  // Servo Pin
//Servo stuff
#define SERVO_CENTER 90  // Standard center position(degree)
#define SERVO_MIN 0      // Left turn physical limit(degree)
#define SERVO_MAX 180    // Right turn physical limit(degree)

RF24 rc(CE_PIN, CSN_PIN);
Servo servo;

const uint64_t pipeAddress = 0xE8E8F0F0E1LL;  //Address for messages
struct Payload {
  uint32_t packetId;
  int16_t X_axis;  // Right Joystick- Left/Right - To Servo
  int16_t Y_axis;  // Left Joystick-  Forward/Backward - To Motor
};
Payload RcData;
unsigned long lastReceiveTime = 0;

void setup() {
  wdt_disable();
  Serial.begin(115200);
  wdt_enable(WDTO_4S);  //Start Watchdog woooooof!
  // Initialize nRF Module
  Serial.println("Setup Starting");
  Serial.println("nRF Setup Start");
  if (!rc.begin()) {
    Serial.println("nRF failed!");
    while (1)
      ;
  }
  rc.setPALevel(RF24_PA_LOW);  //Change as per range
  rc.setDataRate(RF24_1MBPS);
  rc.setChannel(76);
  rc.openReadingPipe(1, pipeAddress);
  rc.startListening();  // Receive only mode
  Serial.println("nRF Setup Complete");
  //Motor Setup
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  // Make sure car doesnt start moving
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  //Servo Setup
  servo.attach(SERVO_PIN);
  servo.write(SERVO_CENTER);  // Set wheels at centre on boot
  Serial.println("Setup Complete");
  Serial.println("Receiver Listening...");
}

void loop() {
  wdt_reset();           //feed the dog
  nRF_Get();             //This function checks and recieves messages
  Move(RcData.Y_axis);   //Moves the motors forward/backward
  Steer(RcData.X_axis);  //Steers the car
}

void nRF_Get() {
  if (rc.available()) {
    rc.read(&RcData, sizeof(Payload));
    lastReceiveTime = millis();  // Reset timer to show message came
    // Print message
    Serial.println("Message Recieved! Details: ");
    Serial.print("ID: ");
    Serial.println(RcData.packetId);
    Serial.print("X: ");
    Serial.println(RcData.X_axis);
    Serial.print("Y: ");
    Serial.println(RcData.Y_axis);
  }

  // Watchdog 2: Stop car if no message for 500ms
  if (millis() - lastReceiveTime > 500) {
    RcData.X_axis = 0;
    RcData.Y_axis = 0;
  }
}

void Move(int16_t thr) {
  if (thr > 0) {  //forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, thr);
  } else if (thr < 0) {  //backward
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, abs(thr));
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
}

void Steer(int16_t steer_input) {
  int servoAngle = map(steer_input, -255, 255, SERVO_MIN, SERVO_MAX);  //Calculate servo position based on limits
  servo.write(servoAngle);
}