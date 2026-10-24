#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_Pin  9
#define CSN_Pin 10

RF24 tr(CE_PIN, CSN_PIN);

// 5-Byte talk address (Reciever gets same...)
const uint64_t pipeAddress = 0xE8E8F0F0E1LL;
