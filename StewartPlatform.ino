/* <Controlling code for Arduino Controlled Rotary Stewart Platform>
    Copyright (C) <2017>  <Jared Korthuis>
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <Servo.h>
#include <Wire.h>
#include <ArduinoNunchuk.h>

#define BAUDRATE 19200
#define CONTROLLER_MAX 200
#define CONTROLLER_MIN -200
#define CONTROLLER_X_MIN -2
#define CONTROLLER_X_MAX 2
#define CONTROLLER_Y_MIN -2
#define CONTROLLER_Y_MAX 2


//define of characters used for control of serial communication ['0'-'8']
#define SETBACKOFF 48
#define SETBACKON 49
#define SETPOSITIONS 50
#define PRINTPOS 51
#define STOPPRINTPOS 52
#define SWITCHIRDA 53
#define SETPOSITIONSINMS 54
#define SWITCHIRDAOFF 55
#define GEPOSITION 56

//MIN and MAX PWM pulse sizes, they can be found in servo documentation
#define MAX 2800
#define MIN 450
#define MID 1500

//Positions of servos mounted in opposite direction
#define INV1 1
#define INV2 3
#define INV3 5

//constants for computation of positions of connection points
#define pi  3.14159
#define deg2rad 180/pi
#define deg30 pi/6


//Array of servo objects
Servo servo[6];
//Zero positions of servos, in this positions their arms are perfectly horizontal, in us
static int zero[6]={MID,1400,MID,MID,MID,MID};
//In this array is stored requested position for platform - x,y,z,rot(x),rot(y),rot(z)
static float arr[6]={0,0.0,0.0, radians(0),radians(0),radians(0)};
//Actual degree of rotation of all servo arms, they start at 0 - horizontal, used to reduce
//complexity of calculating new degree of rotation
static float theta_a[6]={0.0,0.0,0.0, 0.0,0.0,0.0};
//Array of current servo positions in us
static int servo_pos[6];
//rotation of servo arms in respect to axis x
const float beta[] = {pi/2,-pi/2,-pi/6, 5*pi/6,-5*pi/6,pi/6},
//maximum servo positions, 0 is horizontal position
      servo_min=radians(-80),servo_max=radians(80),
//servo_mult - multiplier used for conversion radians->servo pulse in us
//L1-effective length of servo arm, L2 - length of base and platform connecting arm
//z_home - height of platform above base, 0 is height of servo arms
servo_mult=400/(pi/4),L1 = 0.79,L2 = 4.66, z_home = 4.05;
//RD distance from center of platform to attachment points (arm attachment point)
//RD distance from center of base to center of servo rotation points (servo axis)
//theta_p-angle between two servo axis points, theta_r - between platform attachment points
//theta_angle-helper variable
//p[][]=x y values for servo rotation points
//re[]{}=x y z values of platform attachment points positions
//equations used for p and re will affect postion of X axis, they can be changed to achieve
//specific X axis position
const float RD = 2.42,PD =2.99,theta_p = radians(37.5),
theta_angle=(pi/3-theta_p)/2, theta_r = radians(8),
      p[2][6]={
          {
            -PD*cos(deg30-theta_angle),-PD*cos(deg30-theta_angle),
            PD*sin(theta_angle),PD*cos(deg30+theta_angle),
            PD*cos(deg30+theta_angle),PD*sin(theta_angle)
         },
         {
            -PD*sin(deg30-theta_angle),PD*sin(deg30-theta_angle),
            PD*cos(theta_angle),PD*sin(deg30+theta_angle),
            -PD*sin(deg30+theta_angle),-PD*cos(theta_angle)
         }
      },
      re[3][6] = {
          {
              -RD*sin(deg30+theta_r/2),-RD*sin(deg30+theta_r/2),
              -RD*sin(deg30-theta_r/2),RD*cos(theta_r/2),
              RD*cos(theta_r/2),-RD*sin(deg30-theta_r/2),
          },{
              -RD*cos(deg30+theta_r/2),RD*cos(deg30+theta_r/2),
              RD*cos(deg30-theta_r/2),RD*sin(theta_r/2),
              -RD*sin(theta_r/2),-RD*cos(deg30-theta_r/2),
          },{
              0,0,0,0,0,0
          }
};
//arrays used for servo rotation calculation
//H[]-center position of platform can be moved with respect to base, this is
//translation vector representing this move
static float M[3][3], rxp[3][6], T[3], H[3] = {0,0,z_home};
//Set ArduinoNunchuk to nunchuk
ArduinoNunchuk nunchuk = ArduinoNunchuk();
void setup(){


//attachment of servos to PWM digital pins of arduino
   servo[0].attach(3, MIN, MAX);
   servo[1].attach(5, MIN, MAX);
   servo[2].attach(6, MIN, MAX);
   servo[3].attach(9, MIN, MAX);
   servo[4].attach(10, MIN, MAX);
   servo[5].attach(11, MIN, MAX);
//begin of serial communication
   Serial.begin(BAUDRATE);
//initialize nunchuk
nunchuk.init();
//putting into base position
   setPos(arr);
}

//function calculating needed servo rotation value
float getAlpha(int *i){
   static int n;
   static float th=0;
   static float q[3], dl[3], dl2;
   double min=servo_min;
   double max=servo_max;
   n=0;
   th=theta_a[*i];
   while(n<20){
    //calculation of position of base attachment point (point on servo arm where is leg connected)
      q[0] = L1*cos(th)*cos(beta[*i]) + p[0][*i];
      q[1] = L1*cos(th)*sin(beta[*i]) + p[1][*i];
      q[2] = L1*sin(th);
    //calculation of distance between according platform attachment point and base attachment point
      dl[0] = rxp[0][*i] - q[0];
      dl[1] = rxp[1][*i] - q[1];
      dl[2] = rxp[2][*i] - q[2];
      dl2 = sqrt(dl[0]*dl[0] + dl[1]*dl[1] + dl[2]*dl[2]);
    //if this distance is the same as leg length, value of theta_a is corrent, we return it
      if(abs(L2-dl2)<0.01){
         return th;
      }
    //if not, we split the searched space in half, then try next value
      if(dl2<L2){
         max=th;
      }else{
         min=th;
      }
      n+=1;
      if(max==servo_min || min==servo_max){
         return th;
      }
      th = min+(max-min)/2;
   }
   return th;
}

//function calculating rotation matrix
void getmatrix(float pe[])
{
   float psi=pe[5];
   float theta=pe[4];
   float phi=pe[3];
   M[0][0] = cos(psi)*cos(theta);
   M[1][0] = -sin(psi)*cos(phi)+cos(psi)*sin(theta)*sin(phi);
   M[2][0] = sin(psi)*sin(phi)+cos(psi)*cos(phi)*sin(theta);

   M[0][1] = sin(psi)*cos(theta);
   M[1][1] = cos(psi)*cos(phi)+sin(psi)*sin(theta)*sin(phi);
   M[2][1] = cos(theta)*sin(phi);

   M[0][2] = -sin(theta);
   M[1][2] = -cos(psi)*sin(phi)+sin(psi)*sin(theta)*cos(phi);
   M[2][2] = cos(theta)*cos(phi);
}
//calculates wanted position of platform attachment poins using calculated rotation matrix
//and translation vector
void getrxp(float pe[])
{
   for(int i=0;i<6;i++){
      rxp[0][i] = T[0]+M[0][0]*(re[0][i])+M[0][1]*(re[1][i])+M[0][2]*(re[2][i]);
      rxp[1][i] = T[1]+M[1][0]*(re[0][i])+M[1][1]*(re[1][i])+M[1][2]*(re[2][i]);
      rxp[2][i] = T[2]+M[2][0]*(re[0][i])+M[2][1]*(re[1][i])+M[2][2]*(re[2][i]);
   }
}
//function calculating translation vector - desired move vector + home translation vector
void getT(float pe[])
{
   T[0] = pe[0]+H[0];
   T[1] = pe[1]+H[1];
   T[2] = pe[2]+H[2];
}

unsigned char setPos(float pe[]){
    unsigned char errorcount;
    errorcount=0;
    for(int i = 0; i < 6; i++)
    {
        getT(pe);
        getmatrix(pe);
        getrxp(pe);
        theta_a[i]=getAlpha(&i);
        if(i==INV1||i==INV2||i==INV3){
            servo_pos[i] = constrain(zero[i] - (theta_a[i])*servo_mult, MIN,MAX);
        }
        else{
            servo_pos[i] = constrain(zero[i] + (theta_a[i])*servo_mult, MIN,MAX);
        }
    }

    for(int i = 0; i < 6; i++)
    {
        if(theta_a[i]==servo_min||theta_a[i]==servo_max||servo_pos[i]==MIN||servo_pos[i]==MAX){
            errorcount++;
        }
        servo[i].writeMicroseconds(servo_pos[i]);
    }
    for(int i =0; i<6; i++){
      Serial.println(servo_pos[i], DEC);
      
    }
    Serial.print("\n");
    return errorcount;
}



//main control loop, obtain requested action from serial connection, then execute it
void loop()
{
   //get responses from nunchuk
   nunchuk.update();
   if(Serial.available()>0){
      int input=Serial.read();
      switch(input){
//action to turn backlight off
         case SETBACKOFF:

//reserved for future use - possiblity to send just servo timing values
//main control would be executed on communicating partner
         case SETPOSITIONSINMS:
            for(int i=0;i<6;i++){
               long kk;
               while(Serial.available()<4){
                  ;
               }
               kk=(long)Serial.read();
               kk=kk|(Serial.read()<<8);
               kk=kk|(Serial.read()<<16);
               kk=kk|(Serial.read()<<24);
               servo[i].writeMicroseconds(kk);
            }
            break;

//return current position of platform
         case GEPOSITION:
            retPos();
            break;
         default:
            break;
      }
   }
   if(nunchuk.analogX <= 95 && nunchuk.analogY >= 120 && nunchuk.analogY <= 131){
    Serial.print("X: ");
    Serial.print(nunchuk.analogX, DEC);
    if(arr[0] != CONTROLLER_X_MIN){
       arr[0] -=1;
       setPos(arr);  
    }
   }
   if(nunchuk.analogX >=142 && nunchuk.analogY >= 120 && nunchuk.analogY <= 131){
    if(arr[0] != CONTROLLER_X_MAX){
       arr[0] +=1;
       setPos(arr);
    }
   }
   
   if(nunchuk.analogY <= 100 && nunchuk.analogX >= 120 && nunchuk.analogX <= 131){
    if(arr[1] != CONTROLLER_Y_MIN){
       arr[1] -= 1;
       setPos(arr);
    }
   }
   if(nunchuk.analogY >= 140 && nunchuk.analogX >= 120 && nunchuk.analogX <= 131){
    if(arr[1] != CONTROLLER_Y_MAX){
       arr[1] += 1;
       setPos(arr);
    }
   }

   //
   /*if(nunchuk.analogX >= 142 && nunchuk.analogY >= 140){
      if(arr[0] <= CONTROLLER_X_MAX){
         arr[0] +=1;
      }
      if(arr[1] <= CONTROLLER_Y_MAX){
         arr[1] +=1;
      }
         setPos(arr);
      }
      
   
   if(nunchuk.analogX <=105 && nunchuk.analogY >= 140){
      if(arr[0] >= CONTROLLER_X_MIN){
         arr[0] -=1;
         if(arr[1] <= CONTROLLER_Y_MAX){
            arr[1] +=1;
         }
         setPos(arr);
      }
   }
   if(nunchuk.analogX <=105 && nunchuk.analogY <=105){
      if(arr[0] >= CONTROLLER_X_MIN){
         arr[0] -= 1;
      }
      if(arr[1] >= CONTROLLER_Y_MIN){
         arr[1] -= 1;
      }
      setPos(arr);
   }
   if(nunchuk.analogX > 135 && nunchuk.analogY <105){
      if(arr[0] <= CONTROLLER_X_MAX){
         arr[0] += 1;
      }
      if(arr[1] >= CONTROLLER_Y_MIN){
         arr[1] -= 1;
      }
      setPos(arr);
   }*/
   
   //if(nunchuk.analogX <
   
   if(nunchuk.cButton == 1){
      if(arr[2] != CONTROLLER_MAX){
         arr[2] +=25;
      }
      
      Serial.print("\n");
      Serial.print(arr[5]);
      Serial.print("\n");
      setPos(arr);  
      
   }
   
   if(nunchuk.zButton == 1){
      if(arr[2] != CONTROLLER_MIN){
         arr[2] -=25;
      }
      Serial.print("\n");
      for(int i=0;i<6;i++){
        Serial.print(i);
        Serial.print("   ");
        Serial.print(arr[i]);
        Serial.print("\n");
      }
      Serial.print(arr[0]);
      Serial.print("\n");
      setPos(arr);
      
   }
    
}

void retPos(){
   for(int i=0;i<6;i++){
       long val;
       if(i<3){
           val=(long)(arr[i]*100*25.4);
       }else{
           val=(long)(arr[i]*100*deg2rad);
       }
       Serial.write(val);
       Serial.write((val>>8));
       Serial.write((val>>16));
       Serial.write((val>>24));
       Serial.flush();
   }
}

