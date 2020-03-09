#include <ZSharpIR.h>

int distance;
// Create a new instance of the SharpIR class:

ZSharpIR dist = ZSharpIR(A2, 1080);
int t = 0;

void setup() {
  
  Serial.begin(9600);

  Serial.println(" T    D");
}

void loop() {
  
  distance = dist.distance();
  Serial.println(String(t) + " " + String(distance));
  delay(1000);

  t++;
}
