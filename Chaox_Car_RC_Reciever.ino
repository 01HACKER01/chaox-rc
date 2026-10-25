#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10

RF24 rc(CE_PIN, CSN_PIN);

const uint64_t pipeAddress = 0xE8E8F0F0E1LL;  //Address for messages
struct Payload {
  uint32_t packetId;
  int16_t X_axis;  // Right Joystick- Left/Right - To Servo
  int16_t Y_axis;  // Left Joystick-  Forward/Backward - To Motor
};
Payload RcData;
unsigned long lastReceiveTime = 0;

void setup() {
  Serial.begin(115200);
  // Initialize nRF Module
  Serial.println("Setup Starting");
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
  Serial.println("Setup Complete");
  Serial.println("Receiver Listening...");
}

void loop() {
  nRF_Get();
}

void nRF_Get() {
  if (rc.available()) {
    rc.read(&rcData, sizeof(Payload));
    lastReceiveTime = millis();  // Reset timer to show message came

    // Print message 
    Serial.println("Message Recieved! Details: ");
    Serial.print("ID: ");
    Serial.println(rcData.packetId);
    Serial.print("X: ");
    Serial.println(rcData.X_axis);
    Serial.print("Y: ");
    Serial.println(RcData.Y_axis);
  }

  // Watchdog 2: Stop car if no message for 500ms
  if (millis() - lastReceiveTime > 500) {
    rcData.X_axis = 0;
    rcData.Y_axis = 0;
  }
}