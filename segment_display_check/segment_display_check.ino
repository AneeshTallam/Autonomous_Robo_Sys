#define PIN_SHIFT 12   // connected to SHCP (Reverse 11 and 12 if not working)
#define PIN_STORE 11  // connected to STCP
#define PIN_DATA  8  // connected to DS


void setup()
{
  Serial.begin(9600);
  pinMode(PIN_STORE, OUTPUT);
  pinMode(PIN_SHIFT, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);

  
}

int digit0[] = {1,1,1,1,1,1,0,0};
int digit1[] = {0,1,1,0,0,0,0,0};
int digit2[] = {1,1,0,1,1,0,1,0};
int digit3[] = {1,1,1,1,0,0,1,0};
int digit4[] = {0,1,1,0,0,1,1,0};
int digit5[] = {1,0,1,1,0,1,1,0};
int digit6[] = {1,0,1,1,1,1,1,0};
int digit7[] = {1,1,1,0,0,0,0,0};
int digit8[] = {1,1,1,1,1,1,1,0};
int digit9[] = {1,1,1,0,0,1,1,0};

int* ledpins[] = {
  digit0,
  digit1,
  digit2,
  digit3,
  digit4,
  digit5,
  digit6,
  digit7,
  digit8,
  digit9
};

void loop() {
  

    digitalWrite(PIN_STORE, LOW); // Start sending data
    // Send each bit
    for (int i = 0; i < 10; i++) {
      digitalWrite(PIN_STORE, LOW); // Start sending data
      for (int j = 7; j >=0; j--) {
        digitalWrite(PIN_SHIFT, LOW);       // Prepare for bit
        digitalWrite(PIN_DATA, ledpins[i][j]);  // Send current bit
        digitalWrite(PIN_SHIFT, HIGH);      // Clock in the bit
      }
      digitalWrite(PIN_STORE, HIGH); // Latch data to output
      delay(1000); // Wait so we can see the LEDs
    }
    


    

}