

int led1=10;
int led2=9;
int led3=8;
int button1=7;
int button2=6;
int button3=5;
const int sequencemax=10;
int sequence[sequencemax];
int sequenceindex=0;


void setup() 
{

  pinMode(button1, INPUT_PULLUP);  
  pinMode(button2, INPUT_PULLUP);  
  pinMode(button3, INPUT_PULLUP); 
  Serial.begin(9600);
  randomSeed(analogRead(0));
  Serial.println("Test");
}

void loop() 
{
  if (digitalRead(button1) == LOW){  Serial.println("B1");}
    if (digitalRead(button2) == LOW){  Serial.println("B2");}

  if (digitalRead(button3) == LOW){  Serial.println("B3");}


}


