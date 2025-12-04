// Bluetooth Car L298N Control - Low Level
// BTCar_LS_L_v1.0.0ino

#include "BluetoothSerial.h"

#define Bluetooth "BTCar.LS.L.v1.0.0" // Bluetooth Name !

// --- Pin Configuration (Change as needed) ---
// These GPIO pins connect to the L298N motor driver.
// n1 / n2  → Left motor direction control
// n3 / n4  → Right motor direction control
// ENA / ENB → Motor speed control (PWM)
#define n1 33
#define n2 25
#define n3 26
#define n4 27
#define ENA 32
#define ENB 14

BluetoothSerial BT;

void setup() {
  Serial.begin( 115200 );
  BT.begin( Bluetooth );

  pinMode( n1, OUTPUT );
  pinMode( n2, OUTPUT );
  pinMode( n3, OUTPUT );
  pinMode( n4, OUTPUT );
  pinMode( ENA, OUTPUT );
  pinMode( ENB, OUTPUT );
}

char command = 'S';

void loop() {

  // Read incoming Bluetooth data
  if ( BT.available() ) {
    char data = BT.read();
        if( data != '\n' && data != '\r' && data != ' ' && data != 0 ) { // check if blank
        command = data;   // <-- store and REMEMBER last command
    }
  }

  switch (command) {

    case 'F': // Forward
      n1_on(); n2_off(); n3_on(); n4_off();
      break;

    case 'B': // Back
      n1_off(); n2_on(); n3_off(); n4_on();
      break;

    case 'R': // Left turn L
      n1_off(); n2_on(); n3_on(); n4_off();
      break;

    case 'L': // Right turn R
      n1_on(); n2_off(); n3_off(); n4_on();
      break;

    case 'H': // Forward-Left G
      n1_off(); n2_off(); n3_on(); n4_off();
      break;

    case 'G': // Forward-Right H
      n1_on(); n2_off(); n3_off(); n4_off();
      break;

    case 'J': // Back-Right I
      n1_off(); n2_off(); n3_off(); n4_on();
      break;

    case 'I': // Back-Left J
      n1_off(); n2_on(); n3_off(); n4_off();
      break;

    default: // Stop
      n1_off(); n2_off(); n3_off(); n4_off();
      break;
  }

    analogWrite( ENA , 255 ); // Set motor speed (0-255)
    analogWrite( ENB , 255 ); // Set motor speed (0-255)
}

void n1_on(){ digitalWrite( n1 , HIGH ); }
void n1_off(){ digitalWrite( n1 , LOW ); }

void n2_on(){ digitalWrite( n2 , HIGH ); }
void n2_off(){ digitalWrite( n2 , LOW ); }

void n3_on(){ digitalWrite( n3 , HIGH ); }
void n3_off(){ digitalWrite( n3 , LOW ); }

void n4_on(){ digitalWrite( n4 , HIGH ); }
void n4_off(){ digitalWrite( n4 , LOW ); }
