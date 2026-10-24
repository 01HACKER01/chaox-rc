#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10

RF24 tr(CE_PIN, CSN_PIN);

// 5-Byte talk address (Reciever gets same...)
const uint64_t pipeAddress = 0xE8E8F0F0E1LL;

struct Payload {
  uint32_t packetId;
  int16_t X_axis;  //joystick X-axis
  int16_t Y_axis;  //joystick Y-axis
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
  Serial.println("Setup complete");
}
void loop() {
  trData.packetId++; //increment packet id
  Serial.print("Transmitting packet: ");
  Serial.print(trData.packetId);
  Serial.println("Containing: ");
  Serial.print("X-axis value: ");
  Serial.println(trData.X_axis);
  Serial.print("Y-axis value: ");
  Serial.println(trData.Y_axis);
  if (tr.write(&trData, sizeof(Payload))) {
    Serial.println("Transmitted succesfully");
  } else {
    Serial.println("Transmission failed, car maybe out of range...");
  }
  delay(50); //For stability
}