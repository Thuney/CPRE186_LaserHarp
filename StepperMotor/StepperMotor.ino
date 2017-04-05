   //Declare pin functions on Redboard
#define stp 2
#define dir 3
#define MS1 4
#define MS2 5
#define EN  6
#define LASER 7
#define STEPSIZE 1.8

//Information about our range of beams determined via math
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
const double startingAngle = 27; //Degrees
const double endingAngle = 63;   //Degrees
const int numBeams = 11;
const int currentStep = 1;

//Variables for tracking current states
char user_input;
bool forward = true; //true is forward, false is backward

void setup() {
  //Define our pins as input or output
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(EN, OUTPUT);
//  pinMode(LASER, OUTPUT);

  //Set initial voltages to the pins
  resetEDPins(); //Set step, direction, microstep and enable pins to default states

  //Start serial monitor (mostly for debugging)
  Serial.begin(9600);
  //Move to our starting location
  toStartingAngle(startingAngle);
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
        nextStep(2);   
        
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
 * the method above it
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


