                                                                                                                                                                                                                                                                                                                  
  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
//#include <math.h> 

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  460// This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 60 // Analog servos run at ~50 Hz updates
#define Pin0 0
// our servo # counter
const int servo_pin[4][3] = { {4, 5, 6}, {8, 9, 10}, {12, 13, 14}, {0, 1, 2} };
volatile float site_now[4][3];    //real-time coordinates of the end of each leg
volatile float site_expect[4][3]; //expected coordinates of the end of each leg
const float Pi = 3.1415926;
uint8_t servonum = 0;
bool stt=0;
void setup() {
  Serial.begin(9600);
  Serial.println("8 channel Servo test!");
  pinMode(Pin0,INPUT_PULLUP);
  pwm.begin();
  /*
   * In theory the internal oscillator (clock) is 25MHz but it really isn't
   * that precise. You can 'calibrate' this by tweaking this number until
   * you get the PWM update frequency you're expecting!
   * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
   * is used for calculating things like writeMicroseconds()
   * Analog servos run at ~50 Hz updates, It is importaint to use an
   * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
   * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
   *    the I2C PCA9685 chip you are setting the value for.
   * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
   *    expected value (50Hz for most ESCs)
   * Setting the value here is specific to each individual I2C PCA9685 chip and
   * affects the calculations for the PWM update frequency. 
   * Failure to correctly set the int.osc value will cause unexpected PWM results
   */
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);
}

// You can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. It's not precise!

void loop() {
  if(digitalRead(Pin0)==LOW) stt=(stt==1)? 0:1;
  if(stt==1) Ctrl_Ser();
  if(stt==0) move_forward(8);
   

 // move_forward(8);
  
 
   
   /*  move_to(x,y,z,0);
     Serial.println("Lan 1");
    Serial.println("");
    delay(800);
    
      move_to(6,6,2,0);
     
     Serial.println("Lan 2");
     Serial.println("");
     delay(800);*/
   
   
}
//toa do x+y phai lon hon 3
//X MAX 9, Y MAX 9, Z MAX 7
void move_to(volatile double x,volatile double y,volatile double z, int numleg){
  //if(numleg==1||numleg==3) z=7-z;
  if(z==0) z+=0.1;
  if(y==0) y+=0.1;
  if(x==0) x+=0.1;
  double a,R,g1,t1,t2,g3,pulse1,pulse2,pulse3;
  a=sqrt(pow(x,2)+pow(y,2))-3;
  R=sqrt(pow(a,2)+pow(z,2));
  g1=((atan(y/x)) * 180) / Pi;
  if(numleg==0||numleg==2) pulse1=map(g1,0,100,SERVOMIN,SERVOMAX);
  if(numleg==1||numleg==3) pulse1=map(g1,0,100,SERVOMAX,SERVOMIN);
  g3=acos((52-pow(R,2))/48);
  g3=g3*180/Pi;
  g3-=30;
  
  if(numleg==0||numleg==2) pulse3=map(g3,0,100,SERVOMIN,SERVOMAX);
  if(numleg==1||numleg==3) pulse3=map(g3,0,100,SERVOMAX,SERVOMIN);
  t1=((atan(a/z)) * 180) / Pi;
  t2=acos((pow(R,2)-20)/(R*8));
  t2=t2*180/Pi;
  
  
  if(numleg==0||numleg==2) pulse2=map(t1+t2-60,0,100,SERVOMIN,SERVOMAX);
  if(numleg==1||numleg==3) pulse2=map(t1+t2-60,0,100,SERVOMAX,SERVOMIN);

  pwm.setPWM(servo_pin[numleg][1],0,pulse2);
 
  pwm.setPWM(servo_pin[numleg][2],0,pulse3);
  pwm.setPWM(servo_pin[numleg][0],0,pulse1);
 
  Serial.println(pulse2);
  Serial.println(t1+t2);
  Serial.println(pulse3);
   Serial.println(g3);
    Serial.println(pulse1);
     Serial.println(g1);
}
void Ctrl_Ser(){
  
   if (Serial.available()) {
    Serial.println("");
   String cmd = Serial.readStringUntil('\n');
    int s1 = cmd.indexOf(',');
    int s2 = cmd.indexOf(',', s1 + 1);
    int s3 = cmd.indexOf(',', s2 + 1);
    String x = cmd.substring(0, s1);
    String y = cmd.substring(s1 + 1, s2);
    String z = cmd.substring(s2 + 1, s3);
    String numleg = cmd.substring(s3+1,cmd.length());
 /*  pwm.setPWM(servo_pin[0][1],0,SERVOMIN);
    delay(1000);
      pwm.setPWM(servo_pin[0][1],0,SERVOMAX);
      delay(1000);*/
    Serial.println("Lan 1");
    Serial.println("");
    delay(800);
    move_to(x.toInt(),y.toInt(),z.toInt(),numleg.toInt());
   }  
   
}
void move_forward(int step){
  while (step-- > 0)
  { 
  
    //leg 3 move
    move_to(3,0,2,3);
    delay(200);
     move_to(3,0,3,3);
    delay(130);
    //leg 0 move
    move_to(2,2,2,0);
   delay(130);
     move_to(3,6,3,0);
    delay(200);
    //body move
    move_to(3,3,3,0);
    move_to(3,0,3,1);
    move_to(3,6,3,2);
    move_to(3,3,3,3);
    delay(200);
    
    
       //leg 2 move
    move_to(3,0,2,2);
    delay(200);
     move_to(3,0,3,2);
    delay(130);
    //leg 1 move
    move_to(2,2,2,1);
     delay(130);
     move_to(3,6,3,1);
    delay(200);
    //body move
    move_to(3,3,3,1);
    move_to(3,0,3,0);
    move_to(3,6,3,3);
    move_to(3,3,3,2);
    delay(200);
   
    
    
  }
}
