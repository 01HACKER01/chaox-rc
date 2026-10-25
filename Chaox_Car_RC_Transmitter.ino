#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <avr/wdt.h> //Watchdog

#define CE_PIN 9
#define CSN_PIN 10
#define LEFT_JOY_Y A0   // Left Joystick VRy (Forward/Backward)
#define RIGHT_JOY_X A3  // Right Joystick VRx (Left/Right)
#define DEADZONE 15     //Deadzone where the joystick is considered at center

RF24 tr(CE_PIN, CSN_PIN);

// 5-Byte talk address (Reciever gets same...)
const uint64_t pipeAddress = 0xE8E8F0F0E1LL;

struct Payload {
  uint32_t packetId;
  int16_t X_axis;  //joystick X-axis (Left/Right)
  int16_t Y_axis;  //joystick Y-axis (Forward/Backward)
};
Payload trData;

void setup() {
  Serial.begin(115200);
  Serial.println("Initialize nRF");
  if (!tr.begin()) {
    Serial.println("nRF connection failed!!!");
    while (1)
      ;
  }
  tr.setPALevel(RF24_PA_LOW);       //Use Low for testing and High/Max for use...
  tr.setDataRate(RF24_1MBPS);       //Balanced i guess
  tr.setChannel(76);                //idk why i chose this...
  tr.openWritingPipe(pipeAddress);  //connect to reciever
  tr.stopListening();               //make module transmit only
  wdt_enable(WDTO_4S); //Start Watchdog woooooof!
  Serial.println("Setup complete");
}
void loop() {
  trData.packetId++;  //increment packet id
  wdt_reset(); //Feed watchdog to keep it full and happy Yippee!
  //Get joystick values
  int Raw_FB = analogRead(LEFT_JOY_Y);   //Forward/Back Value
  int Raw_LR = analogRead(RIGHT_JOY_X);  //Left/Right Value
  //Convert them into a better format
  int FB = map(Raw_FB, 0, 1023, -255, 255);
  int LR = map(Raw_LR, 0, 1023, -255, 255);
  //Get better centre detection
  if (FB >= -DEADZONE && FB <= DEADZONE) {
    trData.Y_axis = 0;
  } else {
    trData.Y_axis = FB;
  }
  if (LR >= -DEADZONE && LR <= DEADZONE) {
    trData.X_axis = 0;
  } else {
    trData.X_axis = LR;
  }
  //Print stuff
  Serial.print("Transmitting packet: ");
  Serial.print(trData.packetId);
  Serial.println("Containing: ");
  Serial.print("X-axis value: ");
  Serial.println(trData.X_axis);
  Serial.print("Y-axis value: ");
  Serial.println(trData.Y_axis);
  //Send the stuff
  if (tr.write(&trData, sizeof(Payload))) {
    Serial.println("Transmitted succesfully");
  } else {
    Serial.println("Transmission failed, car maybe out of range...");
  }
  delay(50);  //For stability i guess
}