//Lab 1 CSCI 3302 Robotics
//Julio Lemos
//Nhan Nguyen
//Eliaz Ortiz
//David Stone

#include <Sparki.h> // include the sparki library
#define START 0
#define SEARCH 1
#define GRIP 2
#define CHECKMAG 3
#define FINDLINE 4
#define MOVELINE 5
#define DROPBIN 6
#define GOHOME 7
#define NO_ACCEL
int state;
int duration;
int cm;
int objectDistance = 8;
int threshold = 500;
int lineLeft;
int lineCenter;
int lineRight;
int dropDec = 20;
int counter = 0;
bool magBin = false;
bool item = false;

//odometry code
float maxspeed = 0.0285;  // [m/s] speed of the robot that you measured
float alength = 0.0851;   // [m] axle length
float phildotr = 0, phirdotr = 0; // wheel speeds that you sent to the motors
float Xi = 0.05, Yi = -0.05, Thetai = 0; // where the robot is in the world
float Xrdot, Thetardot;    // how fast the robot moves in its coordinate system

float Xg = 0.05;   // Where the robot should go
float Yg = -0.05;
float Thetag = 70;


float alpha, rho, eta; // error between positions in terms of angle to the goal, distance to the goal, and final angle
float a = 0.1, b = 1, c = 0.1; // controller gains


void setup()
{
  sparki.servo(SERVO_CENTER); // Center the Servo
  Serial.begin (9600);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  state = START;
  sparki.clearLCD();
}

void loop()
{
  long int time_start = millis();
  switch (state)
  {
    // case tested - working
    case START:
      /* printing state to help debug */
      sparki.println("start");
      sparki.updateLCD();
      delay(100);
      sparki.gripperOpen();
      delay(2000);
      sparki.gripperStop();
      delay(1500);
      state = SEARCH;
      counter = 0;
      break;

    // case tested - working
    case SEARCH:
      /* printing state to help debug */
      sparki.moveLeft(90);
      Thetai = Thetai - 90;
      sparki.println("search");
      sparki.updateLCD();
      sparki.RGB(RGB_GREEN); // turn the light green
      //sparki.moveForward(); // move Sparki forward
      while (counter < 4) // four checks if there are objects
      {
        // calculate error
        rho   = sqrt((Xi - Xg) * (Xi - Xg) + (Yi - Yg) * (Yi - Yg));
        //alpha = Thetai-atan2(Yi-Yg,Xi-Xg)-PI/2.0;
        alpha = atan2(Yg - Yi, Xg - Xi) - Thetai;
        eta   = Thetai - Thetag;

        // CALCULATE SPEED IN ROBOT COORDINATE SYSTEM
        Xrdot = a * rho;
        //Xrdot=0;
        Thetardot = b * alpha + c * eta;

        // CALCULATE WHEEL SPEED
        phildotr = (2 * Xrdot - Thetardot * alength) / (2.0);
        phirdotr = (2 * Xrdot + Thetardot * alength) / (2.0);

        // set wheel speed
        if (phildotr > maxspeed) {
          phildotr = maxspeed;
        }
        else if (phildotr < -maxspeed) {
          phildotr = -maxspeed;
        }
        if (phirdotr > maxspeed) {
          phirdotr = maxspeed;
        } else if (phirdotr < -maxspeed) {
          phirdotr = -maxspeed;
        }

        float leftspeed  = abs(phildotr);
        float rightspeed = abs(phirdotr);

        leftspeed = (leftspeed / maxspeed) * 100; //100
        rightspeed = (rightspeed / maxspeed) * 100; //100

        if (Thetai < Thetag) // if farther away than 1cm
        {
          if (phildotr > 0)
          {
            sparki.motorRotate(MOTOR_LEFT, DIR_CCW, leftspeed);
          }
          else
          {
            sparki.motorRotate(MOTOR_LEFT, DIR_CW, leftspeed);
          }
          if (phirdotr > 0)
          {
            sparki.motorRotate(MOTOR_RIGHT, DIR_CW, rightspeed);
          }
          else
          {
            sparki.motorRotate(MOTOR_RIGHT, DIR_CCW, rightspeed);
          }
        }
        //  sparki.moveLeft(20);
        //  delay(100);
        cm = sparki.ping();
        delay(100);
        sparki.updateLCD();

        if (cm > 0) { // ping is -1 if sparki doesn't see anything
          if (cm < 60) // if the distance measured is less than 15 centimeters
          {
            sparki.RGB(RGB_RED); // turn the light red
            delay(100);
            sparki.RGB(RGB_GREEN);
            sparki.println(cm);
            sparki.updateLCD();
            sparki.beep(); // beep!
            sparki.moveForward(cm);
            delay(100);
            cm = sparki.ping();
            delay(3000);
            if (cm <= objectDistance)
            {
              state = GRIP;
              break;
            }
          }
        }
        sparki.clearLCD(); // wipe the screen
        sparki.print("distance ");
        sparki.println(cm);
        sparki.print(Xi);
        sparki.print("/");
        sparki.print(Yi);
        sparki.print("/");
        sparki.print(Thetai);
        sparki.println();
        sparki.print(alpha / PI * 180);
        sparki.println();

        sparki.updateLCD(); // display all of the information written to the screen

        // perform odometry
        Xrdot = phildotr / 2.0 + phirdotr / 2.0;
        Thetardot = phirdotr / alength - phildotr / alength;

        Xi = Xi + cos(Thetai) * Xrdot * 0.1;
        Yi = Yi + sin(Thetai) * Xrdot * 0.1;
        Thetai = Thetai + Thetardot * 0.1;
        counter++;
        while (millis() < time_start + 100); // wait until 100ms have elapsed

      }
    // case not tested
    case GRIP:
      /* printing state to help debug */
      sparki.println("grip");
      sparki.updateLCD();
      sparki.gripperClose();
      delay(3000);
      sparki.gripperStop();
      delay(100);
      state = CHECKMAG;
      item = true;
      break;

    case CHECKMAG:
      while (true) {
        delay(100);
        float y  = sparki.magY();   // measure the accelerometer y-axis
        float x = sparki.magX();
        float z = sparki.magZ();
        y = abs(y);
        x = abs(x);
        z = abs(z);
        delay(100);
        float totalmag = x + y + z;
        sparki.println(y);
        sparki.print("x");
        sparki.println(x);
        sparki.print("z");
        sparki.println(z);
        sparki.updateLCD();
        sparki.clearLCD();
        // write the measurements to the screen
        if (totalmag > 1500) {
          magBin = true;
          sparki.println("ITS MAG");
          state = FINDLINE;
          break;
        }
        else {
          magBin = false;
          state = FINDLINE;
          break;
        }
      }


    case FINDLINE:
      sparki.println("findline");
      sparki.updateLCD();
      sparki.moveRight(180);
      delay(5000);
      while (true) {
        lineCenter = sparki.lineCenter(); // measure the center IR sensor
        lineLeft = sparki.lineLeft();
        lineRight = sparki.lineRight();
        sparki.print("Left: ");
        sparki.println(lineLeft);
        sparki.print("Center: ");
        sparki.println(lineCenter);
        sparki.print("Right: ");
        sparki.println(lineRight);
        sparki.updateLCD();
        sparki.clearLCD();
        if ((lineCenter > threshold) && (lineLeft > threshold) && (lineRight > threshold)) // if nothing
        {
          sparki.moveForward();
        }
        if ((lineRight < threshold) && (lineCenter > threshold) && (lineLeft > threshold)) { //if only right sensor
          sparki.moveForward();
        }
        if ((lineRight < threshold) && (lineCenter < threshold) && (lineLeft > threshold)) { //if right and center
          sparki.moveForward();
        }
        //        if((lineRight > threshold) && (lineCenter < threshold) && (lineLeft < threshold)){
        //          sparki.println("OH NO");
        //        }
        if ((lineRight < threshold) && (lineCenter < threshold) && (lineLeft < threshold)) { //if all sensors
          sparki.moveLeft(45);
          sparki.moveForward(2);
          state = MOVELINE;
          break;
        }
        //
        if ((lineCenter < threshold) && (lineRight > threshold) && (lineLeft > threshold)) { //just the center
          sparki.moveStop();
          state = MOVELINE;
          break;

        }
        //        if(lineCenter < threshold){
        //          sparki.moveStop();
        //          state = MOVELINE;
        //          break;
        //        }
        delay(100);
      }

    // case not tested
    case MOVELINE:
      /* printing state to help debug */
      sparki.println("moveline");
      sparki.updateLCD();
      while (true) {
        lineLeft   = sparki.lineLeft();   // measure the left IR sensor
        lineCenter = sparki.lineCenter(); // measure the center IR sensor
        lineRight  = sparki.lineRight();  // measure the right IR sensor
        sparki.print("Left: ");
        sparki.println(lineLeft);
        sparki.print("Center: ");
        sparki.println(lineCenter);
        sparki.print("Right: ");
        sparki.println(lineRight);
        sparki.updateLCD();
        sparki.clearLCD();
        // if the center line sensor is the only one reading a line

        if ( (lineCenter < threshold) && (lineLeft < threshold) && (lineRight < threshold)) //all
        {

          if (!magBin) {
            sparki.moveStop();
            delay(1000);
            state = DROPBIN;

            break;
          }
          else  {
            sparki.println("TIS MAGNETIZED");
            sparki.updateLCD();
            magBin = false;
            sparki.moveForward(5);


          }


        }
        else if ( (lineCenter < threshold) && (lineLeft > threshold) && (lineRight > threshold) )
        {
          sparki.moveForward();
        }
        else if ( lineLeft < threshold) // if line is below left line sensor
        {
          sparki.moveLeft();
        }

        else if ( lineRight < threshold) // if line is below right line sensor
        {
          sparki.moveRight();
        }


        delay(100); // wait 0.1
      }

    case DROPBIN:
      sparki.moveRight(90);
      sparki.moveForward(dropDec);
      sparki.println("moving forward");
      delay(2000);
      sparki.gripperOpen();
      sparki.println("opening");
      delay(2000);
      sparki.gripperStop();
      sparki.println("closing");
      delay(100);
      sparki.moveBackward(dropDec);
      sparki.println("moving backward");
      delay(2000);
      dropDec = dropDec - 5;
      sparki.moveRight(180);
      counter++;
      if (counter < 4) {
        state = SEARCH;
        break;
      }
      else {
        state = GOHOME;
        break;
      }



    case GOHOME:
      while (true) {
        lineLeft   = sparki.lineLeft();   // measure the left IR sensor
        lineCenter = sparki.lineCenter(); // measure the center IR sensor
        lineRight  = sparki.lineRight();

        if ( (lineCenter > threshold) && (lineLeft > threshold) && (lineRight > threshold) )
        {
          sparki.moveBackward();
        }

        if ((lineRight < threshold) && (lineCenter > threshold) && (lineLeft > threshold)) { //if only right sensor
          sparki.moveForward();
        }
        if ((lineRight < threshold) && (lineCenter < threshold) && (lineLeft > threshold)) { //if right and center
          sparki.moveForward();
        }
        //        if((lineRight > threshold) && (lineCenter < threshold) && (lineLeft < threshold)){
        //          sparki.println("OH NO");
        //        }
        if ((lineRight < threshold) && (lineCenter < threshold) && (lineLeft < threshold)) { //if all sensors
          sparki.moveRight(45);
          sparki.moveForward(2);
          state = MOVELINE;
          break;
        }
        //
        if ((lineCenter < threshold) && (lineRight > threshold) && (lineLeft > threshold)) { //just the center
          sparki.moveStop();
          state = MOVELINE;
          break;

        }
      }





  }



  delay(100); // wait 0.1 seconds (100 milliseconds)
}
