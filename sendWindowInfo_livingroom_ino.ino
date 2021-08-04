// SimpleRxAckPayload- the slave or the receiver
//LIVING ROOM
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "LowPower.h"

#define CE_PIN   9
#define CSN_PIN 10

//Bath Room Adress
uint64_t address = {0xD4C3B2E1A9LL};

const int wakeUpPin = 2;
int led = 7;
//int blinky = 10;

RF24 radio(CE_PIN, CSN_PIN);

int datacame = 0;

const byte distance = A0;
int distance_value = 0;

struct PayloadStruct {
  char message[7];          // only using 6 characters for TX & ACK payloads
  uint8_t window;
};
PayloadStruct payload;

//==============

void wakeUpNow() {
  datacame = 1; 
}

void setup() {
  
    pinMode(wakeUpPin, INPUT_PULLUP);
    pinMode(led, OUTPUT);
    pinMode(A0, INPUT);
    
    attachInterrupt(1, wakeUpNow, LOW);
    
    Serial.begin(9600);
    Serial.println("sending window data");
    if (!radio.begin()) {
      Serial.println(F("radio hardware is not responding!!"));
      while (1) {} // hold in infinite loop
    }
    //radio.setDataRate( RF24_250KBPS );
    radio.setPALevel(RF24_PA_LOW);
    radio.setChannel(83);
    radio.openReadingPipe(1, address);
    radio.enableDynamicPayloads();
    radio.enableAckPayload();
    
    radio.maskIRQ(1, 1, 0);
    
    radio.startListening();
    payload.window = 3;
    char msg[] = "living";
    memcpy(payload.message, msg, sizeof(msg));
    radio.writeAckPayload(1, &payload, sizeof(payload));
}

//==========

void loop() {  
  if ( datacame ) {
    detachInterrupt(0);
    datacame = 0;
    receiveData();
    attachInterrupt(1, wakeUpNow, LOW);
  } 
}

void receiveData() {
  if ( radio.available() ) {
        uint8_t bytes = radio.getDynamicPayloadSize();
        PayloadStruct received;
        radio.read(&received, sizeof(received));       // get incoming payload
        readWindow();
        Serial.print(received.message);
        Serial.println(sizeof(received.message));
        radio.writeAckPayload(1, &payload, sizeof(payload));        
   }
} // receiveData

//======================

void readWindow() {
  distance_value = analogRead(distance);
  Serial.println(distance_value);
  if(distance_value > 100)
    payload.window = 1; //closed
  else
    payload.window = 0; //open
} // readWindow

