//Lab 1 CSCI 3302 Robotics
//Julio Lemos
//Nhan Nguyen
//Eliaz Ortiz
//David Stone

#include <Sparki.h> // include the sparki library
#define START 0
#define SEARCH 1
#define GRIP 2
#define FINDLINE 3
#define MOVELINE 4
#define DROP 5
#define DONE 10
#define NO_LCD // disables the LCD, frees up 3088 Bytes Flash Memory, 1k RAM
#define NO_ACCEL
int state;
int cm;
int objectDistance = 5;
int threshold = 500;
int lineLeft;
int lineCenter;
int lineRight;
int onLine = 0;
int checkCenterLine = 0;
bool magBin = false;


void setup()
{
  sparki.servo(SERVO_CENTER); // Center the Servo
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
//      sparki.println("start");
//      sparki.updateLCD();
      delay(1000);

      sparki.gripperOpen();
      delay(2000);
      sparki.gripperStop();
      delay(1500);
      state = SEARCH;
      break;

    // case tested - working
    case SEARCH:
      /* printing state to help debug */
//      sparki.println("search");
//      sparki.updateLCD();

      sparki.RGB(RGB_GREEN); // turn the light green
      //sparki.moveForward(); // move Sparki forward
      while (true)
      {
        sparki.moveLeft(20);
        cm = sparki.ping(); // measures the distance with Sparki's eyes
        delay(1500);

        if (cm != -1) { // ping is -1 if sparki doesn't see anything
          if (cm < 60) // if the distance measured is less than 15 centimeters
          {
            sparki.RGB(RGB_RED); // turn the light red
            delay(1000);
            sparki.RGB(RGB_GREEN);
//            sparki.println(cm);
//            sparki.updateLCD();
            sparki.beep(); // beep!
            sparki.moveForward(cm);
            cm = sparki.ping();
            delay(6000);
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
//      sparki.println("grip");
//      sparki.updateLCD();
      sparki.gripperClose();
      delay(3000);
      sparki.gripperStop();
      state = CHECKMAG;
      break;

    case CHECKMAG:
      while(true){
        float y  = sparki.magY();   // measure the accelerometer y-axis
        delay(300);
  //      sparki.clearLCD();
        // write the measurements to the screen
        if(y > -400){
          magBin = true;
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
//      sparki.println("findline");
//      sparki.updateLCD();
      threshold = 500;
      sparki.moveRight(180);
      delay(5000);
      while (true) {
        lineCenter = sparki.lineCenter(); // measure the center IR sensor
        lineLeft = sparki.lineLeft();
        lineRight = sparki,lineRight();
        if (lineCenter > threshold && lineLeft > threshold && lineRight > threshold) // if nothing 
        {
          sparki.moveForward();
        }
        if(lineRight < threshold && lineCenter > threshold && lineLeft > threshold){ //if only right sensor
          sparki.moveForward();
        }
        if(lineRight < threshold && lineCenter < threshold && lineLeft > threshold){ //if right and center
          sparki.moveLeft();
        }
        if(lineRight < threshold && lineCenter < threshold && lineLeft < threshold){ //if all sensors
          sparki.moveLeft(90);
        }
        
        if (lineCenter < threshold && lineRight > threshold && lineLeft > threshold)
        {
          //delay(100);
          sparki.moveStop();
          state = MOVELINE;
          break;
          
        }
        delay(100);
      }

    // case not tested
    case MOVELINE:
      /* printing state to help debug */
//      sparki.println("moveline");
//      sparki.updateLCD();
      while (true) {
        threshold = 700;
        lineLeft   = sparki.lineLeft();   // measure the left IR sensor
        lineCenter = sparki.lineCenter(); // measure the center IR sensor
        lineRight  = sparki.lineRight();  // measure the right IR sensor
        delay(100);

        // if the center line sensor is the only one reading a line
        if ( (lineCenter < threshold) && (lineLeft > threshold) && (lineRight > threshold) )
        {
          sparki.moveForward(); // move forward
        }
        if ( lineLeft < threshold) // if line is below left line sensor
        {
          sparki.moveLeft(); // turn left
        }

        if ( lineRight < threshold) // if line is below right line sensor
        {
          sparki.moveRight(); // turn right
        }
        if ( (lineCenter < threshold) && (lineLeft < threshold) && (lineRight < threshold))
          {
            
            if(!magBin){
              sparki.moveStop();
              delay(1000);
              state = DROP;
              break;
            }
            else{
              magBin = false;
            }
            
          }        

      delay(100); // wait 0.1
      }
      

      

    case DROP:
//      sparki.println("drop");
//      sparki.updateLCD();
      sparki.gripperOpen();
      delay(2000);
      sparki.gripperStop();
      state = DONE;
      break;

    case DONE:
      //sparki.clearLCD();
//      sparki.println("Task completed my human overlords");
//      sparki.updateLCD();      
      break;
   
  }



  delay(100); // wait 0.1 seconds (100 milliseconds)
}
