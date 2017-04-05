#include "notes.h"

//Declare pin positions on Arduino
#define stp 2
#define dir 3
#define MS1 4
#define MS2 5
#define EN  6
#define LASER 7
#define NOISE_OUT 8
#define PHOTOGATE A0
#define STEPSIZE 1.8

//Information about our range of beams determined via math, logic, and physics
/**What's important to note here is that the angle of incidence -- (where our beam goes) --
 * is twice the angle the mirror makes with the x-axis, where positive angles are counterclockwise.
 * 
 * With this in mind, the step size of our stepper motor is 1.8 degrees per pulse, so our
 * starting beam angle, ending beam angle, and intermediate beam angles have to all be divisible by this factor.
 * 
 * Finally, since any change we make to the angle of the stepper motor will affect the incident beam by a factor of 2,
 * we lose a lot of precision in our stepper motor. As such, these numbers were picked because they were the easiest
 * to work with.
**/
//These angle measures are referring to the angle of the mirror, not the beam angle
const double startingAngle = 27; //Degrees
const double endingAngle = 63;   //Degrees

const int numBeams = 11;
const int currentStep = 1;

//Variables for tracking current states
char user_input;
bool forward = true; //true is forward, false is backward
int photogate_baseline = 0;

void setup() {
  //Define our pins as input or output
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(EN, OUTPUT);
  //pinMode(LASER, OUTPUT);
  pinMode(NOISE_OUT, OUTPUT);
  pinMode(PHOTOGATE, INPUT);

  //Set initial voltages to the pins
  resetEDPins(); //Set step, direction, microstep and enable pins to default states

  //Start serial monitor (mostly for debugging)
  Serial.begin(9600);
  //Move to our starting location
  toStartingAngle(startingAngle);
  //Defines the base light level in the room
//  photogate_baseline = analogRead(PHOTOGATE);
}

//Main loop
void loop() {
  //Loop when the serial monitor is open
  while(Serial.available())
  {
    //Next user input character
    char input = Serial.read();
    
    if(input == '1')
      //Oscillate 1000 times
      for(int i=0;i<1000;i++)
      {
        /* --COMMENTED OUT BECAUSE CIRCUIT ISN'T UP TO DATE WITH THIS FUNCTIONALITY --
          //Current reading from photogate
          int photogate_current = analogRead(PHOTOGATE);
          //If a beam has been broken
          if(photogate_current - photogate_baseline > 200) //Values may need tweaking
            //Play the tone depending on what beam is being broken
            playTone(currentStep);
          else
            //Otherwise, don't play any tone
            noTone();
        */
        nextStep((endingAngle - startingAngle)/(numBeams-1));
      }
        
    //Reset the pins to their defaults
    resetEDPins();
  }
}

/**
 * Moves the stepper motor to the specified starting angle
 */
void toStartingAngle(int angle)
{
  //Enable motor control
  digitalWrite(EN, LOW);
  //Step the motor as many times as it takes to pass the target angle
  for(int i=0; i < angle/STEPSIZE; i++)
    stepMotor();
  //Disable motor control
  digitalWrite(EN, HIGH);
}

/**
 * Moves the motor to the next beam position, accounting
 * for changes in direction.
 */
void nextStep(int incSteps)
{ 
  currentStep += (forward) ? 1 : -1;
  //Allow motor control
  digitalWrite(EN, LOW);
  for(int i=0; i<incSteps; i++) //Loop as many times as is specified
    stepMotor();
  //Switch direction if we're at either extreme of the range
  if(currentStep == numBeams || currentStep == 1) switchDirection();
}

/**
 * Flips the direction the motor rotates, setting pins accordingly.
 */
void switchDirection()
{
  forward = !forward;
  if(forward)
    digitalWrite(dir, HIGH);
  else
    digitalWrite(dir, LOW);
}

/**
 * Pulses the motor one step, delaying 2ms between high/low pulses
 */
void stepMotor()
{
  digitalWrite(stp, HIGH);
  delay(2);
  digitalWrite(stp, LOW);
  delay(2);
}

/**
 * Pulses laser for 1ms. This actually might need to be incorporated into
 * the function above it
 */
void pulseLaser()
{
  digitalWrite(LASER, HIGH);
  delay(1);
  digitalWrite(LASER, LOW);
  delay(1);
}

//Reset Easy Driver pins to default states
void resetEDPins()
{
  digitalWrite(stp, LOW);
  digitalWrite(dir, HIGH);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(EN, HIGH);
//  digitalWrite(LASER, LOW);
}

/**
 * Sets a tone depending on the currentStep position
 */
void playTone(int currentStep)
{
  int frequency;
  //Note declarations are in "notes.h"
  /**
   * Honestly, it might be slightly confusing as to why I did my cases in decreasing order,
   * but the way the currentStep is set up, the farthest beam to the right is currentStep == 1,
   * and I wanted the notes to be in ascending order from left to right
   */
  switch(currentStep)
  {
    case 11:
      frequency = NOTE_C2;
      break;
    case 10:
      frequency = NOTE_D2;
      break;
    case 9:
      frequency = NOTE_E2;
      break;
    case 8:
      frequency = NOTE_F2;
      break;
    case 7:
      frequency = NOTE_G2;
      break;
    case 6:
      frequency = NOTE_A2;
      break;
    case 5:
      frequency = NOTE_B2;
      break;
    case 4:
      frequency = NOTE_C3;
      break;
    case 3:
      frequency = NOTE_D3;
      break;
    case 2:
      frequency = NOTE_E3;
      break;
    case 1:
      frequency = NOTE_F3;
      break;
  }
  tone(NOISE_OUT, frequency);
}




