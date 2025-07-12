#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10);  // CE, CSN

const byte address[6] = "1Node";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108);
  radio.openReadingPipe(1, address);
  radio.openWritingPipe(address);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char incoming[32] = "";
    radio.read(&incoming, sizeof(incoming));
    Serial.print("Received from A: ");
    Serial.println(incoming);

    radio.stopListening();
    const char reply[] = "Reply from B";
    radio.write(&reply, sizeof(reply));
    Serial.println("Sent reply to A");

    radio.startListening();
  }
}
