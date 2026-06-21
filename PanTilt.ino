#include <Servo.h>
#include <math.h>

// Create servo objects
Servo panServo;
Servo tiltServo;

#define PI 3.14159265358979323846

// Pin assignments
const int panPin = 9;
const int tiltPin = 10;

// restraints for angle changes
const int panMin = 0;
const int panMax = 180;
const int tiltMin = 30;
const int tiltMax = 150;

// Starting positions
double x = 0;
double y = 0;

// Starting angles 
double panAngle = 90;
double tiltAngle = 90;

void move(double xCord, double yCord){
  
  double panAngleChange;
  double tiltAngleChange;

  //First filters all cases where division by 0 may happen. Then calculates the amount of pan (bottom motor) needed to align itself with the point given.

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

  //Calculates how much tilt (top servo) is needed to bring itself to its coordinate.

  tiltAngleChange = sqrt(xCord*xCord + yCord*yCord)/10*90;
  if (yCord < 0){
    tiltAngleChange = -tiltAngleChange;
  } 

  tiltAngle = 90 + tiltAngleChange;

  
  panServo.write(panAngle);
  tiltServo.write(tiltAngle);
}

void line(double xCordStart, double yCordStart, double xCordEnd, double yCordEnd, double time){

  // distance between two points 
  double distance = sqrt(pow((xCordEnd-xCordStart),2)+pow((yCordEnd-yCordStart),2));

  //suppose each action has a delay of 100ms, that is the amount of intervals, rounded down. 
  int intervals;

  if (time>=0.1){
    intervals = time/0.1;
  } else {
    return;
  }

  double changeX = (xCordEnd-xCordStart)/intervals;
  double changeY = (yCordEnd-yCordStart)/intervals;


  for (int i=1; i<=intervals; i++){
    move(xCordStart+changeX*i, yCordStart+changeY*i);

    //BUG: if y goes from pos -> neg or neg -> pos, a bigger delay is needed, this is because the motor needs a time to completely turn around.

    //if ()

    delay(100);
  }

  // accounting for rounding errors.
  move(xCordEnd,yCordEnd);
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
      }  else if ((input == 'o' || input == 'O')){
        x=0;
        y=0;
      } else if ((input == '0' || input == '0')){
        Serial.print("Drawing a square!");
        move(5,5);
        line(5,5,5,-5,3);
        line(5,-5,-5,-5,3);
        line(-5,-5,-5,5,3);
        line(-5,5,5,5,3);
      }
    } else {
      Serial.print("Out of bounds!");
    }

    //debug text 
    Serial.print("(");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(")\n");

    move(x,y);
    
    // Serial.print(atan(x/y));
    /*
    Serial.print("Pan: ");
    Serial.print(panAngle);
    Serial.print("\n");
    Serial.print("Tilt: ");
    Serial.print(tiltAngle);
    Serial.print("\n");
    */
  }
}