#include <IRremote.hpp>

#define IR_RECEIVE_PIN 3  // Change to your IR receiver pin

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver ready. Press any button on the remote...");
}

void loop() {
  if (IrReceiver.decode()) {
    uint32_t command = IrReceiver.decodedIRData.command;
    Serial.print("Button Pressed - Hex Code: 0x");
    Serial.println(command, HEX);
    
    IrReceiver.resume();  // Ready to receive the next code
  }
}
