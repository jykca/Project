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

    double newX = xCordStart+changeX*i;
    double newY = yCordStart+changeY*i;


    move(newX, newY);

    //BUG: if y goes from pos -> neg or neg -> pos, a bigger delay is needed, this is because the motor needs a time to completely turn around.

    double oldY = yCordStart+changeY*(i-1);
    if ((oldY<=0 && newY>0) || (oldY>=0 && newY<0)){
      delay(2000);
    }

    delay(100);
  }

  // accounting for rounding errors.
  move(xCordEnd,yCordEnd);
}

void arc(double xStart, double yStart, double xMid, double yMid, double xEnd, double yEnd){

  //arc check, checks if the triangle formed by these three points yields a very small area.

  double triangleArea = 0.5 * ((xStart * (yMid - yEnd)) + (xMid * (yEnd - yStart)) + (xEnd * (yStart - yMid)));

  if (abs(triangleArea) < 0.000001){
    return; // points are bascially collinear
  }

  //finding centre point of arc. 
  double centerX = ((xStart*xStart + yStart*yStart)*(yMid-yEnd) + (xMid*xMid + yMid*yMid)*(yEnd-yStart) + (xEnd*xEnd + yEnd*yEnd)*(yStart-yMid))/(triangleArea*2);

  double centerY = ((xStart*xStart + yStart*yStart)*(xEnd-xMid) + (xMid*xMid + yMid*yMid)*(xStart-xEnd) + (xEnd*xEnd + yEnd*yEnd)*(xMid-xStart))/(triangleArea*2);

  double radius =
        sqrt(pow(xStart - centerX, 2) +
             pow(yStart - centerY, 2));

    // angles ONLY used to generate points (NOT for move)
    double a1 = atan2(yStart - centerY, xStart - centerX);
    double a2 = atan2(yMid   - centerY, xMid   - centerX);
    double a3 = atan2(yEnd   - centerY, xEnd   - centerX);

    // normalize to 0..2PI
    auto norm = [](double a){
        while (a < 0) a += 2*PI;
        while (a >= 2*PI) a -= 2*PI;
        return a;
    };

    a1 = norm(a1);
    a2 = norm(a2);
    a3 = norm(a3);

    // decide direction through cross products. Basically checks whether the vector from start to middle is to the left or right of the vector from start to end.
    double ax = xStart - centerX;
    double ay = yStart - centerY;

    double bx = xMid - centerX;
    double by = yMid - centerY;

    double cx = xEnd - centerX;
    double cy = yEnd - centerY;

    double cross =
    (cx - ax)*(by - ay) - 
    (bx - ax)*(cy - ay);

    bool ccw = (cross < 0);

    double total;

    if (ccw){
        total = a3 - a1;
        if (total < 0) total += 2*PI;
    } else {
        total = a1 - a3;
        if (total < 0) total += 2*PI;
    }

    int steps = 50;

    for (int i = 0; i <= steps; i++){

        double t = (double)i / steps;

        double angle = ccw
            ? a1 + total * t
            : a1 - total * t;

        double x = centerX + radius * cos(angle);
        double y = centerY + radius * sin(angle);

        move(x, y); 
        delay(200);
    }

    move(xEnd, yEnd);
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
      }  else if ((input == 'k' || input == 'K')){
        
        arc(-5, 1, 0, 5, 5, 1);
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