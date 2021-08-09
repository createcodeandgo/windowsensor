/* 
    SimpleRxAckPayload- the slave or the receiver
    LIVING ROOM
*/

#include <SPI.h>
//#include <nRF24L01.h>
#include <RF24.h>
//#include "LowPower.h"

/*
    nRF24
*/
#define CE_PIN   9
#define CSN_PIN 10
uint64_t address = {0xD4C3B2E1A9LL};
const int wakeUpPin = 2; // IRQ pin
RF24 radio(CE_PIN, CSN_PIN);
int datacame = 0;
struct PayloadStruct {
    char message[7];
    uint8_t window;
};
PayloadStruct payload;

/*
    distance sensor
*/
const byte distance = A0;
int distance_value = 0;

void setup() {
    pinMode(wakeUpPin, INPUT); // Interrupt
    pinMode(A0, INPUT); // distance sensor
    
    Serial.begin(9600);
    Serial.println("sending window data");
    if (!radio.begin()) {
        Serial.println(F("radio hardware is not responding!!"));
        while (1) {} // hold in infinite loop
    }

    // nRF24 setup
    radio.setPALevel(RF24_PA_LOW);
    radio.setChannel(83); // same as for the Pi
    radio.openReadingPipe(1, address);
    radio.enableDynamicPayloads();
    radio.enableAckPayload();
    radio.startListening();
    
    // Interrupt setup
    attachInterrupt(1, wakeUpNow, LOW);
    radio.maskIRQ(1, 1, 0);

    // put first payload into Ack packet
    payload.window = 3;
    char msg[] = "living";
    memcpy(payload.message, msg, sizeof(msg));
    radio.writeAckPayload(1, &payload, sizeof(payload));
} // setup

void loop() {  
    // check if ISR ran
    if ( datacame ) {
        detachInterrupt(0);
        datacame = 0;
        receiveData();
        attachInterrupt(1, wakeUpNow, LOW);
    }
} // loop

// ISR
void wakeUpNow() {
    datacame = 1; 
} // wakeUpNow

void receiveData() {
    /*
        read the message that was received
        ACK packet automatically gets sent
        ACK packet for next message is set up
    */
    PayloadStruct received;
    if ( radio.available() ) {
        uint8_t bytes = radio.getDynamicPayloadSize();
        radio.read(&received, sizeof(received)); // get incoming payload
        readWindow();
        Serial.print(received.message);
        Serial.println(sizeof(received.message));
        radio.writeAckPayload(1, &payload, sizeof(payload));        
    }
} // receiveData

void readWindow() {
    /* 
        get value from CNY70
        set payload message for next ACK
    */
    distance_value = analogRead(distance);
    Serial.println(distance_value);
    if(distance_value > 500)
        payload.window = 1; //closed
    else
        payload.window = 0; //open
} // readWindow
