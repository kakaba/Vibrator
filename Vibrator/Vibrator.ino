// Vibrator Bluetooth

#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

int pwm = 0;
int pwmPin = 9;
int setPin = 4;
int connectPin = 5;
bool connectStatus = 0;

String inString = "";

// kamlanis filtris monacemebi
float varVolt = 0.95;  // sashualo gadaxra
float varProcess = 2; // cvlilebaze reagirebis sichqare
float Pc = 0.0;
float G = 0.0;
float P = 1.0;
float Xp = 0.0;
float Zp = 0.0;
float Xe = 0.0;
// kalmanis piltri

void setup() {
  // Open serial communications and wait for port to open:

  Serial.begin(38400);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.setTimeout(10);


  Serial.println("Vibrator is on!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(38400);
  mySerial.setTimeout(10);

  // set pwm sixshire divisorit
  setPwmFrequency( pwmPin , 64 );

  pinMode( setPin, OUTPUT );
  digitalWrite( setPin, LOW );

  pinMode( connectPin , INPUT );

}

void loop() {

  // Read serial input:
  while (mySerial.available() > 0) {
    char inInt = mySerial.read();
    int inChar = (int) inInt;
    Serial.println(inChar);
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }

    // if you get a newline, print the string, then the string's value:
    if (inChar == '#') {
      Serial.print("Value:");
      int value = inString.toInt();
      Serial.println(value);
      Serial.print("String: ");
      Serial.println(inString);

      motorPower( value );

      // clear the string for new input:
      inString = "";
    }
  }


  connectStatus = filter(digitalRead(connectPin));
//  Serial.println(connectStatus);

  if ( connectStatus == 0 ) {
    analogWrite( pwmPin , 0 );
    Serial.println("connection lost shutting down");
  }

}



void motorPower(int power) {
  if ( power > 0 && power <= 255 && connectStatus == 1 ) {
    float value = (float) power ;
    value = value * 0.725;
    int x = ((int) value ) + 71;
    Serial.println( x );
    analogWrite( pwmPin , x );
  }

  else if ( power == 0 ) {
    analogWrite( pwmPin , 0 );
  }

  else {
    analogWrite( pwmPin , 0 );
    Serial.println("Incorrect Value");
  }
}


float filter(float val) {  //функция фильтрации
  Pc = P + varProcess;
  G = Pc / (Pc + varVolt);
  P = (1 - G) * Pc;
  Xp = Xe;
  Zp = Xp;
  Xe = G * (val - Zp) + Xp; // "фильтрованное" значение
  return (Xe);
}


void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if (pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if (pin == 3 || pin == 11) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
