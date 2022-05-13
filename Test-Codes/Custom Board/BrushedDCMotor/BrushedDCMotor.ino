#define EN 5        //pin24 == PA15
#define PH 10       //pin27 == PA18
#define nSLEEP 13   //pin26 == PA17
#define BST_EN 15   //pin07 == PB08


void setup() {
  // put your setup code here, to run once:
  pinMode(EN,OUTPUT);   
  pinMode(PH,OUTPUT);
  pinMode(nSLEEP,OUTPUT);
  pinMode(BST_EN,OUTPUT);         
  digitalWrite(nSLEEP,HIGH);      // nSlEEP == 1 "To turn on the driver IC
  delay(2000);
  digitalWrite(BST_EN,HIGH);      // BST_EN == 1 "To Turn On MOSFET (FEEDBACK NETWORK)
  
}

void loop() {
  // put your main code here, to run repeatedly:

  /*
     PH/EN Control Mode
nSLEEP  EN  PH     OUT1    OUT2    DESCRIPTION
0       X   X       Hi-Z  Hi-Z    Sleep, (H-Bridge Hi-Z)
1       0   X       L       L     Brake, (Low-Side Slow Decay)
1       1   0       L       H     Reverse (OUT2 → OUT1)
1       1   1       H       L     Forward (OUT1 → OUT2)

  */
       //Forward Mode
       digitalWrite(PH,HIGH);   
       digitalWrite(EN,HIGH);  
       delay(5000);
       //Reverse Mode
       digitalWrite(PH,LOW);
       digitalWrite(EN,HIGH);
       delay(5000);
} 
