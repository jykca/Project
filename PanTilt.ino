#include <Servo.h>


// Create servo objects
Servo panServo;
Servo tiltServo;

#define PI 3.14159265358979323846

// Pin assignments
const int panPin = 9;
const int tiltPin = 10;

const int panMin = 0;
const int panMax = 180;
const int tiltMin = 30;
const int tiltMax = 150;


// Starting positions
double x = 0;
double y = 0;

double panAngle = 90;
double tiltAngle = 90;

void move(double xCord, double yCord){
  
  double panAngleChange;
  double tiltAngleChange;

  if (yCord == 0 && xCord > 0){
    panAngleChange = 90;
  } else if (yCord == 0 && xCord < 0){
    panAngleChange = -90;
  } else if (yCord == 0){
    panAngleChange = 0;
  } else{
    panAngleChange = atan(xCord/yCord)*180/PI;
  }

  panAngle = 90 - panAngleChange;

  tiltAngleChange = sqrt(xCord*xCord + yCord*yCord)/10*90;
  if (yCord < 0){
    tiltAngleChange = -tiltAngleChange;
  } 

  tiltAngle = 90 + tiltAngleChange;

  
  panServo.write(panAngle);
  tiltServo.write(tiltAngle);
}

void setup() {
  panServo.attach(panPin);
  tiltServo.attach(tiltPin);

  // Move to center position
  panServo.write(panAngle);
  tiltServo.write(tiltAngle);

  Serial.begin(9600); 

  delay(1000);
}

void loop() {


  if (Serial.available() > 0) {
    char input = Serial.read();
    
    if (sqrt(x*x + y*y)<=10){
      if ((input == 'w' || input == 'W') && y  < 10){
        y = y+1;
      } else if ((input == 'a' || input == 'A') && x > -10){
        x = x-1;
      }  else if ((input == 's' || input == 'S') && y > -10){
        y = y-1;
      }  else if ((input == 'd' || input == 'D') && x < 10){
        x = x+1;
      }  else if ((input == 'q' || input == 'Q')){
    
      } 
    } else {
      Serial.print("Out of bounds!");
    }

    Serial.print("(");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(")\n");
    move(x,y);
    Serial.print(atan(x/y));
    Serial.print("Pan: ");
    Serial.print(panAngle);
    Serial.print("\n");
    Serial.print("Tilt: ");
    Serial.print(tiltAngle);
    Serial.print("\n");
  }
}