

int led1=10;
int led2=9;
int led3=8;
int button1=7;
int button2=6;
int button3=5;
const int sequencemax=100;
int sequence[sequencemax];
int sequenceindex=0;


void setup() 
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(button1, INPUT_PULLUP);  
  pinMode(button2, INPUT_PULLUP);  
  pinMode(button3, INPUT_PULLUP); 
  Serial.begin(9600);
  randomSeed(analogRead(0));
  Serial.println("Serial setup lesson 6");
  reset();
}

void loop() 
{
  int pin = random(8,11);
  sequence[sequenceindex] = pin;
  for (int i = 0; i <= sequenceindex;) {
    digitalWrite(sequence[i], HIGH);
    delay(1000);
    digitalWrite(sequence[i], LOW);
    delay(500);
    i++;
    Serial.println("LED");
  }
  sequenceindex++;
  Serial.println(sequenceindex);
  int lastPressed=0; //lastPressed is the LED # corresponding to the pressed button
  int current=0;
  while(current < sequenceindex) {
    while (digitalRead(button1) == LOW ||digitalRead(button2) == LOW ||digitalRead(button3) == LOW) {
      Serial.println("button loop");
      if (digitalRead(button1) == LOW) {
	lastPressed = led1;
	digitalWrite(led1, HIGH);
  	} 
      else {
      	digitalWrite(led1, LOW);
      }
      if (digitalRead(button2) == LOW) {
	lastPressed = led2;
	digitalWrite(led2, HIGH);
  	} 
      else {
      	digitalWrite(led2, LOW);
      }
      if (digitalRead(button3) == LOW) {
	lastPressed = led3;
	digitalWrite(led3, HIGH);
  	} 
      else {
      	digitalWrite(led3, LOW);
      }

    }
      digitalWrite(lastPressed, LOW);
      if (lastPressed!=0)
      {
        if (sequence[current] != lastPressed) {
	  
          Serial.println("BREAK");
          Serial.println(lastPressed);
          Serial.println("current");
          Serial.println(current);
          Serial.println(sequence[current]);
          Serial.println("sequenceindex");
          Serial.println(sequenceindex);
          Serial.println(sequence[sequenceindex]);
          reset();
          lastPressed = 0;
          
          break;
          
	} else {
	  lastPressed = 0;
          current++;
        }
      
      } 
  }
  

  good();
  fdelay(1000);
  
  if (sequenceindex==sequencemax)
  {
    for (int i = 0; i < sequencemax;) 
    {
      sequence[i]=0;
      i++;
    }
    reset();
  }
}


void reset()
{
  for (int i=0;i<=3;){
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  delay(500);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW); 
  delay(500);
  i++;
  }
  sequenceindex=0;
}

void good()
{
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  delay(500);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW); 
  delay(500);
}
