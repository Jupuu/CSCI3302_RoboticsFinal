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
int dropDec = 30;
bool magBin = false;


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
      break;

    // case tested - working
    case SEARCH:
      /* printing state to help debug */
      sparki.println("search");
      sparki.updateLCD();

      sparki.RGB(RGB_GREEN); // turn the light green
      //sparki.moveForward(); // move Sparki forward
      while (true)
      {
        sparki.moveLeft(20);
        delay(100);
        cm = sparki.ping();
        delay(100);
        sparki.println(cm);
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
      break;

    case CHECKMAG:
      while(true){
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
        if(totalmag > 1500){
           magBin = true;
           sparki.println("ITS MAG");
           state = FINDLINE;
           break;
        }
        else{
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
        if ((lineCenter > threshold) && (lineLeft > threshold) && (lineRight > threshold)) // if nothing 
        {
          sparki.moveForward();
        }
        if((lineRight < threshold) && (lineCenter > threshold) && (lineLeft > threshold)){ //if only right sensor
          sparki.moveForward();
        }
        if((lineRight < threshold) && (lineCenter < threshold) && (lineLeft > threshold)){ //if right and center
          sparki.moveForward();
        }
//        if((lineRight > threshold) && (lineCenter < threshold) && (lineLeft < threshold)){
//          sparki.println("OH NO");
//        }
        if((lineRight < threshold) && (lineCenter < threshold) && (lineLeft < threshold)){ //if all sensors
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

        // if the center line sensor is the only one reading a line
        if ( (lineCenter < threshold) && (lineLeft > threshold) && (lineRight > threshold) )
        {
          sparki.moveForward(); 
        }
        if ( lineLeft < threshold) // if line is below left line sensor
        {
          sparki.moveLeft();
        }

        if ( lineRight < threshold) // if line is below right line sensor
        {
          sparki.moveRight();
        }
        if ( (lineCenter < threshold) && (lineLeft < threshold) && (lineRight < threshold)) //all
          {
            
            if(!magBin){
              sparki.moveStop();
              delay(1000);
              state = DROPBIN;
              break;
            }
            else{
              sparki.println("TIS MAGNETIZED");
              sparki.updateLCD();
              magBin = false;
              sparki.moveForward(5);
            }
            
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
      state = GOHOME;
      break;


   
   
  }



  delay(100); // wait 0.1 seconds (100 milliseconds)
}
