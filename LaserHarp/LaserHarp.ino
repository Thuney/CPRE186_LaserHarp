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
int currentStep = 1;
//Test
int currentNote = 0;
int steps = 0;

//Variables for tracking current states
char user_input;
bool forward = true; //true is forward, false is backward
int photogate_baseline = 0;
int photogate_trigger = 200;

void setup() {
  //Define our pins as input or output
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(LASER, OUTPUT);
  digitalWrite(LASER, LOW);
  pinMode(NOISE_OUT, OUTPUT);

  //Set initial voltages to the pins
  resetEDPins(); //Set step, direction, microstep and enable pins to default states

  //Start serial monitor (mostly for debugging)
  Serial.begin(9600);
  //Move to our starting location
  toStartingAngle(startingAngle);
  //Defines the base light level in the room
  photogate_baseline = analogRead(PHOTOGATE);
}

//Main loop
void loop() {
    //Next user input character
    char input = Serial.read();
    boolean playing = false;
    if(input == '1')
      //Oscillate 1000 times
      for(int i=0;i<3000;i++)
      {
          if((input = Serial.read()) == '0') break;
          int photogate_val = analogRead(PHOTOGATE);
          int difference = photogate_val - photogate_baseline;
          //If a beam has been broken and we aren't playing a note
          if((difference > photogate_trigger && !playing) || (difference > photogate_trigger && currentStep != currentNote))
          {
            //Play the tone depending on what beam is being broken
            playTone(currentStep);
            playing = true;
            currentNote = currentStep;
          }
          else if(difference < (photogate_trigger-(photogate_trigger*.1)) && currentStep == currentNote)
          {
            //Otherwise, don't play any tone
            noTone(NOISE_OUT);
            playing = false;
          }
          nextStep((endingAngle - startingAngle)/(numBeams-1));
          pulseLaser();
      }
    //Reset the pins to their defaults
    resetEDPins();
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
}

//Reset Easy Driver pins to default states
void resetEDPins()
{
  digitalWrite(stp, LOW);
  digitalWrite(dir, HIGH);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(EN, HIGH);
  digitalWrite(LASER, LOW);
  noTone(NOISE_OUT);
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
      frequency = NOTE_C4;
      break;
    case 10:
      frequency = NOTE_D4;
      break;
    case 9:
      frequency = NOTE_E4;
      break;
    case 8:
      frequency = NOTE_F4;
      break;
    case 7:
      frequency = NOTE_G4;
      break;
    case 6:
      frequency = NOTE_A4;
      break;
    case 5:
      frequency = NOTE_B4;
      break;
    case 4:
      frequency = NOTE_C5;
      break;
    case 3:
      frequency = NOTE_D5;
      break;
    case 2:
      frequency = NOTE_E5;
      break;
    case 1:
      frequency = NOTE_F5;
      break;
  }
  tone(NOISE_OUT, frequency);
}
