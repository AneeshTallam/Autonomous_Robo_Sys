#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10);  // CE, CSN

const byte address[6] = "1Node";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108);
  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);
  radio.stopListening();
}

void loop() {
  const char text[] = "Hello from A";
  Serial.println("Sending to B...");
  radio.write(&text, sizeof(text));

  delay(10); // Short delay before listening
  radio.startListening();

  unsigned long start_time = millis();
  while (!radio.available()) {
    if (millis() - start_time > 500) {
      Serial.println("No response from B");
      radio.stopListening();
      return;
    }
  }

  char reply[32] = "";
  radio.read(&reply, sizeof(reply));
  Serial.print("Received from B: ");
  Serial.println(reply);
  
  radio.stopListening();
  delay(1000);
}

