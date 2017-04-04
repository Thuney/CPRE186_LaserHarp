   //Declare pin functions on Redboard
#define stp 2
#define dir 3
#define MS1 4
#define MS2 5
#define EN  6
#define LASER 7
#define STEPSIZE 1.8

const double startingAngle = 27; //Degrees
const double endingAngle = 63;   //Degrees

char user_input;
int numBeams = 11;
int currentStep = 1;
bool forward = true; //true is forward, false is backward

void setup() {
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(EN, OUTPUT);
//  pinMode(LASER, OUTPUT);
  resetEDPins(); //Set step, direction, microstep and enable pins to default states
  Serial.begin(9600);
  digitalWrite(EN, LOW);
  toStartingAngle(startingAngle);
}

 //Main loop
void loop() {
  while(Serial.available())
  {
    char input = Serial.read();
    if(input == '1') {
      digitalWrite(EN, LOW);
      for(int i=0;i<1000;i++)
        nextStep(2);   
    }
    resetEDPins();
  }
}

void toStartingAngle(int angle)
{
  for(int i=0; i < angle/STEPSIZE; i++)
  {
    stepMotor();
  }
}

void nextStep(int incSteps)
{ 
  currentStep += (forward) ? 1 : -1;
  for(int i=0; i<incSteps; i++)
  {
    stepMotor();
  }
  if(currentStep == numBeams || currentStep == 1) switchDirection();
}

void switchDirection()
{
  forward = !forward;
  if(forward)
    digitalWrite(dir, HIGH);
  else
    digitalWrite(dir, LOW);
}

void stepMotor()
{
  digitalWrite(stp, HIGH);
  delay(2);
  digitalWrite(stp, LOW);
  delay(2);
}

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


