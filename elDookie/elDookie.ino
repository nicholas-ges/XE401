/*  
 *   Code to operate the MMP-5 with a Sabertooth Motor Controller set to R/C mode
 *   The Sabertooth expects to receive a "center" or "stop" position first (as if the R/C 
 *   joystick was in the neutral position.
 *   Then it expect to receive the end limits.
 *   If this code is not executed when the controller is powered up, the MMP-5 may move
 *   eratically or not at all
 *   COL Lisa A. Shay
 */

#include <Servo.h> 

//------------------ Servo setup --------------------------------------------
Servo myservoL;  // create servo object to control a servo 
Servo myservoR;  // twelve servo objects can be created on most boards

//------------------- PING setup --------------------------------------
const int LeftPingPin = 7;
const int RightPingPin = 8;

//------------------- Var. Establishment -----------------------------
// establish variables for duration of the ping, and the distance result
// in inches and centimeters:
long lduration, rduration, linches, rinches;
int dark = 0; //state variable to enable 
int i = 0;      // loop counter
const int dur = 2;  // counter duration 2 is the minimun setting to 
                    // produce reliable results.  Increase to 25 if 
                    // the motor controller behaves eratically.
void setup() {
  Serial.begin(9600);
  myservoL.attach(10);   // attaches the servo on pin 10 to the servo object 
  myservoR.attach(13);
}

void loop() {
// --- Confirmed stop for left is 1400 and right is 1500 ---
//---------------- 2000 is reverse -------------------------
//---------------- 1000 is forward -------------------------


//------------- Testing for Photoresistors ----------------------------
  Serial.print("Left Voltage: ");
  Serial.println(volts(A2));//.36-.55 A2: ground, A1: light
  Serial.print(" Right Voltage: ");
  Serial.print(volts(A3));//1.01-1.53 A3: ground (.11 is its darkest value), A0: light
  
//-------------- CHECK FOR LIGHT OR DARK -------------------
if(volts(A0)<0.25 && volts(A1)<0.25)   //sense entering a dark room, engage dark mode
  {
    dark = 1;       //establish dark mode
    Serial.println("dark mode");
    digitalWrite(1, HIGH);

  }
if(dark == 0){
    digitalWrite(1, LOW);
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(LeftPingPin, OUTPUT);
  digitalWrite(LeftPingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(LeftPingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(LeftPingPin, LOW);
  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  pinMode(LeftPingPin, INPUT);
  lduration = pulseIn(LeftPingPin, HIGH);
  
  pinMode(RightPingPin, OUTPUT);
  digitalWrite(RightPingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(RightPingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(RightPingPin, LOW);
  pinMode(RightPingPin, INPUT);
  rduration = pulseIn(RightPingPin, HIGH);
  
   // convert the time into a distance
  linches = microsecondsToInches(lduration);
  rinches = microsecondsToInches(rduration);
  
  //print status
  /*Serial.print("Right Ping: ");
  Serial.print(rinches);
  Serial.print("in, ");
  Serial.print("Left Ping: ");
  Serial.print(linches);
  Serial.print("in, ");
  Serial.println();*/
  if(linches < 7 || rinches < 7) //if anything is super close, reverse
    {
      move(110, 200,25);         //both wheels reverse for 20  
    }
  else
    {
        if(linches < 14 || rinches < 14 || volts(A2)<0.43 || volts(A3)<1.10) //volt(A2)<_ || volta(A3)<___
        {   
           if (linches == rinches) 
           {
             move(0,-250,15);    //move forward        
           }
           else if ((linches < rinches) || volts(A2)<0.43) //Turn Right //|| volt(A2)<___
           {
             //Serial.println(" Right turn ");
             move(-90,100,15);    //left wheel forward, right stop, 20 sec 
           }
           else if ((rinches < linches) || volts(A3)<1.10)//Turn Left // || volt(A3)<__
           {
             //Serial.println(" Left Turn ");

             move(0,-150,15);    //right wheel forward, left stop, 20 sec
           }
        }
        else
        {
          move(-80,-10,20);    //move forward
        }
    }
  }
  else{
    darkProcedure();
  }
  
    
}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are 73.746
  // microseconds per inch (i.e. sound travels at 1130 feet per second).
  // This gives the distance travelled by the ping, outbound and return,
  // so we divide by 2 to get the distance of the obstacle.
  
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

//moving and turning function

//https://www.arduino.cc/en/tutorial/AnalogInput

void move(int left, int right, int t){
    //backward full speed: 200, forward full speed: -200, stop: 0
    myservoL.writeMicroseconds(1300 + left);    // left servo turn 
    myservoR.writeMicroseconds(1400 + right);    // right servo stop
    delay(t);
}

void darkProcedure(){
  if(volts(A0)>0.04 && volts(A1)<0.04){  //if right photoresistor sees more light, turn right as move
    Serial.println("dark right");
    move(-90,100,15);     //move forward but more right
  }
  else if(volts(A1)>0.04 && volts(A0<0.04)){  //if left photoresistor sees more light, turn left as move
    Serial.println("dark left");
    move(-100,-200,15);     
  }
  else if(volts(A1) >0.04 && volts(A0)>0.04){                     //else move forward
    Serial.println("dark forward");
    move(0,20,15);
  }
    else{                     //else move forward
    Serial.println("dark forward");
    move(-100,-100,15);
  }
}
// 2 left, 3 right
float volts(int adPin)                       // Measures volts at adPin
{                                            // Returns floating point voltage
 return float(analogRead(adPin)) * 5.0 / 1024.0;
}    


