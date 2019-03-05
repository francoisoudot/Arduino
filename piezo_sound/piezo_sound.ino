

int output=6;
int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
void setup(){
  pinMode(output, OUTPUT);
}

void loop(){
  int i=0;
  while (i<10){
    for (int j=0; j<10;j++){
      tone(output,tones[j]);
      delay(500);
    }
    i=i++;
  }
 
}



