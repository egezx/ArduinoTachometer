#include <MsTimer2.h>

void setup() 
{
  pinMode(A0, INPUT);
  Serial.begin(9600);

  //Interrupt is required because serial is too slow and will mess up our rpm measurement.
  MsTimer2::set(2000, WriteSerial);
  MsTimer2::start();
}

bool state = false;
int counter = 0;

unsigned long duration = 0;
unsigned long oldTime = micros();

int lightLowThreshold = 50; //Maximum amount of light accepted for low "state".
int lightHighThreshold = 100; //Minimum amount of light required for high "state".

int fanBladeCount = 7;

void WriteSerial()
{
  //Duration is expressed as microseconds. 1 second = 1 000 000 microseconds so we want to divide 1000000 by duration.
  //After division we know rounds per second so we finally have to multiply it by 60 to get rounds per minute.
  String rpmString = "";
  
  if(duration != 0)
    rpmString = "RPM: " + String((1000000 / duration) * 60); 
  else rpmString = "RPM: 0";
  
  
  Serial.println(rpmString);
}


void loop() 
{
  int reading  = analogRead(A0);

  
  if(state && reading < lightLowThreshold) state = false;
  
  else if (!state && reading > lightHighThreshold)
  {
      state = true;
      counter++;
  }


  //After we have detected desired amount of blade bypasses, we know that fan has rotated full 360 degrees.
  //Then we just check how much time has passed since last time fan was in same position.
  if(counter > 0 && counter % fanBladeCount == 0)
  {
    counter = 0;
    duration = micros() - oldTime;
    oldTime = micros();
  }
}
