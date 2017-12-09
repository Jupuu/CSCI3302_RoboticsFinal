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
int state;
int cm;
int objectDistance = 5;
int threshold = 500;
int lineLeft;
int lineCenter;
int lineRight;
int onLine = 0;


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
      sparki.println("start");
      sparki.updateLCD();
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
      sparki.println("search");
      sparki.updateLCD();

      sparki.RGB(RGB_GREEN); // turn the light green
      //sparki.moveForward(); // move Sparki forward
      while (true)
      {
        sparki.moveLeft(20);
        cm = sparki.ping(); // measures the distance with Sparki's eyes
        delay(1500);

        if (cm != -1) { // ping is -1 if sparki doesn't see anything
          if (cm < 30) // if the distance measured is less than 15 centimeters
          {
            sparki.RGB(RGB_RED); // turn the light red
            delay(1000);
            sparki.RGB(RGB_GREEN);
            sparki.println(cm);
            sparki.updateLCD();
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
      sparki.println("grip");
      sparki.updateLCD();
      sparki.gripperClose();
      delay(3000);
      sparki.gripperStop();
      state = FINDLINE;
      break;

      
    case FINDLINE:
      sparki.println("findline");
      sparki.updateLCD();
      threshold = 500;
      sparki.moveRight(180);
      delay(5000);
      while (true) {
        lineCenter = sparki.lineCenter(); // measure the center IR sensor
        if (lineCenter > threshold)
        {
          sparki.moveForward();
        }

        if (lineCenter < threshold)
        {
          sparki.moveRight();
          state = MOVELINE;
          break;
        }
        delay(100);
      }

    // case not tested
    case MOVELINE:
      /* printing state to help debug */
      sparki.println("moveline");
      sparki.updateLCD();
      while (true) {
        threshold = 500;
        lineLeft   = sparki.lineLeft();   // measure the left IR sensor
        lineCenter = sparki.lineCenter(); // measure the center IR sensor
        lineRight  = sparki.lineRight();  // measure the right IR sensor
        delay(100);

        // if the center line sensor is the only one reading a line
        if ( (lineCenter < threshold) && (lineLeft > threshold) && (lineRight > threshold) )
        {
          sparki.moveForward(); // move forward
        }
        if ( lineLeft < threshold ) // if line is below left line sensor
        {
          sparki.moveLeft(); // turn left
        
          onLine == 1;
        }

        if ( lineRight < threshold ) // if line is below right line sensor
        {
          sparki.moveRight(); // turn right
   
          onLine == 1;
        }
        if ( (lineCenter < threshold) && (lineLeft < threshold) && (lineRight < threshold))
          {
            sparki.moveStop();
            delay(1000);
            state = DROP;
            break;
          }        


      sparki.clearLCD(); // wipe the screen
      sparki.print("Line Left: "); // show left line sensor on screen
      sparki.println(lineLeft);
      sparki.print("Line Center: "); // show center line sensor on screen
      sparki.println(lineCenter);
      sparki.print("Line Right: "); // show right line sensor on screen
      sparki.println(lineRight);
      sparki.updateLCD(); // display all of the information written to the screen

      delay(100); // wait 0.1
      }
      

      

    case DROP:
      sparki.println("drop");
      sparki.updateLCD();
      sparki.gripperOpen();
      delay(2000);
      sparki.gripperStop();
      state = DONE;
      break;

    case DONE:
      //sparki.clearLCD();
      sparki.println("Task completed my human overlords");
      sparki.updateLCD();      
      break;
   
  }



  delay(100); // wait 0.1 seconds (100 milliseconds)
}
