#include "BluetoothSerial.h"

BluetoothSerial BT;

#define PWMA 13
#define AIN2 15
#define AIN1 32
#define PWMB 14
#define BIN2 27
#define BIN1 26

char driverDriveA = 'F';
char targetDriveA = 'F';
char driverDriveB = 'F';
char targetDriveB = 'F';

char targerSteer = 'R';

int throttle = 0;
int steerPercent = 0;
int APWM = 0;
int BPWM = 0;

bool breakActive = false;
bool instantMode = true;

long lastRampMillis = 0;
const long rampIntervalMs = 10;
long lastPrintMillis = 0;
bool printSerial = true;


//====================================================================================================================================================


void setup() {

  Serial.begin( 115200 );
  BT.begin( "BTCar_GenA_LA" );

  pinMode( PWMA , OUTPUT );
  pinMode( AIN2 , OUTPUT );
  pinMode( AIN1 , OUTPUT );
  pinMode( PWMB , OUTPUT );
  pinMode( BIN2 , OUTPUT );
  pinMode( BIN1 , OUTPUT );

  Serial.println( " <---- BTCar_GenA_LA ----> " );

}


//====================================================================================================================================================


void loop() {

//============================== B L U E T O O T H   D A T A   B L O C K =============== >>
  if(BT.available()) {
    char data = BT.read();

    // ========================== D A T A   ! B L A N K ================================
    if( data != '\n' && data != '\r' && data != ' ' && data != 0 ) {

      // ======================== L O G I C   C H E C K ================================ >>
      if( data == 'W' ) breakActive = true;
      else if( data == 'w' ) breakActive = false;
      
      if( data == 'Z' ) instantMode = !instantMode;
      // ======================== L O G I C   C H E C K ================================ <<

      // ======================== G E T   M O V E M E N T ============================== >>
      if( data == 'F' || data == 'B' ) {

        targetDriveA = data; 
        targetDriveB = data;

        String placeHolder = "";

        for( int i = 0 ; i < 2 ; i++ ) {
          char x = BT.read();
          placeHolder = placeHolder + x;
        }

        throttle = placeHolder.toInt();

        placeHolder = "";

        data = BT.read();

        targerSteer = data;

        for( int i = 0 ; i < 2 ; i++ ) {
          char x = BT.read();
          placeHolder = placeHolder + x;
        }

        steerPercent = placeHolder.toInt();

      }
      // ======================== G E T   M O V E M E N T ============================== <<
      
    }

  }
  //============================== B L U E T O O T H   D A T A   B L O C K ============== <<
  
  int targetPWMA = map( throttle , 0 , 99 , 0 , 255 );
  int targetPWMB = map( throttle , 0 , 99 , 0 , 255 );
  int targetSteerPercent = map( steerPercent , 0 , 60 , 0 , 255 );

  long current = millis();

  if( targerSteer == 'R' ) {
    targetPWMA = targetPWMA - targetSteerPercent;
  } else if ( targerSteer == 'L' ) {
    targetPWMB = targetPWMB - targetSteerPercent;
  }

  

  //============================== S P E E D   C O N T R O L   B L O C K ================ >>
  if( current - lastRampMillis > rampIntervalMs ) {
    transistion( APWM, targetPWMA, driverDriveA, targetDriveA );
    transistion( BPWM, targetPWMB, driverDriveB, targetDriveB );
    lastRampMillis = current;
  }
  //============================== S P E E D   C O N T R O L   B L O C K ================ <<

  if( APWM == 0 ) breakActive ? A( 0b11 ) : A( 0b00 );
  else (driverDriveA == 'F' ) ? A( 0b10 ) : A( 0b01 );

  if( BPWM == 0 ) breakActive ? B( 0b11 ) : B( 0b00 );
  else (driverDriveB == 'F' ) ? B( 0b10 ) : B( 0b01 );
  
  analogWrite( PWMA , APWM );
  analogWrite( PWMB , BPWM );

  
  if ( current - lastPrintMillis > 100 ) {
    lastPrintMillis = current;
    printSerial = true;
  }

  if( printSerial ) {
    // Format : instantMode , breakActive , driverDriveA/B, A/B PWM
    Serial.print( instantMode + String( " " ) );
    Serial.print( breakActive + String( " " ) );

    Serial.print( driverDriveA );
    Serial.print( driverDriveB + String( " " ) );
    Serial.print( ( ( APWM < 10 ) ? ( "00" + String( APWM ) ) : ( APWM < 100 )  ? ( "0" + String( APWM ) ) : String( APWM ) ) + " " );
    Serial.print( ( ( BPWM < 10 ) ? ( "00" + String( BPWM ) ) : ( BPWM < 100 )  ? ( "0" + String( BPWM ) ) : String( BPWM ) ) + " " );
    Serial.print( targerSteer + String( " " ) );
    Serial.print( ( ( targetSteerPercent < 10 ) ? ( "00" + String( targetSteerPercent ) ) : ( targetSteerPercent < 100 )  ? ( "0" + String( targetSteerPercent ) ) : String( targetSteerPercent ) ) + " " );
    Serial.println(); 
    printSerial = false;
  }

}


//====================================================================================================================================================


void transistion( int &PWM, int targetPWM, char &drive, char targetDrive ) {
  if ( instantMode ) {
    drive = targetDrive;
    PWM = targetPWM;
  } else {
    if ( drive == targetDrive ) {
      if ( PWM > targetPWM ) PWM--;
      else if ( PWM < targetPWM ) PWM++;
    } else {
      if ( PWM > 0 ) PWM--;
      else drive = targetDrive;
    }
  }
}

void A( int bit ) {
  switch ( bit ) {
    case 0b10:
      digitalWrite( AIN1, HIGH );
      digitalWrite( AIN2, LOW );
      break;
    case 0b01:
      digitalWrite( AIN1, LOW );
      digitalWrite( AIN2, HIGH );
      break;
    case 0b11:
      digitalWrite( AIN1, HIGH );
      digitalWrite( AIN2, HIGH );
      break;
    case 0b00:
      digitalWrite( AIN1, LOW );
      digitalWrite( AIN2, LOW );
      break;
  }
}

void B( int bit ) {
  switch ( bit ) {
    case 0b10:
      digitalWrite( BIN1, HIGH );
      digitalWrite( BIN2, LOW );
      break;
    case 0b01:
      digitalWrite( BIN1, LOW );
      digitalWrite( BIN2, HIGH );
      break;
    case 0b11:
      digitalWrite( BIN1, HIGH );
      digitalWrite( BIN2, HIGH );
      break;
    case 0b00:
      digitalWrite( BIN1, LOW );
      digitalWrite( BIN2, LOW );
      break;
  }
}