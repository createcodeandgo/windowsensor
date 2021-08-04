import time
import sys
import struct
from RF24 import RF24, RF24_PA_LOW
from Adafruit_IO import Client
import info

radio = RF24(22, 0)

addresses = {"living": b"\xA9\xE1\xB2\xC3\xD4",
             "bath": b"\xB5\xE1\xB2\xC3\xD4"}

status = {"living": 3, "bath": 3}

def send_data():
    for item in list(addresses):
      pings = 0
      while (pings < 2):
        radio.openWritingPipe(addresses[item])
        radio.stopListening()
        payload = b"wakeup\x00"
        #print(payload)
        result = radio.write(payload)
        if result:
            has_payload, pipe_number = radio.available_pipe()
            if has_payload:
                length = radio.getDynamicPayloadSize()
                response = radio.read(length)
                #print(response)
                msg = ""
                info = [3,3]
                n = 0
                for c in list(response)[:6]:
                    msg += chr(c)
                for i in list(response)[6:8]:
                    info[n] = int(i)
                    n += 1

                print( msg, " ", info[1])
                if info[1] == 0:
                    text = "open"
                    status[item] = info[1]
                elif info[1] == 1:
                    text = "closed"
                    status[item] = info[1]
                elif info[1] == 3:
                    text = "wokeup"
                    status[item] = 3
                else:
                    print("BEEP BEEP BEEP")
                time.sleep(1)
                if pings == 1:
                    print(item," is ",text)
            else:
                print("empty ACK")
        else:
            print("transmission failed")
        time.sleep(1)
        pings += 1
    aio.send('window.bathroom', status["bath"])
    aio.send('window.living-room', status["living"])

if __name__ == "__main__":
    aio = Client(info.ADANAME, info.ADAKEY)

    if not radio.begin():
        raise RuntimeError("radio Hardware not responding")

    address = b"\xA9\xE1\xB2\xC3\xD4"

    radio.setChannel(83)

    radio.setPALevel(RF24_PA_LOW)
    radio.enableDynamicPayloads()
    radio.enableAckPayload()

    #radio.openWritingPipe(addresses[0])
    
    try:
        send_data()
        radio.powerDown()
    except KeyboardInterrupt:
        print("ending transmission")
        radio.powerDown()
        sys.exit()

