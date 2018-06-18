#include <Battery.h>
//#include <TimerFa.h>

const int PWM_A = 13;
const int PWM_B = 4;
const int SOL_V = A0;
const int SOL_A = A1;
const int BAT_V = A2;
const int BAT_A = A3;
const float DC_V = 12;
const int BAT_ON = 8;
const int SOL_ON = 9;
const int SOC_MIN = 10;
const int SOC_MAX = 95;
float bus_V = 6;
float bat_V = 0;
float bat_A = 0;
float bat_P = 0;
float bat_SOC = 0;
float sol_V = 0;
float sol_A = 0;
float sol_P = 0;
float pre_sol_P = 0;
float P_net = 0;
bool PAO_direction = 0;
int mode = 0;
int duty_cycle_A = 0;
int duty_cycle_B = 0;
int unexpected = 0;

Battery bat = Battery(3000, 4200, BAT_V);
//TimerFa Timer1;

void setup()
{
  TCCR0B = (TCCR0B & 0b11111000) | 0x01;
  pinMode(BAT_ON, OUTPUT);
  pinMode(SOL_ON, OUTPUT);
  bat.begin(3300, 1.47, &sigmoidal);
  Serial.begin(115200);
}

void loop()
{
  digitalWrite(SOL_ON, HIGH);
  sol_V = float(analogRead(SOL_V)) * 5 / 1023 *13.02/3.26;
  if(sol_V < DC_V)
  {
    duty_cycle_A++;
  }
  else
  {
    duty_cycle_A--;
  }
  constrain(duty_cycle_A, 0, 255);
  analogWrite(PWM_A, duty_cycle_A);
  
  digitalWrite(BAT_ON,HIGH);
  analogWrite(PWM_B, 100);
  
  /*readADC();
  if (Timer1.MicrosaniyedeBir(500000L))
  {
    modeSet();
  }
  if(mode == 1)
  {
    regulateVolt();
  }
  else if(mode == 2 || mode == 3)
  {
    regulateVolt();
    mppt();
  }*/
}

void readADC()
{
 bat_V = bat.voltage() / 1000;
 bat_A = analogRead(BAT_A) * 5 / 1023;
 sol_V = analogRead(SOL_V) * 5 / 1023;
 sol_A = analogRead(SOL_A) * 5 / 1023;
 
 bat_P = bat_V * bat_A;
 bat_SOC = bat.level();
 pre_sol_P = sol_P;
 sol_P = sol_V * sol_A;
 
 P_net = bat_P - sol_P;
}

void modeSet()
{ 
  if(mode == 0)
  {
    mode = 3;
  }
  else
  {
    if(P_net > 0 && bat_SOC <= SOC_MIN)
    {
      mode = 0;
    }
    else if(P_net > 0 && bat_SOC > SOC_MIN)
    {
      mode = 1;
    }
    else if(P_net < 0 && bat_SOC >= SOC_MAX)
    {
      mode = 2;
    }
    else if(P_net < 0 && bat_SOC < SOC_MAX)
    {
      mode = 3;
    }
    else
    {
      unexpected++;
    }
  }
  
  switch(mode)
  {
    case 0:   pinMode(BAT_ON, LOW);
              pinMode(SOL_ON, LOW);
              break;
              
    case 1:   pinMode(BAT_ON, HIGH);
              pinMode(SOL_ON, LOW);
              break;
              
    case 2:   pinMode(BAT_ON, LOW);
              pinMode(SOL_ON, HIGH);
              break;
              
    case 3:   pinMode(BAT_ON, HIGH);
              pinMode(SOL_ON, HIGH);
              break;
  }
}

void regulateVolt()
{
  if(sol_V < DC_V)
  {
    duty_cycle_A += 1;
  }
  else
  {
    duty_cycle_A -= 1;
  }
  constrain(duty_cycle_A, 0, 255);
  analogWrite(PWM_A, duty_cycle_A);
}

void mppt()
{
  if(sol_P < pre_sol_P)
  {
    PAO_direction != PAO_direction;
  }
  if(PAO_direction == 0)
  {
    duty_cycle_A+=1;
  }
  else
  {
    duty_cycle_A-=1;
  } 
  constrain(duty_cycle_A, 0, 255);
  analogWrite(PWM_A, duty_cycle_A);
}

